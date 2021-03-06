#pragma once
#include "game_state.h"

typedef enum {
    text_defeated_monster,
    text_use_item_prompt,
    text_item_vanishes,
    text_read_scroll,
    text_found_gold,
    text_hungry,
    text_aggravate_monsters,
    text_restore_strength,
    text_remove_armor,
    text_sick,
    text_gain_strength,
    text_was_called,
    text_bite,
    text_remove_curse,
    text_dart_hit,
    text_dart_miss,
    text_found_trap,
    text_wear_armor,
    text_call_it_prompt,
    text_enchant_armor,
    text_enchant_weapon,
    text_monster_detect_fail,
    text_magic_detect_fail,
    text_missile_vanishes,
    text_sleep_trap,
    text_more,
    text_now_wielding,
    text_frozen,
    text_detect_food_fail,
    text_create_monster_fail,
    text_inventory_prompt,
    text_faint,
    text_gain_item,
    text_destroy_scare_monster,
    text_wear_ring,
    text_already_wearing_armor,
    text_welcome,
    text_used_item_call_it_prompt,
    text_go_up_stairs,
    text_identify_item,
    text_maniacal_laughter,
    text_vorpalize_weapon,
} text_id;

const char* get_text(text_id id);