#include "tarot_data.h"
#include "embedded_assets.h"

// --- SPREADS ---

const char spread_single_name[] PROGMEM = "Single Card";
const char spread_single_zh[] PROGMEM = "单牌阵";
const char spread_single_desc[] PROGMEM = "A focused pull for clarity on one question";
const char spread_single_guide[] PROGMEM = "This is a single card pull. Go deep. Explore every facet of this one card \xE2\x80\x94 its symbolism, its energy, its shadows and light \xE2\x80\x94 as it relates to the question. A single card deserves the same depth as a full spread. Treat it like a meditation, not a summary.";
const char pos_single_0[] PROGMEM = "Significance";
const char* const pos_single[] PROGMEM = { pos_single_0 };

const char spread_yes_no_name[] PROGMEM = "Yes or No";
const char spread_yes_no_zh[] PROGMEM = "是非题";
const char spread_yes_no_desc[] PROGMEM = "A direct answer with nuance";
const char spread_yes_no_guide[] PROGMEM = "The querent seeks a yes-or-no answer. Begin with a clear lean \xE2\x80\x94 yes, no, or \"not yet\" \xE2\x80\x94 based on the card's energy and orientation. Then unpack the why. Upright cards generally lean yes; reversed lean no, but context and the specific card matter more than a rigid rule. Be direct first, then add depth.";
const char pos_yes_no_0[] PROGMEM = "Answer";
const char* const pos_yes_no[] PROGMEM = { pos_yes_no_0 };

const char spread_three_name[] PROGMEM = "Three Card";
const char spread_three_zh[] PROGMEM = "三牌阵";
const char spread_three_desc[] PROGMEM = "Past, present, and future";
const char spread_three_guide[] PROGMEM = "Read as a narrative arc: the Past card sets the stage and shows what brought the querent here. The Present card reveals the current energy and tension. The Future card shows where this trajectory leads. The power is in the movement between them \xE2\x80\x94 show how one flows into the next.";
const char pos_three_0[] PROGMEM = "Past";
const char pos_three_1[] PROGMEM = "Present";
const char pos_three_2[] PROGMEM = "Future";
const char* const pos_three[] PROGMEM = { pos_three_0, pos_three_1, pos_three_2 };

const char spread_love_name[] PROGMEM = "Love";
const char spread_love_zh[] PROGMEM = "情感咨询";
const char spread_love_desc[] PROGMEM = "Relationship dynamics and potential";
const char spread_love_guide[] PROGMEM = "Read the first two cards as mirrors of each person's energy. The Connection card reveals what binds or stands between them. The Challenge is the friction point \xE2\x80\x94 not necessarily negative, but where growth is required. The Potential shows what this relationship can become if the challenge is met honestly. Be compassionate but truthful; never promise outcomes in matters of the heart.";
const char pos_love_0[] PROGMEM = "You";
const char pos_love_1[] PROGMEM = "The Other";
const char pos_love_2[] PROGMEM = "The Connection";
const char pos_love_3[] PROGMEM = "The Challenge";
const char pos_love_4[] PROGMEM = "The Potential";
const char* const pos_love[] PROGMEM = { pos_love_0, pos_love_1, pos_love_2, pos_love_3, pos_love_4 };

const char spread_career_name[] PROGMEM = "Career Path";
const char spread_career_zh[] PROGMEM = "事业发展";
const char spread_career_desc[] PROGMEM = "Professional direction and obstacles";
const char spread_career_guide[] PROGMEM = "Read this as a strategic map. Current Position shows where the querent truly stands (not where they think they stand). The Obstacle is what blocks progress. Hidden Influence reveals an unseen force \xE2\x80\x94 a person, a belief, or a circumstance operating beneath the surface. The Action is what must be done. The Outcome shows the result of taking (or ignoring) that action. Be pragmatic and grounded.";
const char pos_career_0[] PROGMEM = "Current Position";
const char pos_career_1[] PROGMEM = "The Obstacle";
const char pos_career_2[] PROGMEM = "Hidden Influence";
const char pos_career_3[] PROGMEM = "The Action";
const char pos_career_4[] PROGMEM = "The Outcome";
const char* const pos_career[] PROGMEM = { pos_career_0, pos_career_1, pos_career_2, pos_career_3, pos_career_4 };

