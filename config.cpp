#include <iostream>

#include "config.h"
#include "moon.h"

namespace config {

int SCREEN_W;
int SCREEN_H;
double FPS;

double FRAME_REST;
double MAX_FRAME_TIME;

ALLEGRO_COLOR BG_COLOR;

const int EMPTY = -1;

ALLEGRO_COLOR FIELD_COLOR;
double FIELD_W;
double FIELD_THICK;
double FIELD_MARGIN;

int BOARD_W;
int BOARD_H;
double BOARD_SHIFT_X;
double BOARD_SHIFT_Y;

double BALL_JUMP_H;
double BALL_RADIUS;
std::vector<ALLEGRO_COLOR> BALL_COLORS;
glm::vec3 BALL_COLOR_FILTER;

ALLEGRO_COLOR SCORE_COLOR;
int SCORE_FONT_SIZE;
int SCORE_SHIFT_X;
int SCORE_SHIFT_Y;

int SERIE_MIN;

double DEAL_PERIOD;
int DEAL_COUNT_INIT;
int DEAL_COUNT_INGAME;

double MOVE_PERIOD;
double BUMP_PERIOD;
double SCORE_PERIOD;

int GAMEOVER_SHIFT_X;
int GAMEOVER_SHIFT_Y;
int GAMEOVER_FONT_SIZE;
double GAMEOVER_PERIOD;

double MENU_PADDING;
double MENU_MARGIN;
int MENU_FONT_SIZE;
ALLEGRO_COLOR MENU_SELECT_COLOR;
ALLEGRO_COLOR MENU_REGULAR_COLOR;
ALLEGRO_COLOR MENU_BG_COLOR_L;
ALLEGRO_COLOR MENU_BG_COLOR_D;

int HIGHSCORE_CHARACTERS;

namespace {

ALLEGRO_COLOR parse_color(MoonValue *value, const std::string& symbol)
{
    if ((value->type != MN_ARRAY) ||
        (value->data.compound == NULL || value->data.compound->type != MN_REAL) ||
        (value->data.compound->next == NULL || value->data.compound->next->type != MN_REAL) ||
        (value->data.compound->next->next == NULL || value->data.compound->next->next->type != MN_REAL)) {
            std::cerr << "Type error while reading config value " << symbol << std::endl;
            exit(1);
    }
    return al_map_rgb_f(
        value->data.compound->data.real,
        value->data.compound->next->data.real,
        value->data.compound->next->next->data.real);
}

double load_real(const std::string& symbol)
{
    MoonValue *value = mn_exec_command(symbol.c_str());
    if (value->type != MN_REAL) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    double result = value->data.real;
    mn_dispose(value);
    return result;
}

int load_int(const std::string& symbol)
{
    MoonValue *value = mn_exec_command(symbol.c_str());
    if (value->type != MN_INT) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    int result = value->data.integer;
    mn_dispose(value);
    return result;
}

ALLEGRO_COLOR load_color(const std::string& symbol)
{
    MoonValue *value = mn_exec_command(symbol.c_str());
    ALLEGRO_COLOR result = parse_color(value, symbol);
    mn_dispose(value);
    return result;
}

std::vector<ALLEGRO_COLOR> load_color_vector(const std::string& symbol)
{
    MoonValue *value = mn_exec_command(symbol.c_str());
    if (value->type != MN_ARRAY) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    MoonValue *item = value->data.compound;
    std::vector<ALLEGRO_COLOR> result;
    while (item) {
        result.push_back(parse_color(item, symbol));
        item = item->next;
    }
    mn_dispose(value);
    return result;
}

glm::vec3 load_vec3(const std::string& symbol)
{
    MoonValue *value = mn_exec_command(symbol.c_str());
    if ((value->type != MN_ARRAY) ||
        (value->data.compound == NULL || value->data.compound->type != MN_REAL) ||
        (value->data.compound->next == NULL || value->data.compound->next->type != MN_REAL) ||
        (value->data.compound->next->next == NULL || value->data.compound->next->next->type != MN_REAL)) {
            std::cerr << "Type error while reading config value " << symbol << std::endl;
            exit(1);
    }
    glm::vec3 result {
        value->data.compound->data.real,
        value->data.compound->next->data.real,
        value->data.compound->next->next->data.real };
    mn_dispose(value);
    return result;
}

}

void load()
{
    mn_init();

    if (!mn_exec_file("config.mn")) {
        std::cerr << "Failed loading configuration file." << std::endl;
        std::cerr << "Error message: " << mn_error_message() << std::endl;
        exit(1);
    }

    SCREEN_W = load_int("SCREEN_W");
    SCREEN_H = load_int("SCREEN_H");
    FPS = load_real("FPS");

    FRAME_REST = load_real("FRAME_REST");
    MAX_FRAME_TIME = load_real("MAX_FRAME_TIME");

    BG_COLOR = load_color("BG_COLOR");

    FIELD_COLOR = load_color("FIELD_COLOR");
    FIELD_W = load_real("FIELD_W");
    FIELD_THICK = load_real("FIELD_THICK");
    FIELD_MARGIN = load_real("FIELD_MARGIN");

    BOARD_W = load_int("BOARD_W");
    BOARD_H = load_int("BOARD_H");

    BOARD_SHIFT_X = load_real("BOARD_SHIFT_X");
    BOARD_SHIFT_Y = load_real("BOARD_SHIFT_Y");

    BALL_JUMP_H = load_real("BALL_JUMP_H");
    BALL_RADIUS = load_real("BALL_RADIUS");
    BALL_COLORS = load_color_vector("BALL_COLORS");
    BALL_COLOR_FILTER = load_vec3("BALL_COLOR_FILTER");

    SCORE_COLOR = load_color("SCORE_COLOR");
    SCORE_FONT_SIZE = load_int("SCORE_FONT_SIZE");
    SCORE_SHIFT_X = load_int("SCORE_SHIFT_X");
    SCORE_SHIFT_Y = load_int("SCORE_SHIFT_Y");

    SERIE_MIN = load_int("SERIE_MIN");

    DEAL_PERIOD = load_real("DEAL_PERIOD");
    DEAL_COUNT_INIT = load_int("DEAL_COUNT_INIT");
    DEAL_COUNT_INGAME = load_int("DEAL_COUNT_INGAME");

    MOVE_PERIOD = load_real("MOVE_PERIOD");
    BUMP_PERIOD = load_real("BUMP_PERIOD");
    SCORE_PERIOD = load_real("SCORE_PERIOD");

    GAMEOVER_SHIFT_X = load_int("GAMEOVER_SHIFT_X");
    GAMEOVER_SHIFT_Y = load_int("GAMEOVER_SHIFT_Y");
    GAMEOVER_FONT_SIZE = load_int("GAMEOVER_FONT_SIZE");
    GAMEOVER_PERIOD = load_real("GAMEOVER_PERIOD");

    MENU_PADDING = load_real("MENU_PADDING");
    MENU_MARGIN = load_real("MENU_MARGIN");
    MENU_FONT_SIZE = load_int("MENU_FONT_SIZE");

    MENU_SELECT_COLOR = load_color("MENU_SELECT_COLOR");
    MENU_REGULAR_COLOR = load_color("MENU_REGULAR_COLOR");
    MENU_BG_COLOR_L = load_color("MENU_BG_COLOR_L");
    MENU_BG_COLOR_D = load_color("MENU_BG_COLOR_D");

    HIGHSCORE_CHARACTERS = load_int("HIGHSCORE_CHARACTERS");
}

}
