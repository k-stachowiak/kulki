// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef KULKI_CONSTANTS_H
#define KULKI_CONSTANTS_H

#include <vector>
#include <glm/glm.hpp>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "dick.h"

class KulkiConfig;

namespace dick {
    struct Resources;
}

struct KulkiConstants {

    int empty_field;
    int board_w;
    int board_h;

    ALLEGRO_FONT *gameover_font;
    ALLEGRO_FONT *score_font;
    ALLEGRO_FONT *menu_font;
    ALLEGRO_BITMAP *ball_bmp;
    ALLEGRO_BITMAP *tile_bmp;
    ALLEGRO_BITMAP *rarrow_bmp;
    ALLEGRO_BITMAP *larrow_bmp;

    std::vector<dick::Color> ball_colors;

    int screen_w;
    int screen_h;
    double field_w;
    double field_margin;
    double field_thick;
    dick::Color field_color;
    dick::Color bg_color;
    double board_shift_x;
    double board_shift_y;
    double ball_radius;
    double ball_jump_h;

    double deal_period;
    int deal_count_init;
    int deal_count_ingame;

    int gameover_shift_x;
    int gameover_shift_y;

    int highscore_characters;
    int highscore_max_entries;

    double menu_padding;
    double menu_margin;
    dick::Color menu_bg_color_l;
    dick::Color menu_bg_color_d;
    dick::Color menu_regular_color;
    dick::Color menu_select_color;

    double move_period;

    double score_period;
    int score_serie_min;
    int score_shift_x;
    int score_shift_y;
    dick::Color score_color;

    double bump_period;

    KulkiConstants(dick::Resources &resources, KulkiConfig &config);
};

#endif
