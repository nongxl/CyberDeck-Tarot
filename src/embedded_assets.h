#ifndef EMBEDDED_ASSETS_H
#define EMBEDDED_ASSETS_H

#include <Arduino.h>

#define DECLARE_CARD(id, name) \
    extern const uint8_t _binary_assets_cards_##id##_##name##_png_start[] asm("_binary_assets_cards_" #id "_" #name "_png_start"); \
    extern const uint8_t _binary_assets_cards_##id##_##name##_png_end[] asm("_binary_assets_cards_" #id "_" #name "_png_end");

DECLARE_CARD(00, the_fool)
DECLARE_CARD(01, the_magician)
DECLARE_CARD(02, the_high_priestess)
DECLARE_CARD(03, the_empress)
DECLARE_CARD(04, the_emperor)
DECLARE_CARD(05, the_hierophant)
DECLARE_CARD(06, the_lovers)
DECLARE_CARD(07, the_chariot)
DECLARE_CARD(08, strength)
DECLARE_CARD(09, the_hermit)
DECLARE_CARD(10, wheel_of_fortune)
DECLARE_CARD(11, justice)
DECLARE_CARD(12, the_hanged_man)
DECLARE_CARD(13, death)
DECLARE_CARD(14, temperance)
DECLARE_CARD(15, the_devil)
DECLARE_CARD(16, the_tower)
DECLARE_CARD(17, the_star)
DECLARE_CARD(18, the_moon)
DECLARE_CARD(19, the_sun)
DECLARE_CARD(20, judgement)
DECLARE_CARD(21, the_world)

extern const uint8_t _binary_assets_cards_back_png_start[] asm("_binary_assets_cards_back_png_start");
extern const uint8_t _binary_assets_cards_back_png_end[] asm("_binary_assets_cards_back_png_end");

#endif
