// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <allegro5/allegro.h>
#include <glm/glm.hpp>

namespace config {

extern int SCREEN_W;
extern int SCREEN_H;
extern double FPS;

extern double FRAME_REST;
extern double MAX_FRAME_TIME;

extern ALLEGRO_COLOR BG_COLOR;

extern const int EMPTY;

extern ALLEGRO_COLOR FIELD_COLOR;
extern double FIELD_W;
extern double FIELD_THICK;
extern double FIELD_MARGIN;

extern int BOARD_W;
extern int BOARD_H;
extern double BOARD_SHIFT_X;
extern double BOARD_SHIFT_Y;

extern double BALL_JUMP_H;
extern double BALL_RADIUS;
extern std::vector<ALLEGRO_COLOR> BALL_COLORS;

extern glm::vec3 BALL_COLOR_FILTER;

extern ALLEGRO_COLOR SCORE_COLOR;
extern int SCORE_FONT_SIZE;
extern int SCORE_SHIFT_X;
extern int SCORE_SHIFT_Y;

extern int STREAK_MIN;

extern double DEAL_PERIOD;
extern int DEAL_COUNT_INIT;
extern int DEAL_COUNT_INGAME;

extern double MOVE_PERIOD;
extern double BUMP_PERIOD;
extern double SCORE_PERIOD;

extern int GAMEOVER_SHIFT_X;
extern int GAMEOVER_SHIFT_Y;
extern int GAMEOVER_FONT_SIZE;
extern double GAMEOVER_PERIOD;

extern double MENU_PADDING;
extern double MENU_MARGIN;
extern int MENU_FONT_SIZE;
extern ALLEGRO_COLOR MENU_SELECT_COLOR;
extern ALLEGRO_COLOR MENU_REGULAR_COLOR;
extern ALLEGRO_COLOR MENU_BG_COLOR_L;
extern ALLEGRO_COLOR MENU_BG_COLOR_D;

extern int HIGHSCORE_CHARACTERS;

void load();

}

#endif
