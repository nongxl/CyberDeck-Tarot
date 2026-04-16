#include <M5Cardputer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#if __has_include("src/secrets.h")
#include "src/secrets.h"
#endif

#include "src/tarot_engine.h"
#include "src/embedded_assets.h"
#include <vector>

#ifndef DEFAULT_API_KEY
#define DEFAULT_API_KEY ""
#endif

enum AppState {
    STATE_INIT,
    STATE_LANG_CHECK,
    STATE_LANG_SELECT,
    STATE_WIFI_CHECK,
    STATE_WIFI_SCAN,
    STATE_WIFI_SELECT,
    STATE_WIFI_PASSWD,
    STATE_WIFI_CONNECTING,
    STATE_SPREAD_SELECT,
    STATE_LLM_API_TEST,
    STATE_APP_READY,
    STATE_ERROR_SD
};

AppState currentState = STATE_INIT;

// Configuration
String ssid = "";
String password = "";

String apiUrl = "https://open.bigmodel.cn/api/paas/v4/chat/completions";
String apiModel = "glm-4-flash";
String apiKey = DEFAULT_API_KEY; // Hardcoded default from secrets.h
String appLang = ""; // "en" or "zh"
uint32_t wifiConnectTimeoutStart = 0; // Fixed timer for connections

// IMU State
float imu_roll_f = 0.0f;
float imu_pitch_f = 0.0f;
int global_parallaxX = 0;
int global_parallaxY = 0;

// Display constants - Status bar removed
const int TOP_BAR_HEIGHT = 0;
const int TERM_START_Y = 0;

// Scan state
int numNetworks = 0;
int selectedNetwork = 0;
int startNetworkIndex = 0;
const int MAX_NETWORKS_DISPLAY = 8; // More space now
int selectedLang = 0;

// Spread state
int selectedSpread = 0;
SpreadType currentSpread = SPREAD_SINGLE;

// Scrolling History
struct TerminalElement {
    enum Type { TEXT, CARDS, CELTIC_CROSS_TOP };
    Type type;
    String text;
    uint16_t color;

    // Card row data
    std::vector<int> cardIds;
    std::vector<bool> reversed;
    std::vector<String> positionNames;
    std::vector<String> cardNames;
    int scrollX;
    int scrollY;
    int revealedCount; // 0=all face-down, cardIds.size()=all revealed

    TerminalElement(String t, uint16_t c) : type(TEXT), text(t), color(c), scrollX(0), scrollY(0), revealedCount(0) {}
    TerminalElement(const std::vector<int>& ids, const std::vector<bool>& rev,
                    const std::vector<String>& posNames, const std::vector<String>& cNames, Type typeOverride = CARDS)
        : type(typeOverride), cardIds(ids), reversed(rev), positionNames(posNames), cardNames(cNames), scrollX(0), scrollY(0), revealedCount(0) {}
};


std::vector<TerminalElement> history;
int scrollLine = 0; // index of first visible element
uint32_t lastScrollMillis = 0;
const int VISIBLE_LINES = 8;
int interpretStartLine = 0; // Lock view during reading to skip cards

// Terminal display
M5Canvas terminal(&M5Cardputer.Display);

String L(String en, String zh) {
    if (appLang == "zh") return zh;
    return en;
}

