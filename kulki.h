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
#include "resources.h"
#include "kulki_state.h"
#include "kulki_context.h"

class Kulki {

//    bool m_alive;
//    Board m_board;

//    Resources m_resources;
//    ALLEGRO_FONT *m_score_font;
//
//    std::pair<int, int> m_cursor_screen;
//    std::pair<int, int> m_cursor_tile;
//
//    KulkiContext m_state_context;
//
//    Kulki(const Kulki&);    // Not copyable ...
//    Kulki(Kulki&&);         // ... or moveable.

    // Helpers.
    // --------

//    glm::mat3 m_current_transform()
//    {
//        return scale(config::FIELD_W) * translate(config::BOARD_SHIFT_X, config::BOARD_SHIFT_Y);
//    }

//    void m_on_key(int key, bool down)
//    {
//        m_state_context.m_current_state->on_key(key, down);
//    }

//    void m_on_button(int button, bool down)
//    {
//        if (!down) return;
//        else m_state_context.m_current_state->on_button(button, down);
//    }

//    void m_on_cursor(int x, int y)
//    {
//        m_cursor_screen.first = x;
//        m_cursor_screen.second = y;
//
//        glm::mat3 inv = glm::inverse(m_current_transform());
//        glm::vec3 screen_pos { x, y, 1 };
//        glm::vec3 tile_pos = screen_pos * inv;
//
//        m_cursor_tile.first = floor(tile_pos.x);
//        m_cursor_tile.second = floor(tile_pos.y);
//
//        m_state_context.m_current_state->on_cursor(x, y);
//    }

//    void m_tick(double dt)
//    {
//        m_state_context.m_current_state->tick(dt);
//    }

//    void m_draw(double)
//    {
//        glm::mat3 transf = m_current_transform();
//
//        al_clear_to_color(config::BG_COLOR);
//
//        al_draw_textf(m_score_font,
//                config::SCORE_COLOR, config::SCORE_SHIFT_X, config::SCORE_SHIFT_Y, ALLEGRO_ALIGN_LEFT,
//                "Score : %d", m_state_context.m_score);
//
//        int ball_index = 0;
//        for (int color : m_state_context.m_next_deal) {
//            m_state_context.draw_ball(-2 + 0.5, ball_index++ + 0.5, color, config::BALL_RADIUS, 1.0, transf);
//        }
//
//        m_state_context.draw_board(m_board, transf);
//
//        m_state_context.m_current_state->draw(transf);
//
//        al_flip_display();
//    }

public:
//    Kulki() :
//        m_alive { true },
//        m_board { config::BOARD_W, config::BOARD_H },
//        m_score_font { m_resources.get_font("data/prstartk.ttf", -config::SCORE_FONT_SIZE) },
//        m_cursor_screen { -1, -1 },
//        m_cursor_tile { -1, -1 },
//        m_state_context { &m_board, &m_alive, m_resources, &m_cursor_tile, config::COLOR_COUNT }
//    {
//        m_state_context.set_state_menu();
//    }

//    void run(Allegro& al)
//    {
//        using namespace std::placeholders;
//        al.realtime_loop(
//            std::bind(&Kulki::m_on_key, this, _1, _2),
//            std::bind(&Kulki::m_on_button, this, _1, _2),
//            std::bind(&Kulki::m_on_cursor, this, _1, _2),
//            std::bind(&Kulki::m_tick, this, _1),
//            std::bind(&Kulki::m_draw, this, _1),
//            m_alive);
//    }
};

#endif