const char spread_celtic_name[] PROGMEM = "Celtic Cross";
const char spread_celtic_zh[] PROGMEM = "凯尔特十字";
const char spread_celtic_desc[] PROGMEM = "The classic deep-dive spread";
const char spread_celtic_guide[] PROGMEM = "This is the most complete spread. Read it in two phases: First, the cross (cards 1-6) tells the story \xE2\x80\x94 Present and Challenge are the core tension, Foundation is the root cause, Recent Past is what's fading, Crown is the conscious goal, Near Future is the next chapter. Then the staff (cards 7-10) reveals the deeper truth \xE2\x80\x94 Self is the querent's inner state, Environment is external forces, Hopes & Fears (often the same thing) reveals what drives them, and Outcome is the culmination. Weave both phases into one unified narrative.";
const char pos_celtic_0[] PROGMEM = "Present";
const char pos_celtic_1[] PROGMEM = "Challenge";
const char pos_celtic_2[] PROGMEM = "Foundation";
const char pos_celtic_3[] PROGMEM = "Recent Past";
const char pos_celtic_4[] PROGMEM = "Crown";
const char pos_celtic_5[] PROGMEM = "Near Future";
const char pos_celtic_6[] PROGMEM = "Self";
const char pos_celtic_7[] PROGMEM = "Environment";
const char pos_celtic_8[] PROGMEM = "Hopes & Fears";
const char pos_celtic_9[] PROGMEM = "Outcome";
const char* const pos_celtic[] PROGMEM = { pos_celtic_0, pos_celtic_1, pos_celtic_2, pos_celtic_3, pos_celtic_4, pos_celtic_5, pos_celtic_6, pos_celtic_7, pos_celtic_8, pos_celtic_9 };

const SpreadDefinition PGM_SPREADS[] PROGMEM = {
    { SPREAD_SINGLE, spread_single_name, spread_single_zh, spread_single_desc, 1, pos_single, spread_single_guide },
    { SPREAD_YES_NO, spread_yes_no_name, spread_yes_no_zh, spread_yes_no_desc, 1, pos_yes_no, spread_yes_no_guide },
    { SPREAD_THREE_CARD, spread_three_name, spread_three_zh, spread_three_desc, 3, pos_three, spread_three_guide },
    { SPREAD_LOVE, spread_love_name, spread_love_zh, spread_love_desc, 5, pos_love, spread_love_guide },
    { SPREAD_CAREER, spread_career_name, spread_career_zh, spread_career_desc, 5, pos_career, spread_career_guide },
    { SPREAD_CELTIC_CROSS, spread_celtic_name, spread_celtic_zh, spread_celtic_desc, 10, pos_celtic, spread_celtic_guide },
};


// --- CARDS ---
// Macros to define cards cleanly
#define DEF_CARD(ID, ASSET_TAG, NAME, NUM, K1, K2, K3, K4, UP, REV, DESC) \
    const char c##ID##_name[] PROGMEM = NAME; \
    const char c##ID##_num[] PROGMEM = NUM; \
    const char c##ID##_k1[] PROGMEM = K1; \
    const char c##ID##_k2[] PROGMEM = K2; \
    const char c##ID##_k3[] PROGMEM = K3; \
    const char c##ID##_k4[] PROGMEM = K4; \
    const char* const c##ID##_k[] PROGMEM = { c##ID##_k1, c##ID##_k2, c##ID##_k3, c##ID##_k4 }; \
    const char c##ID##_up[] PROGMEM = UP; \
    const char c##ID##_rev[] PROGMEM = REV; \
    const char c##ID##_desc[] PROGMEM = DESC; \
    const uint8_t* c##ID##_img = _binary_assets_cards_##ASSET_TAG##_png_start; \
    uint32_t c##ID##_size = (uint32_t)(_binary_assets_cards_##ASSET_TAG##_png_end - _binary_assets_cards_##ASSET_TAG##_png_start);