int getUTF8CharLen(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

void getPngSize(const uint8_t* data, uint32_t size, int32_t& w, int32_t& h) {
    if (size < 24) return;
    // PNG IHDR width is at 16 (4 bytes BE), height at 20 (4 bytes BE)
    w = (data[16] << 24) | (data[17] << 16) | (data[18] << 8) | data[19];
    h = (data[20] << 24) | (data[21] << 16) | (data[22] << 8) | data[23];
}

void popTerm(int count = 1) {
    for (int i = 0; i < count && !history.empty(); i++) {
        history.pop_back();
    }
}

// Count lines needed to wrap 'text' within maxW pixels
static int countWrapLines(M5Canvas& cv, const String& text, int maxW) {
    if (text.length() == 0) return 0;
    int lines = 1;
    String cur = "";
    for (int i = 0; i < (int)text.length(); ) {
        int clen = getUTF8CharLen((unsigned char)text[i]);
        if (i + clen > (int)text.length()) clen = text.length() - i;
        String ch = text.substring(i, i + clen);
        i += clen;
        if (cv.textWidth(cur + ch) > maxW) { lines++; cur = ch; }
        else cur += ch;
    }
    return lines;
}

// Render wrapped text, returns number of lines printed
static int renderWrappedLines(M5Canvas& cv, const String& text, int x, int startY, int maxW, int lineH) {
    String rem = text;
    int rendered = 0;
    while (rem.length() > 0) {
        String cur = "";
        int i = 0;
        while (i < (int)rem.length()) {
            int clen = getUTF8CharLen((unsigned char)rem[i]);
            if (i + clen > (int)rem.length()) clen = rem.length() - i;
            String ch = rem.substring(i, i + clen);
            if (cv.textWidth(cur + ch) > maxW) break;
            cur += ch; i += clen;
        }
        if (cur.length() == 0) break;
        cv.setCursor(x, startY + rendered * lineH);
        cv.print(cur);
        rem = rem.substring(i);
        rendered++;
    }
    return rendered;
}


// Draw the terminal window from a specific scroll offset
void redrawTerminal() {
    terminal.fillSprite(TFT_BLACK);
    int fontHeight = terminal.fontHeight();
    int curY = 0;
    
    for (int i = scrollLine; i < (int)history.size() && curY < 135; i++) {
        if (history[i].type == TerminalElement::TEXT) {
            terminal.setTextColor(history[i].color);
            terminal.setCursor(0, curY);
            terminal.print(history[i].text);
            curY += fontHeight + 2;
        } else if (history[i].type == TerminalElement::CARDS) {
            const int cardW      = 80;
            const int cardImgH   = 105; // fills screen: 105 + 2*14 + 2 = 135px
            const int lineH      = fontHeight;
            const int labelMaxW  = cardW - 2;
            const int cardRowH   = cardImgH + lineH * 2 + 2;

            int totalW = history[i].cardIds.size() * cardW;
            int startX = (totalW < 240) ? (240 - totalW) / 2 : 0;
            int revealed = history[i].revealedCount;

            // Pass 1: Draw card images
            for (int cIdx = 0; cIdx < (int)history[i].cardIds.size(); cIdx++) {
                int cardX = startX + cIdx * cardW - history[i].scrollX;
                if (cardX > -cardW && cardX < 240) {
                    if (cIdx < revealed) {
                        // --- Face-up: draw actual card image ---
                        TarotCard card;
                        loadCard(history[i].cardIds[cIdx], card);
                        if (card.imagePtr) {
                            int32_t iw = 0, ih = 0;
                            getPngSize(card.imagePtr, card.imageSize, iw, ih);
                            float sx = (iw > 0) ? (float)cardW / iw : 1.0f;
                            float sy = (ih > 0) ? (float)cardImgH / ih : 1.0f;
                            bool isRev = !history[i].reversed.empty() && history[i].reversed[cIdx];
                            if (isRev) {
                                M5Canvas cardSprite(&terminal);
                                cardSprite.createSprite(cardW, cardImgH);
                                cardSprite.drawPng(card.imagePtr, card.imageSize, 0, 0, cardW, cardImgH, 0, 0, sx, sy);
                                cardSprite.pushRotateZoom(&terminal, cardX + cardW/2, curY + cardImgH/2, 180.0f, 1.0f, 1.0f);
                                cardSprite.deleteSprite();
                            } else {
                                terminal.drawPng(card.imagePtr, card.imageSize, cardX, curY, 0, 0, 0, 0, sx, sy);
                            }
                        }
                    } else {
                        // --- Face-down: draw card back ---
                        size_t backSize = _binary_assets_cards_back_png_end - _binary_assets_cards_back_png_start;
                        int32_t bw = 0, bh = 0;
                        getPngSize(_binary_assets_cards_back_png_start, backSize, bw, bh);
                        float bsx = (bw > 0) ? (float)cardW / bw : 1.0f;
                        float bsy = (bh > 0) ? (float)cardImgH / bh : 1.0f;
                        terminal.drawPng(_binary_assets_cards_back_png_start, backSize, cardX, curY, 0, 0, 0, 0, bsx, bsy);
                    }
                }
            }

            // Pass 2: Position name label (yellow, only for revealed cards fully on-screen)
            for (int cIdx = 0; cIdx < revealed && cIdx < (int)history[i].cardIds.size(); cIdx++) {
                int cardX = startX + cIdx * cardW - history[i].scrollX;
                if (cardX >= 0 && cardX + cardW <= 240 && cIdx < (int)history[i].positionNames.size()) {
                    terminal.setTextColor(TFT_YELLOW);
                    String posLabel = history[i].positionNames[cIdx];
                    while (posLabel.length() > 0 && terminal.textWidth(posLabel) > labelMaxW) {
                        int j = posLabel.length() - 1;
                        while (j > 0 && ((unsigned char)posLabel[j] & 0xC0) == 0x80) j--;
                        posLabel = posLabel.substring(0, j);
                    }
                    terminal.setCursor(cardX + 1, curY + cardImgH + 1);
                    terminal.print(posLabel);
                }
            }

            // Pass 3: Card name label (colored by orientation, only for revealed cards fully on-screen)
            for (int cIdx = 0; cIdx < revealed && cIdx < (int)history[i].cardIds.size(); cIdx++) {
                int cardX = startX + cIdx * cardW - history[i].scrollX;
                if (cardX >= 0 && cardX + cardW <= 240 && cIdx < (int)history[i].cardNames.size()) {
                    bool isRev = !history[i].reversed.empty() && history[i].reversed[cIdx];
                    terminal.setTextColor(isRev ? TFT_RED : TFT_WHITE);
                    String nameLabel = history[i].cardNames[cIdx];
                    while (nameLabel.length() > 0 && terminal.textWidth(nameLabel) > labelMaxW) {
                        int j = nameLabel.length() - 1;
                        while (j > 0 && ((unsigned char)nameLabel[j] & 0xC0) == 0x80) j--;
                        nameLabel = nameLabel.substring(0, j);
                    }
                    terminal.setCursor(cardX + 1, curY + cardImgH + 1 + lineH);
                    terminal.print(nameLabel);
                }
            }

            curY += cardRowH;
        } else if (history[i].type == TerminalElement::CELTIC_CROSS_TOP) {
            const int cardW      = 80;
            const int cardImgH   = 105;
            const int lineH      = fontHeight;
            const int labelMaxW  = cardW - 2;
            const int rowH       = cardImgH + lineH * 2 + 2; // 135px
            int revealed         = history[i].revealedCount;

            auto drawCardNode = [&](int cIdx, int x, int yBase, bool rotated = false, int zLevel = 0) {
                if (cIdx >= (int)history[i].cardIds.size()) return;
                
                int pX = (global_parallaxX * zLevel) / 5;
                int pY = (global_parallaxY * zLevel) / 5;
                int finalX = x + pX;
                int finalY = curY + yBase + pY;

                // Slightly looser culling to allow 3D peeking
                if (finalY + cardImgH <= -40 || finalY >= 135 + 40) return; 

                // Draw Drop Shadow
                if (zLevel > 0) {
                    terminal.fillRectAlpha(finalX + 4 + pX/2, finalY + 4 + pY/2, 
                                           rotated ? cardImgH : cardW, 
                                           rotated ? cardW : cardImgH, 
                                           150, TFT_BLACK);
                }

                if (cIdx < revealed) {
                    TarotCard card;
                    loadCard(history[i].cardIds[cIdx], card);
                    if (card.imagePtr) {
                        int32_t iw = 0, ih = 0;
                        getPngSize(card.imagePtr, card.imageSize, iw, ih);
                        float sx = (iw > 0) ? (float)cardW / iw : 1.0f;
                        float sy = (ih > 0) ? (float)cardImgH / ih : 1.0f;
                        bool isRev = !history[i].reversed.empty() && history[i].reversed[cIdx];
                        
                        M5Canvas cardSprite(&terminal);
                        cardSprite.createSprite(cardW, cardImgH);
                        cardSprite.drawPng(card.imagePtr, card.imageSize, 0, 0, cardW, cardImgH, 0, 0, sx, sy);
                        
                        float angle = 0.0f;
                        if (isRev) angle += 180.0f;
                        if (rotated) angle += 90.0f; 
                        
                        if (angle != 0.0f) {
                            cardSprite.pushRotateZoom(&terminal, finalX + cardW/2, finalY + cardImgH/2, angle, 1.0f, 1.0f);
                        } else {
                            cardSprite.pushSprite(finalX, finalY);
                        }
                        cardSprite.deleteSprite();
                    }
                } else {
                    size_t backSize = _binary_assets_cards_back_png_end - _binary_assets_cards_back_png_start;
                    int32_t bw = 0, bh = 0;
                    getPngSize(_binary_assets_cards_back_png_start, backSize, bw, bh);
                    float bsx = (bw > 0) ? (float)cardW / bw : 1.0f;
                    float bsy = (bh > 0) ? (float)cardImgH / bh : 1.0f;
                    
                    if (rotated) {
                        M5Canvas cardSprite(&terminal);
                        cardSprite.createSprite(cardW, cardImgH);
                        cardSprite.drawPng(_binary_assets_cards_back_png_start, backSize, 0, 0, cardW, cardImgH, 0, 0, bsx, bsy);
                        cardSprite.pushRotateZoom(&terminal, finalX + cardW/2, finalY + cardImgH/2, 90.0f, 1.0f, 1.0f);
                        cardSprite.deleteSprite();
                    } else {
                        terminal.drawPng(_binary_assets_cards_back_png_start, backSize, finalX, finalY, 0, 0, 0, 0, bsx, bsy);
                    }
                }
            };
            
            auto drawLabelOverlay = [&](int cIdx, int x, int yBase, bool alignTop, int zLevel = 0) {
                if (cIdx >= revealed || cIdx >= (int)history[i].cardIds.size()) return;
                int pX = (global_parallaxX * zLevel) / 5;
                int pY = (global_parallaxY * zLevel) / 5;
                int finalX = x + pX;
                int finalY = curY + yBase + pY;
                
                int badgeH = lineH * 2 + 2;
                int rectY = alignTop ? finalY : (finalY + cardImgH - badgeH);
                
                terminal.fillRectAlpha(finalX, rectY, cardW, badgeH, 200, TFT_BLACK);
                
                terminal.setTextColor(TFT_YELLOW);
                String posLabel = history[i].positionNames[cIdx];
                while (posLabel.length() > 0 && terminal.textWidth(posLabel) > labelMaxW) {
                    posLabel = posLabel.substring(0, posLabel.length() - 1);
                }
                terminal.setCursor(finalX + 1, rectY + 1);
                terminal.print(posLabel);
                
                bool isRev = !history[i].reversed.empty() && history[i].reversed[cIdx];
                terminal.setTextColor(isRev ? TFT_RED : TFT_WHITE);
                String nameLabel = history[i].cardNames[cIdx];
                while (nameLabel.length() > 0 && terminal.textWidth(nameLabel) > labelMaxW) {
                    nameLabel = nameLabel.substring(0, nameLabel.length() - 1);
                }
                terminal.setCursor(finalX + 1, rectY + 1 + lineH);
                terminal.print(nameLabel);
            };

            terminal.fillRect(0, curY, 240, 135, TFT_BLACK);

            if (2 < (int)history[i].cardIds.size()) {
                drawCardNode(2, 80, 55, false, 0); if(2 < revealed) drawLabelOverlay(2, 80, 55, false, 0);
            }
            if (4 < (int)history[i].cardIds.size()) {
                drawCardNode(4, 80, -15, false, 1); if(4 < revealed) drawLabelOverlay(4, 80, -15, true, 1);
            }
            if (3 < (int)history[i].cardIds.size()) {
                drawCardNode(3, 0, 20, false, 2); if(3 < revealed) drawLabelOverlay(3, 0, 20, false, 2);
            }
            if (5 < (int)history[i].cardIds.size()) {
                drawCardNode(5, 160, 20, false, 2); if(5 < revealed) drawLabelOverlay(5, 160, 20, false, 2);
            }
            if (0 < (int)history[i].cardIds.size()) {
                drawCardNode(0, 80, 20, false, 3);
                if (0 < revealed && revealed <= 1) drawLabelOverlay(0, 80, 20, false, 3); 
            }
            if (1 < (int)history[i].cardIds.size()) {
                drawCardNode(1, 80, 20, true, 5); 
                if (1 < revealed) {
                    int zLevel = 5;
                    int pX = (global_parallaxX * zLevel) / 5;
                    int pY = (global_parallaxY * zLevel) / 5;
                    int finalX = 80 + pX;
                    int finalY = curY + 20 + pY;

                    int badgeH = lineH * 2 + 2;
                    int rectY = finalY + cardImgH/2 - badgeH/2;
                    terminal.fillRectAlpha(finalX, rectY, cardW, badgeH, 200, TFT_BLACK);
                    terminal.setTextColor(TFT_YELLOW);
                    terminal.setCursor(finalX + 1, rectY + 1);
                    terminal.print(history[i].positionNames[1]); // Challenge
                    bool isRev = !history[i].reversed.empty() && history[i].reversed[1];
                    terminal.setTextColor(isRev ? TFT_RED : TFT_WHITE);
                    String nameLabel = history[i].cardNames[1];
                    while (nameLabel.length() > 0 && terminal.textWidth(nameLabel) > labelMaxW) {
                        nameLabel = nameLabel.substring(0, nameLabel.length() - 1);
                    }
                    terminal.setCursor(finalX + 1, rectY + 1 + lineH);
                    terminal.print(nameLabel);
                }
            }

            curY += 135; 
        }
    }

    // Draw scrollbar if content exceeds screen
    if (history.size() > (size_t)VISIBLE_LINES) {
        int barHeight = max(10, (135 * VISIBLE_LINES) / (int)history.size());
        int barY = (135 * scrollLine) / (int)history.size();
        terminal.fillRect(237, 0, 3, 135, TFT_DARKGREY);
        terminal.fillRect(237, barY, 3, barHeight, TFT_WHITE);
    }

    terminal.pushSprite(0, 0);
}

// Advanced print that handles word wrapping into history
void printTerm(String msg, uint16_t color = TFT_WHITE, bool scrollToEnd = true, bool redraw = true) {
    // Split message by newline first
    int startIdx = 0;
    while (startIdx < msg.length()) {
        int nextNewLine = msg.indexOf('\n', startIdx);
        String linePart = (nextNewLine == -1) ? msg.substring(startIdx) : msg.substring(startIdx, nextNewLine);
        
        // Wrap linePart into 240px width
        String currentLine = "";
        for (int i = 0; i < (int)linePart.length(); ) {
            int clen = getUTF8CharLen((unsigned char)linePart[i]);
            if (i + clen > (int)linePart.length()) clen = linePart.length() - i;
            String sChar = linePart.substring(i, i + clen);
            i += clen;

            String testLine = currentLine + sChar;
            if (terminal.textWidth(testLine) > 235) {
                history.push_back(TerminalElement(currentLine, color));
                currentLine = sChar;
            } else {
                currentLine = testLine;
            }
        }
        if (currentLine.length() > 0) {
            history.push_back(TerminalElement(currentLine, color));
        } else if (nextNewLine != -1) {
            // Handled explicit empty line from \n\n or trailing \n
            history.push_back(TerminalElement("", color));
        }
        
        if (nextNewLine == -1) break;
        startIdx = nextNewLine + 1;
        // If message ends with \n, add one more empty line
        if (startIdx == (int)msg.length()) {
            history.push_back(TerminalElement("", color));
        }
    }
    
    if (scrollToEnd) {
        if ((int)history.size() > VISIBLE_LINES) {
            scrollLine = history.size() - VISIBLE_LINES;
        } else {
            scrollLine = 0;
        }
    }
    if (redraw) redrawTerminal();
}

// Special version for streaming deltas
void appendTerm(String delta, uint16_t color = TFT_WHITE, bool redraw = true) {
    if (delta.length() == 0) return;
    
    // If history is empty, start one
    if (history.empty()) {
        history.push_back(TerminalElement("", color));
    }
    
    for (int i = 0; i < (int)delta.length(); ) {
        int clen = getUTF8CharLen((unsigned char)delta[i]);
        if (i + clen > (int)delta.length()) clen = delta.length() - i;
        String sChar = delta.substring(i, i + clen);
        i += clen;

        if (sChar == "\n") {
            history.push_back(TerminalElement("", color));
            continue;
        }
        
        TerminalElement &last = history.back();
        if (last.type == TerminalElement::CARDS) {
             history.push_back(TerminalElement(sChar, color));
             continue;
        }
        
        // If the line is empty (e.g. just created by a newline), adopt the new color
        if (last.text.length() == 0) {
            last.color = color;
        }

        String test = last.text + sChar;
        if (terminal.textWidth(test) > 235) {
            history.push_back(TerminalElement(sChar, color));
        } else {
            last.text = test;
        }
    }
    
    if (history.size() > VISIBLE_LINES) {
        scrollLine = max(interpretStartLine, (int)history.size() - VISIBLE_LINES);
    }
    if (redraw) redrawTerminal();
}

void drawLangList() {
    history.clear();
    scrollLine = 0;
    printTerm("Select Language / 选择语言:", TFT_YELLOW);
    printTerm("(UP: ; DOWN: . ENTER: Ent)", TFT_DARKGREY);
    
    // We can't easily use printTerm for inverted colors in this list without a custom renderer
    // So for lists we'll just draw directly to terminal temporarily
    terminal.fillSprite(TFT_BLACK);
    terminal.setCursor(0, 0);
    terminal.setTextColor(TFT_YELLOW); terminal.println("Select Language / 选择语言:");
    terminal.setTextColor(TFT_DARKGREY); terminal.println("(UP: ; DOWN: . ENTER: Ent)");
    
    if (selectedLang == 0) terminal.setTextColor(TFT_BLACK, TFT_WHITE);
    else terminal.setTextColor(TFT_WHITE, TFT_BLACK);
    terminal.println("1. English");
    
    if (selectedLang == 1) terminal.setTextColor(TFT_BLACK, TFT_WHITE);
    else terminal.setTextColor(TFT_WHITE, TFT_BLACK);
    terminal.println("2. 简体中文");
    
    terminal.pushSprite(0, 0);
}

void drawSpreadList() {
    terminal.fillSprite(TFT_BLACK);
    terminal.setCursor(0, 0);
    terminal.setTextColor(TFT_YELLOW); 
    terminal.setTextSize(1.1, 1.1); // Increase title size
    terminal.println(L("Form the Ritual Circle:", "布设仪式之阵："));
    terminal.setTextSize(1.0, 1.0); // Reset size
    
    terminal.setCursor(0, terminal.getCursorY() + 6); // Space after title

    for (int i = 0; i < NUM_SPREADS; i++) {
        if (i == selectedSpread) terminal.setTextColor(TFT_BLACK, TFT_WHITE);
        else terminal.setTextColor(TFT_WHITE, TFT_BLACK);
        
        SpreadDefinition sd;
        loadSpread((SpreadType)i, sd);
        
        char bufEN[64];
        char bufZH[64];
        strncpy_P(bufEN, sd.name, sizeof(bufEN)); bufEN[sizeof(bufEN)-1] = '\0';
        strncpy_P(bufZH, sd.zh_name, sizeof(bufZH)); bufZH[sizeof(bufZH)-1] = '\0';
        
        terminal.println(String(i+1) + ". " + L(bufEN, bufZH));
        terminal.setCursor(0, terminal.getCursorY() + 4); // Increase line spacing
    }
    
    // Draw card back at the right side (Moved left by 10 from 145)
    size_t backSize = _binary_assets_cards_back_png_end - _binary_assets_cards_back_png_start;
    int32_t bw = 0, bh = 0;
    getPngSize(_binary_assets_cards_back_png_start, backSize, bw, bh);
    float bsx = (bw > 0) ? (float)80 / bw : 1.0f;
    float bsy = (bh > 0) ? (float)105 / bh : 1.0f;
    terminal.drawPng(_binary_assets_cards_back_png_start, backSize, 135, 15, 0, 0, 0, 0, bsx, bsy);

    // Breathing effect (pulsing brightness via black overlay)
    float s = sin(millis() / 450.0f); 
    int alpha = 110 + (int)(s * 90); // Range approx 20-200
    terminal.fillRectAlpha(135, 15, 80, 105, alpha, TFT_BLACK);

    terminal.pushSprite(0, 0);
}

void loadWifiConfig() {
    if (SD.exists("/CyberDeck-Tarot/wifi.json")) {
        File file = SD.open("/CyberDeck-Tarot/wifi.json");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        if (!error) {
            ssid = doc["ssid"] | "";
            password = doc["password"] | "";
        }
        file.close();
    }
}

void saveWifiConfig() {
    File file = SD.open("/CyberDeck-Tarot/wifi.json", FILE_WRITE);
    if (file) {
        JsonDocument doc;
        doc["ssid"] = ssid;
        doc["password"] = password;
        serializeJson(doc, file);
        file.close();
    }
}

void loadLLMConfig() {
    if (SD.exists("/CyberDeck-Tarot/config.json")) {
        File file = SD.open("/CyberDeck-Tarot/config.json");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        if (!error) {
            apiUrl = doc["url"] | apiUrl;
            apiModel = doc["model"] | apiModel;
            String loadedKey = doc["api_key"] | "";
            if (loadedKey != "") apiKey = loadedKey;
            appLang = doc["language"] | appLang;
        }
        file.close();
    }
}

void saveLLMConfig() {
    File file = SD.open("/CyberDeck-Tarot/config.json", FILE_WRITE);
    if (file) {
        JsonDocument doc;
        // Only save UI/System preferences. 
        // Backend/Model configurations (url, model, api_key) are treated as read-only 
        // from the device UI to prevent built-in key leakage and preserve manual user edits.
        doc["language"] = appLang;
        serializeJson(doc, file);
        file.close();
    }
}

void drawWifiList() {
    terminal.fillSprite(TFT_BLACK);
    terminal.setCursor(0, 0);
    terminal.setTextColor(TFT_YELLOW); 
    terminal.println(L("Select WiFi (UP: ; DOWN: .):", "选择WiFi (上: ; 下: .):"));
    for (int i = 0; i < MAX_NETWORKS_DISPLAY && (i + startNetworkIndex) < numNetworks; i++) {
        int idx = i + startNetworkIndex;
        if (idx == selectedNetwork) {
            terminal.setTextColor(TFT_BLACK, TFT_WHITE);
        } else {
            terminal.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        String entry = String(idx + 1) + ". " + WiFi.SSID(idx);
        terminal.println(entry);
    }
    terminal.pushSprite(0, 0);
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextFont(&fonts::efontCN_14);
    
    // Set up terminal to full screen
    terminal.createSprite(240, 135);
    terminal.setTextFont(&fonts::efontCN_14);
    // Line spacing will be handled manually in redrawTerminal()
    
    SPI.begin(40, 39, 14, 12);
    if (!SD.begin(12, SPI, 15000000)) { 
        M5Cardputer.Display.fillRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height(), TFT_RED);
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
        M5Cardputer.Display.setTextDatum(middle_center);
        M5Cardputer.Display.drawString("SOUL MISSING / 魂龛缺失", M5Cardputer.Display.width()/2, M5Cardputer.Display.height()/2);
        currentState = STATE_ERROR_SD;
        return;
    }
    
    if (!SD.exists("/CyberDeck-Tarot")) {
        SD.mkdir("/CyberDeck-Tarot");
    }
    
    currentState = STATE_LANG_CHECK;
}

void runLLMTest() {
    if (apiKey == "") {
        printTerm(L("Error: API Key is missing!", "错误：缺少 API 密钥！"), TFT_RED);
        printTerm(L("Please set 'api_key' in SD card:", "请在 SD 卡配置文件中设置："), TFT_WHITE);
        printTerm("/CyberDeck-Tarot/config.json", TFT_YELLOW);
        return;
    }

    std::vector<DrawnCard> drawn = shuffleAndDraw(currentSpread);

    // Remove the "Shuffling..." status line before showing cards
    popTerm(1);

    // Build card metadata
    SpreadDefinition spread;
    loadSpread(currentSpread, spread);

    std::vector<int> cardIds;
    std::vector<bool> revList;
    std::vector<String> posNames;
    std::vector<String> cNames;

    for (const auto& d : drawn) {
        cardIds.push_back(d.cardId);
        revList.push_back(d.reversed);

        TarotCard tc;
        loadCard(d.cardId, tc);
        char cNameBuf[64];
        strncpy_P(cNameBuf, tc.name, sizeof(cNameBuf));
        cNameBuf[sizeof(cNameBuf)-1] = '\0';
        char cNumBuf[8];
        strncpy_P(cNumBuf, tc.numeral, sizeof(cNumBuf));
        cNumBuf[sizeof(cNumBuf)-1] = '\0';
        // Label: localized card name + numeral (e.g. "愚者 0" or "The Fool 0")
        cNames.push_back(L(String(cNameBuf), getCardZhName(d.cardId)) + " " + String(cNumBuf));

        char posNameBuf[64];
        strncpy_P(posNameBuf, (const char*)pgm_read_ptr(&(spread.positions[d.positionIndex])), sizeof(posNameBuf));
        posNameBuf[sizeof(posNameBuf)-1] = '\0';
        posNames.push_back(String(posNameBuf));
    }

    // Push CARDS element(s) — all cards start face-down (revealedCount = 0)
    int crossStaffIndex = -1;
    if (currentSpread == SPREAD_CELTIC_CROSS) {
        std::vector<int> tIds(cardIds.begin(), cardIds.begin() + 6);
        std::vector<bool> tRev(revList.begin(), revList.begin() + 6);
        std::vector<String> tPos(posNames.begin(), posNames.begin() + 6);
        std::vector<String> tNames(cNames.begin(), cNames.begin() + 6);

        std::vector<int> sIds(cardIds.begin() + 6, cardIds.end());
        std::vector<bool> sRev(revList.begin() + 6, revList.end());
        std::vector<String> sPos(posNames.begin() + 6, posNames.end());
        std::vector<String> sNames(cNames.begin() + 6, cNames.end());

        history.push_back(TerminalElement(tIds, tRev, tPos, tNames, TerminalElement::CELTIC_CROSS_TOP));
        crossStaffIndex = history.size(); // Index of the staff element
        history.push_back(TerminalElement(sIds, sRev, sPos, sNames, TerminalElement::CARDS));
    } else {
        history.push_back(TerminalElement(cardIds, revList, posNames, cNames));
    }

    interpretStartLine = 0; // Reset for new session
    scrollLine = 0; // show from card area top
    redrawTerminal();
    delay(300);

    // === Card Reveal Animation ===
    int startElIdx = history.size() - ((currentSpread == SPREAD_CELTIC_CROSS) ? 2 : 1);
    for (int elIdx = startElIdx; elIdx < (int)history.size(); elIdx++) {
        TerminalElement& cardEl = history[elIdx];
        int cardW = 80;
        int totalW = cardEl.cardIds.size() * cardW;
        int startX = (totalW < 240) ? (240 - totalW) / 2 : 0;

        if (elIdx == crossStaffIndex) {
            // Reached the staff, jump scrollLine exactly to this staff element so it renders properly in terminal
            scrollLine = elIdx;
            redrawTerminal();
        }

        for (int cIdx = 0; cIdx < (int)cardEl.cardIds.size(); cIdx++) {
            if (cardEl.type == TerminalElement::CELTIC_CROSS_TOP) {
                // Wait briefly before revealing
                delay(100);
            } else if (cardEl.type == TerminalElement::CARDS) {
                // Auto horizontal scroll if the next card to reveal is off-screen
                int targetRight = startX + (cIdx + 1) * cardW;
                if (targetRight > cardEl.scrollX + 240) {
                    int targetScroll = targetRight - 240;
                    while (cardEl.scrollX < targetScroll) {
                        cardEl.scrollX += 20;
                        if (cardEl.scrollX > targetScroll) cardEl.scrollX = targetScroll;
                        redrawTerminal();
                        delay(20); 
                    }
                    delay(100); 
                }
            }

            // Brief pause viewing the back
            delay(250);
            // Reveal this card
            cardEl.revealedCount = cIdx + 1;
            redrawTerminal();
            delay(350); 
        }
    }

    // === Pause for suspense (No text, just full cards) ===
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) break;
        delay(30);
    }

    // === Auto-scroll to blank area below cards ===
    int cardsIdx = (int)history.size() - 1;
    interpretStartLine = cardsIdx + 1; // Lock auto-scroll above the cards
    // Add the "thinking" message BELOW the cards, no immediate redraw
    printTerm(L("The Infinite whisper to Aurelia...", "无尽的虚空正向 Aurelia 低语..."), TFT_YELLOW, false, false);
    // Scroll to show the thinking message (not the cards)
    scrollLine = interpretStartLine;
    redrawTerminal();
    delay(200);

    String sysPrompt = buildSystemPrompt(currentSpread, appLang);
    String userMsg = buildUserMessage(L("What do the cards reveal?", "牌面揭示了什么？"), drawn, currentSpread);

    HTTPClient http;
    http.setTimeout(45000);
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    if (apiKey != "") {
        http.addHeader("Authorization", "Bearer " + apiKey);
    }
    http.addHeader("Accept", "text/event-stream");

    JsonDocument doc;
    doc["model"] = apiModel;
    doc["stream"] = true;
    JsonArray messages = doc["messages"].to<JsonArray>();

    JsonObject sys = messages.add<JsonObject>();
    sys["role"] = "system";
    sys["content"] = sysPrompt;

    JsonObject usr = messages.add<JsonObject>();
    usr["role"] = "user";
    usr["content"] = userMsg;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    if (httpCode == HTTP_CODE_OK) {
        // Replace "looking..." with reading header, no immediate redraw from prints
        popTerm(1);
        printTerm(L("Echoes from the Astral Plane:\n", "来自星界的回响：\n"), TFT_CYAN, false, false);
        // Keep scrollLine at cardsIdx+1 to stay in the text area
        scrollLine = cardsIdx + 1;
        redrawTerminal();

        WiFiClient* stream = http.getStreamPtr();
        String charBuffer = "";
        bool doneReceived = false;

        while (http.connected() && (!doneReceived || charBuffer.length() > 0)) {
            // 1. Ingest as much as possible from network to the buffer
            while (stream->available() > 0) {
                String line = stream->readStringUntil('\n');
                if (line.startsWith("data: ")) {
                    String dataJson = line.substring(6);
                    if (dataJson.indexOf("[DONE]") != -1) {
                        doneReceived = true;
                        break;
                    }
                    JsonDocument chunkDoc;
                    if (!deserializeJson(chunkDoc, dataJson)) {
                        const char* textDelta = chunkDoc["choices"][0]["delta"]["content"];
                        if (textDelta) charBuffer += String(textDelta);
                    }
                }
            }

            // 2. Pump one UTF-8 character from buffer to screen
            if (charBuffer.length() > 0) {
                int clen = getUTF8CharLen((unsigned char)charBuffer[0]);
                if (clen > (int)charBuffer.length()) clen = charBuffer.length(); 
                String sChar = charBuffer.substring(0, clen);
                charBuffer = charBuffer.substring(clen);

                // Print the character with font-compatibility replacement
                String printChar = sChar;
                if (sChar == "—" || sChar == "–") printChar = "-"; 
                else if (sChar == "…") printChar = "...";
                else if (sChar == "“" || sChar == "”") printChar = "\"";
                else if (sChar == "‘" || sChar == "’") printChar = "'";
                else if (sChar == "·") printChar = "·"; // Keep middle dot if supported, else fix

                appendTerm(printChar, TFT_WHITE);
                
                // Breath Pacing logic
                int wait = 50; // base speed (User adjusted)
                if (sChar == "。" || sChar == "！" || sChar == "？" || sChar == "." || sChar == "!" || sChar == "?") wait = 450;
                else if (sChar == "，" || sChar == "；" || sChar == "," || sChar == ";") wait = 300;
                else if (sChar == "\n") wait = 600;
                
                // Smoothly update while waiting to maintain IMU parallax if someone moves it
                uint32_t waitStart = millis();
                while (millis() - waitStart < (uint32_t)wait) {
                    delay(10);
                }
            } else {
                if (doneReceived) break;
                delay(10);
            }
        }
        printTerm(L("\n[The Stars have spoken]", "\n[众星已启示]"), TFT_GREEN, false);
        printTerm(L("Traverse the stars with ; and . / ENTER for a new vision.", "以 ; 和 . 键拨动星河 / 回车召唤新幻象。"), TFT_DARKGREY, false);
        redrawTerminal();

    } else {
        printTerm(L("The Arcane path is blocked: ", "奥术路径受阻: ") + String(http.errorToString(httpCode).c_str()), TFT_RED);
        if (httpCode > 0) printTerm(http.getString(), TFT_RED);
    }
    http.end();
}

