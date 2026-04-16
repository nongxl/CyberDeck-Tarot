#ifndef TAROT_ENGINE_H
#define TAROT_ENGINE_H

#include "tarot_data.h"
#include <vector>
#include <Arduino.h>

std::vector<DrawnCard> shuffleAndDraw(SpreadType spreadType);

String buildSystemPrompt(SpreadType spreadType, String appLang);

String buildUserMessage(String question, const std::vector<DrawnCard>& cards, SpreadType spreadType);

#endif
