#include "tarot_engine.h"

std::vector<DrawnCard> shuffleAndDraw(SpreadType spreadType) {
    SpreadDefinition spread;
    loadSpread(spreadType, spread);

    std::vector<int> deck;
    for (int i = 0; i < NUM_CARDS; i++) {
        deck.push_back(i);
    }

    // Fisher-Yates shuffle using true RNG from ESP32
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = esp_random() % (i + 1);
        std::swap(deck[i], deck[j]);
    }

    std::vector<DrawnCard> drawn;
    int cardsToDraw = spread.numPositions;
    for (int i = 0; i < cardsToDraw; i++) {
        DrawnCard card;
        card.cardId = deck[i];
        // 30% chance of reversed
        card.reversed = (esp_random() % 100) < 30;
        card.positionIndex = i;
        drawn.push_back(card);
    }
    return drawn;
}

String buildSystemPrompt(SpreadType spreadType, String appLang) {
    SpreadDefinition spread;
    loadSpread(spreadType, spread);

    String prompt = "";
    prompt += "You are Aurelia, a tarot reader who learned the cards from her grandmother in a small shop that smelled of cedarwood and old paper. You've been reading for 30 years. You don't perform — you listen, you see, and you speak plainly about what the cards show.\n\n";
    prompt += "Your voice:\n";
    prompt += "- Warm but direct. You're the kind of reader people trust because you don't sugarcoat.\n";
    prompt += "- You speak in second person (\"you\") to the querent, as if they're sitting across from you.\n";
    prompt += "- Your language is vivid and sensory — you describe what you *see* in the cards, not just what they *mean*.\n";
    prompt += "- You never say \"this card means...\" — instead, you weave the meaning into observation. (\"I see you standing at a threshold, one foot already across...\")\n";
    prompt += "- You're not afraid of difficult truths, but you always leave the querent with agency. The cards show the current path — not a fixed fate.\n\n";
    
    prompt += "Structure:\n";
    prompt += "- Open with a brief moment of connection — acknowledge the question's weight or the energy you sense.\n";
    prompt += "- Read the cards in their positions, but DON'T treat each card as a separate paragraph. The cards are a conversation with each other. Let them interrupt, echo, and challenge one another.\n";
    prompt += "- When a card is reversed, don't just say \"reversed means X.\" Describe the reversal as energy — blocked, inverted, struggling to express itself, or sometimes a card that's *about* to turn upright.\n";
    prompt += "- Close with a synthesis — not a summary. Pull one thread that ties everything together and leave the querent with something to sit with.\n\n";
    
    // Copy the reading guide from PROGMEM
    char buf[512];
    strncpy_P(buf, spread.readingGuide, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    prompt += String(buf) + "\n\n";

    prompt += "Rules:\n";
    prompt += "- NEVER list cards mechanically.\n";
    prompt += "- NEVER use bullet points, headers, numbered lists, or markdown formatting.\n";
    prompt += "- NEVER say \"remember\" or \"keep in mind\" — show, don't lecture.\n";
    prompt += "- Keep it under 600 words for single/yes-no spreads, under 900 for three-card/love/career, under 1200 for celtic cross.\n";
    prompt += "- Write in flowing paragraphs as natural speech.\n";
    
    if (appLang == "zh") {
        prompt += "\nIMPORTANT: You must speak to the user in Simplified Chinese (简体中文), maintaining the exact Aurelia persona, warmth, and depth, but natively translated into Chinese.\n";
    } else {
        prompt += "\nIMPORTANT: You must reply in English.\n";
    }

    return prompt;
}

String buildUserMessage(String question, const std::vector<DrawnCard>& cards, SpreadType spreadType) {
    SpreadDefinition spread;
    loadSpread(spreadType, spread);

    char nameBuf[64];
    strncpy_P(nameBuf, spread.name, sizeof(nameBuf));
    nameBuf[sizeof(nameBuf) - 1] = '\0';

    String msg = "";
    msg += "\"" + question + "\"\n\n";
    msg += String(nameBuf) + " spread:\n\n";

    for (size_t i = 0; i < cards.size(); i++) {
        TarotCard tCard;
        loadCard(cards[i].cardId, tCard);

        char cName[64], cNum[16], cUp[512], cRev[512];
        strncpy_P(cName, tCard.name, sizeof(cName)); cName[sizeof(cName)-1] = '\0';
        strncpy_P(cNum, tCard.numeral, sizeof(cNum)); cNum[sizeof(cNum)-1] = '\0';
        strncpy_P(cUp, tCard.uprightMeaning, sizeof(cUp)); cUp[sizeof(cUp)-1] = '\0';
        strncpy_P(cRev, tCard.reversedMeaning, sizeof(cRev)); cRev[sizeof(cRev)-1] = '\0';
        
        char posName[64];
        strncpy_P(posName, (const char*)pgm_read_ptr(&(spread.positions[cards[i].positionIndex])), sizeof(posName));
        posName[sizeof(posName)-1] = '\0';

        String orientation = cards[i].reversed ? "REVERSED" : "UPRIGHT";
        String meaning = cards[i].reversed ? String(cRev) : String(cUp);

        msg += "[" + String(posName) + "] " + String(cName) + " (" + String(cNum) + ") — " + orientation + "\n";
        
        // Keywords
        for (int k = 0; k < tCard.numKeywords; k++) {
            char kw[32];
            strncpy_P(kw, (const char*)pgm_read_ptr(&(tCard.keywords[k])), sizeof(kw));
            kw[sizeof(kw)-1] = '\0';
            msg += String(kw);
            if (k < tCard.numKeywords - 1) msg += ", ";
        }
        msg += "\n" + meaning + "\n\n";
    }

    return msg;
}