void loop() {
    M5Cardputer.update();
    
    switch (currentState) {
        case STATE_LANG_CHECK:
            loadLLMConfig();
            if (appLang == "") {
                selectedLang = 0;
                drawLangList();
                currentState = STATE_LANG_SELECT;
            } else {
                history.clear();
                printTerm(L("Aligning with the Celestial Spheres...", "正在对齐天球运行..."), TFT_GREEN);
                currentState = STATE_WIFI_CHECK;
            }
            break;
            
        case STATE_LANG_SELECT:
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                if (M5Cardputer.Keyboard.isKeyPressed(';')) { 
                    if (selectedLang > 0) { selectedLang--; drawLangList(); }
                } else if (M5Cardputer.Keyboard.isKeyPressed('.')) {
                    if (selectedLang < 1) { selectedLang++; drawLangList(); }
                } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
                    if (selectedLang == 0) appLang = "en";
                    else appLang = "zh";
                    saveLLMConfig();
                    
                    history.clear();
                    printTerm(L("Language set to English", "语言已设为: 简体中文"), TFT_GREEN);
                    delay(1000);
                    
                    history.clear();
                    printTerm(L("Aligning with the Celestial Spheres...", "正在对齐天球运行..."), TFT_GREEN);
                    currentState = STATE_WIFI_CHECK;
                }
            }
            break;

        case STATE_WIFI_CHECK:
            loadWifiConfig();
            if (ssid.length() > 0) {
                printTerm(L("Seeking the Unseen Threads of ", "正在搜寻隐秘的联结: ") + ssid, TFT_YELLOW);
                WiFi.mode(WIFI_STA);
                WiFi.disconnect();
                delay(100);
                WiFi.begin(ssid.c_str(), password.c_str());
                wifiConnectTimeoutStart = millis(); // Reset timing
                currentState = STATE_WIFI_CONNECTING;
            } else {
                currentState = STATE_WIFI_SCAN;
            }
            break;
            
        case STATE_WIFI_CONNECTING: {
            if (WiFi.status() == WL_CONNECTED) {
                printTerm(L("Resonance Harmonized!", "共鸣已和谐！"), TFT_GREEN);
                saveWifiConfig();
                
                selectedSpread = 0;
                drawSpreadList();
                currentState = STATE_SPREAD_SELECT;
            } else if (millis() - wifiConnectTimeoutStart > 20000) {
                printTerm(L("The Void remains silent.", "虚空保持着沉默。"), TFT_RED);
                WiFi.disconnect();
                // We keep ssid/password in case user wants to try again
                currentState = STATE_WIFI_SCAN;
            }
            break;
        }

        case STATE_WIFI_SCAN:
            printTerm(L("Scrutinizing Ethereal Echoes...", "正在查探以太的回响..."), TFT_YELLOW);
            numNetworks = WiFi.scanNetworks();
            if (numNetworks == 0) {
                printTerm(L("The path is hidden. Praying once more...", "路径已隐匿，再次祈求..."), TFT_RED);
                delay(1000);
            } else {
                selectedNetwork = 0;
                startNetworkIndex = 0;
                drawWifiList();
                currentState = STATE_WIFI_SELECT;
            }
            break;
            
        case STATE_WIFI_SELECT:
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                if (M5Cardputer.Keyboard.isKeyPressed(';')) { // UP
                    if (selectedNetwork > 0) {
                        selectedNetwork--;
                        if (selectedNetwork < startNetworkIndex) {
                            startNetworkIndex--;
                        }
                        drawWifiList();
                    }
                } else if (M5Cardputer.Keyboard.isKeyPressed('.')) { // DOWN
                    if (selectedNetwork < numNetworks - 1) {
                        selectedNetwork++;
                        if (selectedNetwork >= startNetworkIndex + MAX_NETWORKS_DISPLAY) {
                            startNetworkIndex++;
                        }
                        drawWifiList();
                    }
                } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
                    ssid = WiFi.SSID(selectedNetwork);
                    terminal.fillSprite(TFT_BLACK);
                    terminal.setCursor(0,0);
                    terminal.setTextColor(TFT_GREEN); terminal.println(L("Selected: ", "已选择: ") + ssid);
                    terminal.setTextColor(TFT_YELLOW); terminal.println(L("Enter Password:", "输入网络密码:"));
                    terminal.print(">");
                    terminal.pushSprite(0,0);
                    password = "";
                    currentState = STATE_WIFI_PASSWD;
                }
            }
            break;
            
        case STATE_WIFI_PASSWD:
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
                
                bool changed = false;
                for (auto i : status.word) {
                    password += i;
                    changed = true;
                }
                if (status.del && password.length() > 0) {
                    password.remove(password.length() - 1);
                    changed = true;
                }
                
                if (changed) {
                    terminal.fillRect(10, terminal.getCursorY(), 240, 16, TFT_BLACK); // clear password line
                    terminal.setCursor(10, terminal.getCursorY());
                    terminal.print(password);
                    terminal.pushSprite(0,0);
                }
                
                if (status.enter) {
                    history.clear();
                    printTerm(L("Seeking the Unseen Threads...", "正在搜寻隐秘的联结..."), TFT_YELLOW);
                    WiFi.mode(WIFI_STA);
                    WiFi.disconnect();
                    delay(100);
                    WiFi.begin(ssid.c_str(), password.c_str());
                    wifiConnectTimeoutStart = millis(); // Update timer
                    currentState = STATE_WIFI_CONNECTING;
                }
            }
            break;

        case STATE_SPREAD_SELECT:
            drawSpreadList(); // Animate breathing effect
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                if (M5Cardputer.Keyboard.isKeyPressed(';')) { 
                    if (selectedSpread > 0) { selectedSpread--; }
                } else if (M5Cardputer.Keyboard.isKeyPressed('.')) {
                    if (selectedSpread < NUM_SPREADS - 1) { selectedSpread++; }
                } else if (M5Cardputer.Keyboard.keysState().enter) {
                    currentSpread = (SpreadType)selectedSpread;
                    history.clear();
                    scrollLine = 0;
                    printTerm(L("Stirring the Waters of Destiny...", "正在搅动命运之泉..."), TFT_GREEN);
                    delay(500); 

                    currentState = STATE_LLM_API_TEST;
                }
            }
            break;
            
        case STATE_LLM_API_TEST:
            runLLMTest();
            currentState = STATE_APP_READY;
            break;
            
        case STATE_APP_READY: {
            bool changed = M5Cardputer.Keyboard.isChange();
            bool pressed = M5Cardputer.Keyboard.isPressed();
            auto status = M5Cardputer.Keyboard.keysState();

            bool needRedraw = false;

            if (pressed) {
                // Repeatable scroll
                if (millis() - lastScrollMillis > 50) {
                    if (M5Cardputer.Keyboard.isKeyPressed(';')) { 
                        if (scrollLine > 0) { scrollLine--; lastScrollMillis = millis(); needRedraw = true; }
                    } else if (M5Cardputer.Keyboard.isKeyPressed('.')) { 
                        int maxLine = max(0, (int)history.size() - VISIBLE_LINES);
                        if (scrollLine < maxLine) { scrollLine++; lastScrollMillis = millis(); needRedraw = true; }
                    }
                }

                // One-shot actions
                if (changed) {
                    if (status.enter) {
                        selectedSpread = 0;
                        drawSpreadList();
                        currentState = STATE_SPREAD_SELECT;
                        return; // return early to avoid redraw over menu
                    } else if (M5Cardputer.Keyboard.isKeyPressed(',')) { // Horizontal Left
                        for (auto& el : history) {
                            if (el.type == TerminalElement::CARDS && el.scrollX > 0) {
                                el.scrollX -= 30; needRedraw = true;
                            }
                        }
                    } else if (M5Cardputer.Keyboard.isKeyPressed('/')) { // Horizontal Right
                        for (auto& el : history) {
                            if (el.type == TerminalElement::CARDS) {
                                int maxScroll = (el.cardIds.size() * 80) - 240;
                                if (el.scrollX < maxScroll) {
                                    el.scrollX += 30; needRedraw = true;
                                }
                            }
                        }
                    }
                }
            }

            // IMU Parallax & Tilt Scroll logic
            if (M5.Imu.isEnabled()) {
                M5.Imu.update();
                float ax, ay, az;
                M5.Imu.getAccel(&ax, &ay, &az);
                
                // M5Cardputer is usually used in landscape, axes might be mapped differently. 
                // Lightened LPF for better responsiveness (0.85 -> 0.65)
                imu_roll_f = imu_roll_f * 0.65f + ax * 0.35f;
                imu_pitch_f = imu_pitch_f * 0.65f + ay * 0.35f;
                
                // Calculate target parallax pixels - Increased gain (40 -> 70)
                int new_pX = constrain((int)(-imu_roll_f * 70.0f), -50, 50);
                int new_pY = constrain((int)(-imu_pitch_f * 70.0f), -50, 50);
                
                if (abs(new_pX - global_parallaxX) >= 1 || abs(new_pY - global_parallaxY) >= 1) {
                    global_parallaxX = new_pX;
                    global_parallaxY = new_pY;
                    needRedraw = true;
                }
                
                // Horizontal Tilt Scroll - Lower threshold (0.35 -> 0.2) and increased speed (6 -> 12)
                if (abs(imu_roll_f) > 0.20f) {
                    static uint32_t lastImuScroll = 0;
                    if (millis() - lastImuScroll > 30) {
                        for (auto& el : history) {
                            if (el.type == TerminalElement::CARDS && el.cardIds.size() > 3) {
                                int maxScroll = (el.cardIds.size() * 80) - 240;
                                if (maxScroll > 0) {
                                    // Proportional speed based on tilt angle
                                    int speed = (int)(abs(imu_roll_f) * 30.0f); 
                                    el.scrollX += (imu_roll_f > 0.0f) ? speed : -speed;
                                    if (el.scrollX < 0) el.scrollX = 0;
                                    if (el.scrollX > maxScroll) el.scrollX = maxScroll;
                                    needRedraw = true;
                                }
                            }
                        }
                        lastImuScroll = millis();
                    }
                }
            }

            if (needRedraw) {
                redrawTerminal();
            }
            break;
        }
            
        case STATE_ERROR_SD:
            break;
    }
}
