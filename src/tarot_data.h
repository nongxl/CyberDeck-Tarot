#ifndef TAROT_DATA_H
#define TAROT_DATA_H

#include <Arduino.h>

#define NUM_CARDS 22
#define NUM_SPREADS 6

enum SpreadType {
    SPREAD_SINGLE = 0,
    SPREAD_YES_NO,
    SPREAD_THREE_CARD,
    SPREAD_LOVE,
    SPREAD_CAREER,
    SPREAD_CELTIC_CROSS
};

struct SpreadDefinition {
    SpreadType id;
    const char* name;
    const char* zh_name;
    const char* description;
    int numPositions;
    const char* const* positions;
    const char* readingGuide;
};

struct TarotCard {
    int id;
    const char* name;
    const char* numeral;
    int numKeywords;
    const char* const* keywords;
    const char* uprightMeaning;
    const char* reversedMeaning;
    const char* description;
    const uint8_t* imagePtr;
    uint32_t imageSize;
};

struct DrawnCard {
    int cardId;
    bool reversed;
    int positionIndex;
};

// Accessors for PROGMEM data
void loadCard(int id, TarotCard& cardOut);
void loadSpread(SpreadType type, SpreadDefinition& spreadOut);
String getCardZhName(int id); // Returns Chinese name for card ID

#endif // TAROT_DATA_H