DEF_CARD(0, 00_the_fool, "The Fool", "0", "beginnings", "innocence", "spontaneity", "free spirit",
    "The Fool represents new beginnings, having faith in the future, and embracing the unknown with open arms. It signals a time to take a leap of faith and trust the journey ahead. This card encourages you to follow your heart even if the path seems unconventional.",
    "Reversed, The Fool warns of recklessness, poor judgment, and naivety. You may be taking unnecessary risks without considering the consequences. It can also indicate holding back from a new experience out of fear.",
    "A young figure stands at the edge of a cliff, gazing upward at the sky with a small white dog at their feet. They carry a small bundle on a staff over one shoulder, a white rose in the other hand.")

DEF_CARD(1, 01_the_magician, "The Magician", "I", "manifestation", "willpower", "resourcefulness", "skill",
    "The Magician signifies that you have all the tools and resources you need to manifest your desires. It is a card of action, concentration, and personal power. Now is the time to channel your energy and make things happen.",
    "Reversed, The Magician suggests manipulation, trickery, or untapped potential. You may be using your talents for deceptive purposes, or you may not be utilizing your full capabilities. Beware of illusions and misdirection.",
    "A robed figure stands before a table bearing a cup, pentacle, sword, and wand. One hand points to the sky, the other to the earth. An infinity symbol floats above their head, surrounded by a garden of roses and lilies.")

DEF_CARD(2, 02_the_high_priestess, "The High Priestess", "II", "intuition", "mystery", "subconscious", "inner voice",
    "The High Priestess calls you to listen to your inner voice and trust your intuition. She guards the threshold between the conscious and subconscious mind. Secrets may soon be revealed, and wisdom comes from looking within rather than seeking external validation.",
    "Reversed, The High Priestess indicates that you are ignoring your intuition or that hidden information is being withheld. You may feel disconnected from your inner self. It can also suggest secrets, gossip, or superficial knowledge.",
    "A serene woman sits between two pillars, one black and one white, with a crescent moon at her feet. She holds a scroll partially concealed by her robe, and a veil adorned with pomegranates hangs behind her.")

DEF_CARD(3, 03_the_empress, "The Empress", "III", "abundance", "nurturing", "fertility", "nature",
    "The Empress embodies abundance, beauty, and the nurturing force of nature. She signals a period of growth, creativity, and sensual pleasure. This card encourages you to connect with the natural world and embrace the feminine energy of creation and care.",
    "Reversed, The Empress can indicate creative blocks, dependence on others, or neglecting self-care. You may be smothing someone with attention or experiencing a lack of abundance. It warns against being overly possessive or losing touch with nature.",
    "A regal woman reclines on a throne of cushions in a lush garden. She wears a crown of twelve stars and a gown patterned with pomegranates. A field of golden wheat grows at her feet and a heart-shaped shield rests beside her.")

DEF_CARD(4, 04_the_emperor, "The Emperor", "IV", "authority", "structure", "stability", "leadership",
    "The Emperor represents authority, structure, and the power of discipline. He calls on you to take control of your situation and establish order. This card signifies leadership, strategic thinking, and the strength that comes from setting firm boundaries.",
    "Reversed, The Emperor warns of tyranny, rigidity, or an abuse of power. You may be struggling with authority figures or imposing excessive control on others. It can also suggest a lack of discipline or feeling powerless in a situation.",
    "A stern figure sits upon a stone throne carved with ram heads, wearing armor beneath a red robe. He holds an ankh scepter in one hand and an orb in the other. Barren mountains rise behind him under an orange sky.")

DEF_CARD(5, 05_the_hierophant, "The Hierophant", "V", "tradition", "conformity", "wisdom", "spiritual guidance",
    "The Hierophant represents spiritual wisdom, tradition, and established institutions. He encourages you to seek guidance from a mentor or follow a well-trodden path. This card speaks to the value of shared beliefs, rituals, and the wisdom passed down through generations.",
    "Reversed, The Hierophant signals rebellion against convention, unorthodox approaches, or questioning established beliefs. You may feel constrained by tradition or be seeking your own spiritual path. It can also indicate a poor counselor or outdated advice.",
    "A religious figure in ornate vestments sits between two grey pillars, raising one hand in blessing while holding a triple-cross staff. Two acolytes kneel before him, and two crossed keys rest at his feet.")

