#include "config.h"
#include "resources.h"
#include "kulki_constants.h"

KulkiConstants::KulkiConstants(Resources &resources, KulkiConfig &config) :

    empty_field { config.get_integer("EMPTY") },
    board_w { config.get_integer("BOARD_W") },
    board_h { config.get_integer("BOARD_H") },

    gameover_font { resources.get_font("data/prstartk.ttf", -config.get_integer("GAMEOVER_FONT_SIZE")) },
    score_font { resources.get_font("data/prstartk.ttf", -config.get_integer("SCORE_FONT_SIZE")) },
    menu_font { resources.get_font("data/prstartk.ttf", -config.get_integer("MENU_FONT_SIZE")) },
    ball_bmp { resources.get_bitmap("data/ball2.png") },
    tile_bmp { resources.get_bitmap("data/tile.png") },

    ball_colors { config.get_color_range("BALL_COLORS") },
    ball_count { config.get_integer("COLOR_COUNT") },

    screen_w { config.get_integer("SCREEN_W") },
    screen_h { config.get_integer("SCREEN_H") },
    field_w { config.get_real("FIELD_W") },
    field_margin { config.get_real("FIELD_MARGIN") },
    field_thick { config.get_real("FIELD_THICK") },
    field_color(config.get_color("FIELD_COLOR")),
    bg_color(config.get_color("BG_COLOR")),
    board_shift_x { config.get_real("BOARD_SHIFT_X") },
    board_shift_y { config.get_real("BOARD_SHIFT_Y") },
    ball_color_filter { config.get_vec("BALL_COLOR_FILTER") },
    ball_radius { config.get_real("BALL_RADIUS") },
    ball_jump_h { config.get_real("BALL_JUMP_H") },

    deal_period { config.get_real("DEAL_PERIOD") },
    deal_count_init { config.get_integer("DEAL_COUNT_INIT") },
    deal_count_ingame { config.get_integer("DEAL_COUNT_INGAME") },

    gameover_period { config.get_real("GAMEOVER_PERIOD") },
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
{
    assert(ball_count <= static_cast<int>(ball_colors.size()));
}
