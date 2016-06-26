// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "dick.h"
#include "kulki_constants.h"

KulkiConstants::KulkiConstants(dick::Resources &resources, KulkiConfig &config) :

    empty_field { config.get_integer("EMPTY") },
    board_w { config.get_integer("BOARD_W") },
    board_h { config.get_integer("BOARD_H") },

    gameover_font { (ALLEGRO_FONT*)resources.get_font("data/neuropol_x_rg.ttf", -config.get_integer("GAMEOVER_FONT_SIZE")) },
    score_font { (ALLEGRO_FONT*)resources.get_font("data/neuropol_x_rg.ttf", -config.get_integer("SCORE_FONT_SIZE")) },
    menu_font { (ALLEGRO_FONT*)resources.get_font("data/neuropol_x_rg.ttf", -config.get_integer("MENU_FONT_SIZE")) },
    ball_bmp { (ALLEGRO_BITMAP*)resources.get_image("data/ball2.png") },
    tile_bmp { (ALLEGRO_BITMAP*)resources.get_image("data/tile.png") },
    feet_bmp { (ALLEGRO_BITMAP*)resources.get_image("data/feet.png") },
    rarrow_bmp { (ALLEGRO_BITMAP*)resources.get_image("data/rarrow.png") },
    larrow_bmp { (ALLEGRO_BITMAP*)resources.get_image("data/larrow.png") },

    ball_colors { config.get_color_range("BALL_COLORS") },

    screen_w { config.get_integer("SCREEN_W") },
    screen_h { config.get_integer("SCREEN_H") },
    field_w { config.get_real("FIELD_W") },
    field_margin { config.get_real("FIELD_MARGIN") },
    field_thick { config.get_real("FIELD_THICK") },
    field_color(config.get_color("FIELD_COLOR")),
    bg_color(config.get_color("BG_COLOR")),
    board_shift_x { config.get_real("BOARD_SHIFT_X") },
    board_shift_y { config.get_real("BOARD_SHIFT_Y") },
    ball_radius { config.get_real("BALL_RADIUS") },
    ball_jump_h { config.get_real("BALL_JUMP_H") },

    deal_period { config.get_real("DEAL_PERIOD") },
    deal_count_init { config.get_integer("DEAL_COUNT_INIT") },
    deal_count_ingame { config.get_integer("DEAL_COUNT_INGAME") },

    gameover_shift_x { config.get_integer("GAMEOVER_SHIFT_X") },
    gameover_shift_y { config.get_integer("GAMEOVER_SHIFT_Y") },

    highscore_characters { config.get_integer("HIGHSCORE_CHARACTERS") },
    highscore_max_entries { config.get_integer("HIGHSCORE_ENTRIES") },

    menu_padding { config.get_real("MENU_PADDING") },
    menu_margin { config.get_real("MENU_MARGIN") },
    menu_bg_color_l(config.get_color("MENU_BG_COLOR_L")),
    menu_bg_color_d(config.get_color("MENU_BG_COLOR_D")),
    menu_regular_color(config.get_color("MENU_REGULAR_COLOR")),
    menu_select_color(config.get_color("MENU_SELECT_COLOR")),

    move_period { config.get_real("MOVE_PERIOD") },

    score_period { config.get_real("SCORE_PERIOD") },
    score_serie_min { config.get_integer("SCORE_SERIE_MIN") },
    score_shift_x { config.get_integer("SCORE_SHIFT_X") },
    score_shift_y { config.get_integer("SCORE_SHIFT_Y") },
    score_color(config.get_color("SCORE_COLOR")),

    bump_period { config.get_real("BUMP_PERIOD") }
{}
