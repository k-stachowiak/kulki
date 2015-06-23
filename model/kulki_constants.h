#ifndef KULKI_CONSTANTS_H
#define KULKI_CONSTANTS_H

#include <vector>
#include <glm/glm.hpp>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class KulkiConfig;
class Resources;

struct KulkiConstants {

    int empty_field;
    int board_w;
    int board_h;

    ALLEGRO_FONT *gameover_font;
    ALLEGRO_FONT *score_font;
    ALLEGRO_FONT *menu_font;
    ALLEGRO_BITMAP *ball_bmp;
    ALLEGRO_BITMAP *tile_bmp;

    std::vector<ALLEGRO_COLOR> ball_colors;
    int ball_count;

    int screen_w;
    int screen_h;
    double field_w;
    double field_margin;
    double field_thick;
    ALLEGRO_COLOR field_color;
    ALLEGRO_COLOR bg_color;
    double board_shift_x;
    double board_shift_y;
    glm::vec3 ball_color_filter;
    double ball_radius;
    double ball_jump_h;

    double deal_period;
    int deal_count_init;
    int deal_count_ingame;

    double gameover_period;
    int gameover_shift_x;
    int gameover_shift_y;

    int highscore_characters;
    int highscore_max_entries;

    double menu_padding;
    double menu_margin;
    ALLEGRO_COLOR menu_bg_color_l;
    ALLEGRO_COLOR menu_bg_color_d;
    ALLEGRO_COLOR menu_regular_color;
    ALLEGRO_COLOR menu_select_color;

    double move_period;

    double score_period;
    int score_serie_min;
    int score_shift_x;
    int score_shift_y;
    ALLEGRO_COLOR score_color;

    double bump_period;

    KulkiConstants(Resources &resources, KulkiConfig &config);
};

#endif
