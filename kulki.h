// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef KULKI_H
#define KULKI_H

#include <deque>
#include <vector>
#include <random>
#include <utility>
#include <stdexcept>
#include <functional>

#include <glm/glm.hpp>

#include "board.h"
#include "allegro.h"
#include "kulki_states.h"

void draw_field(
        const glm::vec3& top_left, const glm::vec3& bot_right,
        bool fill, const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;

    if (fill) {
        al_draw_filled_rectangle(x1, y1, x2, y2, config::FIELD_COLOR);
    } else {
        al_draw_rectangle(x1, y1, x2, y2, config::FIELD_COLOR, config::FIELD_THICK);
    }
}

void draw_ball(double x, double y, int color, double r, const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;

    float red, green, blue;
    al_unmap_rgb_f(config::BALL_COLORS[color], &red, &green, &blue);

    ALLEGRO_COLOR filtered_color = al_map_rgb_f(
            red * config::BALL_COLOR_FILTER.r,
            green * config::BALL_COLOR_FILTER.g,
            blue * config::BALL_COLOR_FILTER.b);

    al_draw_filled_circle(c.x, c.y, r, filtered_color);
}

void draw_board(const Board& b, int hlx, int hly, const glm::mat3& transf)
{
    for (int x = 0; x < b.m_width; ++x) {
        for (int y = 0; y < b.m_height; ++y) {
            draw_field(
                glm::vec3 { x + config::FIELD_MARGIN, y + config::FIELD_MARGIN, 1.0 },
                glm::vec3 { x + 1.0 - config::FIELD_MARGIN, y + 1.0 - config::FIELD_MARGIN, 1.0 },
                x == hlx && y == hly,
                transf);
            if (b(x, y) != config::EMPTY) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), config::BALL_RADIUS, transf);
            }
        }
    }
}

class Kulki {

    bool m_alive;
    Board m_board;
    ALLEGRO_FONT* m_gameover_font;
    ALLEGRO_FONT* m_score_font;
    ALLEGRO_FONT* m_menu_font;

    std::pair<int, int> m_cursor_screen;
    std::pair<int, int> m_cursor_tile;
    int m_score;

    KulkiContext m_state_context;

    Kulki(const Kulki&);    // Not copyable ...
    Kulki(Kulki&&);         // ... or moveable.

    // Helpers.
    // --------

    glm::mat3 m_current_transform()
    {
        return scale(config::FIELD_W) * translate(config::BOARD_SHIFT_X, config::BOARD_SHIFT_Y);
    }

    void m_on_key(int key, bool down)
    {
        if (key == ALLEGRO_KEY_ESCAPE && down) {
            m_state_context.set_state_menu();
            return;
        }

        m_state_context.m_current_state->on_key(key, down);
    }

    void m_on_button(int button, bool down)
    {
        if (!down) return;
        else m_state_context.m_current_state->on_button(button, down);
    }

    void m_on_cursor(int x, int y)
    {
        m_cursor_screen.first = x;
        m_cursor_screen.second = y;

        glm::mat3 inv = glm::inverse(m_current_transform());
        glm::vec3 screen_pos { x, y, 1 };
        glm::vec3 tile_pos = screen_pos * inv;

        m_cursor_tile.first = tile_pos.x;
        m_cursor_tile.second = tile_pos.y;

        m_state_context.m_current_state->on_cursor(x, y);
    }

    void m_tick(double dt)
    {
        m_state_context.m_current_state->tick(dt);
    }

    void m_draw(double)
    {
        glm::mat3 transf = m_current_transform();

        al_clear_to_color(config::BG_COLOR);

        draw_board(m_board, m_cursor_tile.first, m_cursor_tile.second, transf);
        al_draw_textf(m_score_font,
                config::SCORE_COLOR,
                config::SCORE_SHIFT_X,
                config::SCORE_SHIFT_Y,
                ALLEGRO_ALIGN_LEFT,
                "Score : %d",
                m_score);
        m_state_context.m_current_state->draw(transf);

        al_flip_display();
    }

public:
    Kulki() :
        m_alive { true },
        m_board { config::BOARD_W, config::BOARD_H },
        m_gameover_font { al_load_font("prstartk.ttf", -config::GAMEOVER_FONT_SIZE, 0) },
        m_score_font { al_load_font("prstartk.ttf", -config::SCORE_FONT_SIZE, 0) },
        m_menu_font { al_load_font("prstartk.ttf", -config::MENU_FONT_SIZE, 0) },
        m_cursor_screen { -1, -1 },
        m_cursor_tile { -1, -1 },
        m_score { 0 },
        m_state_context { &m_board, &m_score, &m_alive, &m_cursor_tile, m_score_font, m_gameover_font, m_menu_font }
    {
        m_state_context.set_state_menu();
    }

    ~Kulki()
    {
        al_destroy_font(m_gameover_font);
        al_destroy_font(m_score_font);
        al_destroy_font(m_menu_font);
    }

    void run(Allegro& al)
    {
        using namespace std::placeholders;
        al.realtime_loop(
            std::bind(&Kulki::m_on_key, this, _1, _2),
            std::bind(&Kulki::m_on_button, this, _1, _2),
            std::bind(&Kulki::m_on_cursor, this, _1, _2),
            std::bind(&Kulki::m_tick, this, _1),
            std::bind(&Kulki::m_draw, this, _1),
            m_alive);
    }
};

#endif