DEF_CARD(6, 06_the_lovers, "The Lovers", "VI", "love", "harmony", "partnerships", "choices",
    "The Lovers card speaks to deep connections, harmony, and meaningful choices. Beyond romantic love, it represents alignment of values and the union of opposites. A significant decision lies before you, one that requires you to follow your heart while staying true to your principles.",
    "Reversed, The Lovers warn of disharmony, imbalance, or misaligned values in a relationship. You may be facing a difficult choice where neither option feels right. It can indicate self-love issues, broken trust, or a temptation that could lead you astray.",
    "Beneath a radiant angel, a man and woman stand naked in a garden. Behind the woman grows the Tree of Knowledge with a serpent; behind the man, the Tree of Life with twelve flames. A mountain rises between them.")

DEF_CARD(7, 07_the_chariot, "The Chariot", "VII", "determination", "willpower", "triumph", "control",
    "The Chariot signifies victory through determination and willpower. You are being called to harness opposing forces and drive forward with confidence. This card represents overcoming obstacles, maintaining focus, and achieving success through sheer resolve and self-discipline.",
    "Reversed, The Chariot indicates a loss of control, aggression, or lack of direction. You may be feeling pulled in different directions or trying to force an outcome that is not aligned with your path. It warns of defeat through scattered energy or unchecked ambition.",
    "An armored figure rides a stone chariot drawn by one black and one white sphinx. A canopy of stars stretches above, and a city lies behind. The charioteer wears a crown and bears a square on the breastplate.")

DEF_CARD(8, 08_strength, "Strength", "VIII", "courage", "patience", "compassion", "inner strength",
    "Strength represents inner courage, patience, and the quiet power of compassion. Rather than brute force, this card speaks to taming your inner beasts through gentleness and understanding. You have the resilience and emotional fortitude to face whatever challenges arise.",
    "Reversed, Strength suggests self-doubt, insecurity, or raw, unchecked emotions. You may be lacking confidence or letting fear and anxiety overpower your better judgment. It can also indicate an abuse of power or an inability to control destructive impulses.",
    "A woman gently holds open the jaws of a lion, an infinity symbol hovering above her head. She wears a white robe and a garland of flowers, standing in a verdant meadow beneath a golden sky.")

DEF_CARD(9, 09_the_hermit, "The Hermit", "IX", "introspection", "solitude", "wisdom", "inner guidance",
    "The Hermit calls you to withdraw from the noise of the world and seek answers within. This is a time for introspection, contemplation, and soul-searching. The wisdom you seek cannot be found externally; it resides in the quiet stillness of your own inner light.",
    "Reversed, The Hermit warns of isolation, loneliness, or withdrawal taken to an extreme. You may be shutting others out or refusing guidance when you need it most. It can also suggest that your period of reflection is complete and it is time to re-engage with the world.",
    "A cloaked elder stands atop a snow-covered mountain, holding a lantern containing a six-pointed star in one hand and a long staff in the other. The figure gazes downward into the darkness below.")

DEF_CARD(10, 10_wheel_of_fortune, "Wheel of Fortune", "X", "cycles", "destiny", "change", "luck",
    "The Wheel of Fortune signals a turning point, a shift in fate, and the cyclical nature of life. Good luck and positive change are on the horizon. This card reminds you that nothing is permanent and encourages you to embrace the ebb and flow of fortune with grace.",
    "Reversed, the Wheel of Fortune suggests bad luck, resistance to change, or being stuck in a negative cycle. You may feel that external forces are working against you. It warns against clinging to the past and encourages you to break free from repeating patterns.",
    "A great golden wheel floats among clouds, inscribed with alchemical symbols and the letters T-A-RO. Four winged creatures sit in the corners reading books, while a sphinx perches atop the wheel and a serpent descends its side.")

DEF_CARD(11, 11_justice, "Justice", "XI", "fairness", "truth", "accountability", "law",
    "Justice calls for truth, fairness, and accountability. The consequences of past actions are coming to bear, and balance must be restored. This card urges you to make decisions with clarity and integrity, knowing that the universe seeks equilibrium in all things.",
    "Reversed, Justice indicates dishonesty, unfairness, or a refusal to accept accountability. Legal matters may not go in your favor, or you may be avoiding the consequences of your actions. It warns of bias, corruption, or an unjust situation.",
    "A crowned figure sits on a stone throne between two pillars, holding a double-edged sword upright in one hand and a set of balanced scales in the other. A purple veil hangs behind the throne.")

