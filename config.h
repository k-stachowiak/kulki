#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <allegro5/allegro.h>
#include <glm/glm.hpp>

namespace {
    const int SCREEN_W = 800;
    const int SCREEN_H = 800;
    const double FPS = 50.0;

    const double FRAME_REST = 0.001;
    const double MAX_FRAME_TIME = 0.25;

    const ALLEGRO_COLOR BG_COLOR = al_map_rgb_f(0.5, 0.5, 0.5);

    const int EMPTY = -1;

    const ALLEGRO_COLOR FIELD_COLOR = al_map_rgb_f(0.75, 0.75, 0.75);
    const double FIELD_W = 64;
    const double FIELD_THICK = 1;
    const double FIELD_MARGIN = 0.025;

    const int BOARD_W = 9;
    const int BOARD_H = 9;
    const double BOARD_SHIFT_X = (SCREEN_W - (BOARD_W * FIELD_W)) / 2.0;
    const double BOARD_SHIFT_Y = (SCREEN_H - (BOARD_H * FIELD_W)) / 2.0;

    const double BALL_JUMP_H = 0.333;
    const double BALL_RADIUS = 0.8 * (FIELD_W / 2.0);
    const std::vector<ALLEGRO_COLOR> BALL_COLORS {
        al_map_rgb_f(0, 0, 0),
        al_map_rgb_f(0, 0, 1),
        al_map_rgb_f(0, 1, 0),
        al_map_rgb_f(0, 1, 1),
        al_map_rgb_f(1, 0, 0),
        al_map_rgb_f(1, 0, 1),
        al_map_rgb_f(1, 1, 0),
        al_map_rgb_f(1, 1, 1)
    };

    const glm::vec3 BALL_COLOR_FILTER { 0.9, 0.9, 0.9 };

    const ALLEGRO_COLOR SCORE_COLOR = al_map_rgb_f(1, 1, 1);
    const int SCORE_FONT_SIZE = 24;
    const int SCORE_SHIFT_X = 10;
    const int SCORE_SHIFT_Y = 10;

    const int STREAK_MIN = 5;

    const double DEAL_PERIOD = 0.25;
    const int DEAL_COUNT_INIT = 7, DEAL_COUNT_INGAME = 3;

    const double MOVE_PERIOD = 0.25;

    const double SCORE_PERIOD = 1.0;

    const int GAMEOVER_SHIFT_X = SCREEN_W / 2, GAMEOVER_SHIFT_Y = SCREEN_H / 2;
    const int GAMEOVER_FONT_SIZE = 64;
    const double GAMEOVER_PERIOD = 0.125;

    const double MENU_PADDING = 20.0;
    const double MENU_MARGIN = 10.0;
    const int MENU_FONT_SIZE = 30;
    const ALLEGRO_COLOR MENU_SELECT_COLOR = al_map_rgb_f(1, 1, 0);
    const ALLEGRO_COLOR MENU_REGULAR_COLOR = al_map_rgb_f(0, 1, 0);
    const ALLEGRO_COLOR MENU_BG_COLOR_L = al_map_rgb_f(0.333, 0.333, 0.333);
    const ALLEGRO_COLOR MENU_BG_COLOR_D = al_map_rgb_f(0.25, 0.25, 0.25);
}

#endif