DEF_CARD(12, 12_the_hanged_man, "The Hanged Man", "XII", "surrender", "new perspective", "sacrifice", "letting go",
    "The Hanged Man invites you to pause, surrender, and see the world from a new perspective. This is not a card of punishment but of willing sacrifice and spiritual insight. By letting go of control and embracing stillness, profound understanding and transformation become possible.",
    "Reversed, The Hanged Man suggests resistance to necessary sacrifice, stalling, or martyrdom. You may be refusing to let go of something that no longer serves you, or delaying an inevitable decision. It can also indicate unnecessary suffering or a victim mentality.",
    "A figure hangs upside down from a living tree by one foot, the other leg crossed behind the knee. A golden halo surrounds their serene face, and their hands are hidden behind their back.")

DEF_CARD(13, 13_death, "Death", "XIII", "transformation", "endings", "renewal", "transition",
    "Death signals a profound transformation, the end of one chapter and the beginning of another. This card rarely means literal death; instead, it speaks to necessary endings that make way for new growth. Embrace the change, for resisting it will only prolong the transition.",
    "Reversed, Death indicates resistance to change, stagnation, or fear of letting go. You may be clinging to old patterns, relationships, or identities that have run their course. Personal transformation is being delayed by your refusal to release what is no longer serving you.",
    "A skeletal figure in black armor rides a white horse through a field where people of all stations have fallen. A bishop approaches with hands clasped in prayer. In the distance, the sun rises between two towers.")

DEF_CARD(14, 14_temperance, "Temperance", "XIV", "balance", "moderation", "patience", "harmony",
    "Temperance calls for balance, patience, and moderation in all things. It speaks to the art of blending opposites into a harmonious whole. This card encourages you to take the middle path, practice self-restraint, and trust that the right outcome will unfold in its own time.",
    "Reversed, Temperance warns of excess, imbalance, or a lack of harmony. You may be overindulging, rushing into things, or struggling to find your center. It can indicate conflict between different areas of your life or a need to realign your priorities.",
    "A winged angel stands with one foot on land and one in water, pouring liquid between two golden cups. A path winds from the water toward distant mountains where a golden crown floats above twin peaks. Irises bloom at the water's edge.")

DEF_CARD(15, 15_the_devil, "The Devil", "XV", "bondage", "materialism", "shadow self", "attachment",
    "The Devil represents the shadow side: bondage, addiction, and unhealthy attachments. It reveals the chains that bind you, often of your own making. This card challenges you to confront your deepest fears and dependencies, recognizing that the power to free yourself has always been within you.",
    "Reversed, The Devil signals a release from bondage, breaking free of addictions, or reclaiming your power. You are beginning to see through illusions and recognize the self-imposed limitations that have held you back. Liberation and personal empowerment are at hand.",
    "A horned, bat-winged figure crouches on a dark pedestal to which a naked man and woman are loosely chained. Both figures have small horns and tails. An inverted pentagram blazes above the devil's head.")

DEF_CARD(16, 16_the_tower, "The Tower", "XVI", "upheaval", "revelation", "sudden change", "liberation",
    "The Tower heralds sudden upheaval, destruction of false structures, and revelations that shake your foundations. While this can feel devastating, it clears away what was built on unstable ground. From the rubble, you can rebuild something stronger, truer, and more aligned with reality.",
    "Reversed, The Tower suggests you are resisting necessary change or that a disaster has been narrowly avoided. You may be aware that something in your life is unsustainable but are clinging to it anyway. It can also indicate a personal transformation happening internally rather than externally.",
    "Lightning strikes a tall stone tower on a rocky peak, shattering its crown. Two figures plummet through the air amid falling flames and debris. Dark clouds swirl around the burning structure against a black sky.")

DEF_CARD(17, 17_the_star, "The Star", "XVII", "hope", "inspiration", "renewal", "serenity",
    "The Star shines as a beacon of hope, inspiration, and spiritual renewal after a period of darkness. It signals a time of healing, peace, and reconnection with your higher self. Trust that the universe is guiding you toward your purpose, and let your inner light radiate outward.",
    "Reversed, The Star indicates a loss of faith, discouragement, or disconnection from your spiritual path. You may be feeling uninspired, hopeless, or unable to see the light at the end of the tunnel. It urges you to reconnect with what gives your life meaning and purpose.",
    "A nude figure kneels by a pool of water beneath a sky of eight radiant stars, pouring water from two pitchers onto the land and into the pool. A bird perches in a tree behind her. The landscape is lush and open.")

DEF_CARD(18, 18_the_moon, "The Moon", "XVIII", "illusion", "fear", "subconscious", "intuition",
    "The Moon reveals the realm of illusion, fear, and the deep subconscious. Things are not as they seem, and confusion or deception may cloud your judgment. This card calls you to trust your intuition, face your fears, and navigate through uncertainty with inner knowing rather than logic alone.",
    "Reversed, The Moon suggests that illusions are being dispelled and clarity is returning. Fears and anxieties that once paralyzed you are losing their grip. It can also indicate repressed emotions surfacing or the release of subconscious patterns that have held you captive.",
    "A full moon gazes down between two towers as a winding path leads from a pool of water into distant mountains. A crayfish emerges from the pool while a dog and a wolf howl at the moon from either side of the path.")

DEF_CARD(19, 19_the_sun, "The Sun", "XIX", "joy", "success", "vitality", "optimism",
    "The Sun radiates pure joy, success, and vitality. It is one of the most positive cards in the deck, signaling a time of happiness, clarity, and achievement. Everything is coming together, and you are free to express your true self with confidence and enthusiasm.",
    "Reversed, The Sun may indicate temporary setbacks, diminished joy, or an overly optimistic outlook that ignores reality. You may be struggling to see the bright side or experiencing delays in achieving your goals. The happiness you seek is still available but may require more effort to reach.",
    "A radiant sun beams down on a joyful child riding a white horse through a garden of sunflowers. The child holds a red banner aloft, and the horse wears a garland of flowers. A stone wall stands behind them.")

DEF_CARD(20, 20_judgement, "Judgement", "XX", "rebirth", "reckoning", "absolution", "inner calling",
    "Judgement calls you to rise up, heed your inner calling, and embrace a spiritual rebirth. It is a time of reckoning, self-evaluation, and answering a higher purpose. Past experiences have prepared you for this moment of transformation; answer the call with courage and clarity.",
    "Reversed, Judgement suggests self-doubt, an ignored calling, or an inability to learn from past mistakes. You may be avoiding a necessary reckoning or forgiving yourself or others. It warns against harsh self-judgment and encourages compassion in your self-evaluation.",
    "An angel blows a great trumpet from the clouds as figures rise from open coffins with arms outstretched. Men, women, and children emerge from grey tombs, their faces turned upward toward the heavens. Snow-capped mountains frame the background.")

DEF_CARD(21, 21_the_world, "The World", "XXI", "completion", "integration", "accomplishment", "wholeness",
    "The World represents completion, accomplishment, and the fulfillment of a major life cycle. You have reached a point of wholeness and integration, having learned the lessons this journey had to offer. Celebrate your achievements and prepare for the next cycle of growth to begin.",
    "Reversed, The World indicates incomplete goals, shortcuts, or a lack of closure. You may be so close to finishing something but feel stuck at the final hurdle. It can also suggest that you are seeking fulfillment externally when true wholeness must come from within.",
    "A dancing figure is encircled by a great laurel wreath, holding a wand in each hand. The four creatures of the evangelists occupy the corners: an angel, an eagle, a bull, and a lion. Purple ribbons bind the wreath at top and bottom.")


#define ENTRY(ID) { ID, c##ID##_name, c##ID##_num, 4, c##ID##_k, c##ID##_up, c##ID##_rev, c##ID##_desc, c##ID##_img, c##ID##_size }

const TarotCard PGM_CARDS[] PROGMEM = {
    ENTRY(0), ENTRY(1), ENTRY(2), ENTRY(3), ENTRY(4),
    ENTRY(5), ENTRY(6), ENTRY(7), ENTRY(8), ENTRY(9),
    ENTRY(10), ENTRY(11), ENTRY(12), ENTRY(13), ENTRY(14),
    ENTRY(15), ENTRY(16), ENTRY(17), ENTRY(18), ENTRY(19),
    ENTRY(20), ENTRY(21)
};

void loadCard(int id, TarotCard& cardOut) {
    memcpy_P(&cardOut, &PGM_CARDS[id], sizeof(TarotCard));
}

void loadSpread(SpreadType type, SpreadDefinition& spreadOut) {
    memcpy_P(&spreadOut, &PGM_SPREADS[type], sizeof(SpreadDefinition));
}

// --- Chinese card names (22 Major Arcana) ---
const char zh_c0[]  PROGMEM = "\xe6\x84\x9a\xe8\x80\x85";           // 愚者
const char zh_c1[]  PROGMEM = "\xe9\xad\x94\xe6\xb3\x95\xe5\xb8\x88"; // 魔法师
const char zh_c2[]  PROGMEM = "\xe5\xa5\xb3\xe7\xa5\xad\xe5\x8f\xb8"; // 女祭司
const char zh_c3[]  PROGMEM = "\xe5\xa5\xb3\xe7\x9a\x87";           // 女皇
const char zh_c4[]  PROGMEM = "\xe7\x9a\x87\xe5\xb8\x9d";           // 皇帝
const char zh_c5[]  PROGMEM = "\xe6\x95\x99\xe7\x9a\x87";           // 教皇
const char zh_c6[]  PROGMEM = "\xe6\x81\x8b\xe4\xba\xba";           // 恋人
const char zh_c7[]  PROGMEM = "\xe6\x88\x98\xe8\xbd\xa6";           // 战车
const char zh_c8[]  PROGMEM = "\xe5\x8a\x9b\xe9\x87\x8f";           // 力量
const char zh_c9[]  PROGMEM = "\xe9\x9a\x90\xe8\x80\x85";           // 隐者
const char zh_c10[] PROGMEM = "\xe5\x91\xbd\xe8\xbf\x90\xe4\xb9\x8b\xe8\xbd\xae"; // 命运之轮
const char zh_c11[] PROGMEM = "\xe6\xad\xa3\xe4\xb9\x89";           // 正义
const char zh_c12[] PROGMEM = "\xe5\x80\x92\xe5\x90\x8a\xe4\xba\xba"; // 倒吊人
const char zh_c13[] PROGMEM = "\xe6\xad\xbb\xe7\xa5\x9e";           // 死神
const char zh_c14[] PROGMEM = "\xe8\x8a\x82\xe5\x88\xb6";           // 节制
const char zh_c15[] PROGMEM = "\xe6\x81\xb6\xe9\xad\x94";           // 恶魔
const char zh_c16[] PROGMEM = "\xe5\xa1\x94";                        // 塔
const char zh_c17[] PROGMEM = "\xe6\x98\x9f\xe6\x98\x9f";           // 星星
const char zh_c18[] PROGMEM = "\xe6\x9c\x88\xe4\xba\xae";           // 月亮
const char zh_c19[] PROGMEM = "\xe5\xa4\xaa\xe9\x98\xb3";           // 太阳
const char zh_c20[] PROGMEM = "\xe5\xae\xa1\xe5\x88\xa4";           // 审判
const char zh_c21[] PROGMEM = "\xe4\xb8\x96\xe7\x95\x8c";           // 世界

const char* const ZH_CARD_NAMES[NUM_CARDS] PROGMEM = {
    zh_c0,  zh_c1,  zh_c2,  zh_c3,  zh_c4,  zh_c5,
    zh_c6,  zh_c7,  zh_c8,  zh_c9,  zh_c10, zh_c11,
    zh_c12, zh_c13, zh_c14, zh_c15, zh_c16, zh_c17,
    zh_c18, zh_c19, zh_c20, zh_c21
};

String getCardZhName(int id) {
    if (id < 0 || id >= NUM_CARDS) return "";
    const char* pgmPtr = (const char*)pgm_read_ptr(&ZH_CARD_NAMES[id]);
    char buf[32];
    strncpy_P(buf, pgmPtr, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
    return String(buf);
}
