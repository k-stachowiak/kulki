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

void draw_field(
        const glm::vec3& top_left, const glm::vec3& bot_right,
        bool fill, const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;

    if (fill) {
        al_draw_filled_rectangle(x1, y1, x2, y2, FIELD_COLOR);
    } else {
        al_draw_rectangle(x1, y1, x2, y2, FIELD_COLOR, FIELD_THICK);
    }
}

void draw_ball(double x, double y, int color, double r, const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;
    al_draw_filled_circle(c.x, c.y, r, BALL_COLORS[color]);
}

void draw_board(const Board& b, int hlx, int hly, const glm::mat3& transf)
{
    for (int x = 0; x < b.m_width; ++x) {
        for (int y = 0; y < b.m_height; ++y) {
            draw_field(
                glm::vec3 { x + FIELD_MARGIN, y + FIELD_MARGIN, 1.0 },
                glm::vec3 { x + 1.0 - FIELD_MARGIN, y + 1.0 - FIELD_MARGIN, 1.0 },
                x == hlx && y == hly,
                transf);
            if (b(x, y) != EMPTY) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), BALL_RADIUS, transf);
            }
        }
    }
}

class Kulki {

    enum class State {
        MENU,
        DEAL,
        WAIT_BALL,
        WAIT_DEST,
        MOVE,
        GAMEOVER,
        SCORE
    };

    std::random_device m_rdev;
    std::default_random_engine m_reng;

    bool m_alive;
    Board m_board;
    ALLEGRO_FONT* m_gameover_font;
    ALLEGRO_FONT* m_score_font;

    std::pair<int, int> m_cursor_screen;
    std::pair<int, int> m_cursor_tile;
    int m_score;

    State m_state;

    int m_deal_count;
    double m_deal_time;
    std::vector<std::pair<int, int>> m_deal_positions;

    int m_waitd_src_x, m_waitd_src_y;
    int m_waitd_color;
    double m_waitd_time;

    std::deque<std::pair<int, int>> m_move_path;
    double m_move_time;
    int m_move_dst_x, m_move_dst_y;
    int m_move_color;

    double m_score_time;
    double m_score_cx, m_score_cy;
    int m_score_incr;

    double m_gameover_time;
    int m_gameover_index;

    Kulki(const Kulki&);    // Not copyable ...
    Kulki(Kulki&&);         // ... or moveable.

    // Helpers.
    // --------

    glm::mat3 m_current_transform()
    {
        return scale(FIELD_W) * translate(BOARD_SHIFT_X, BOARD_SHIFT_Y); 
    }

    void m_new_ball(int& x, int& y, int& color)
    {
        std::uniform_int_distribution<int> distr_x(0, m_board.m_width - 1);
        std::uniform_int_distribution<int> distr_y(0, m_board.m_height - 1);
        std::uniform_int_distribution<int> distr_color(0, BALL_COLORS.size() - 1);

        do {
            x = distr_x(m_reng);
            y = distr_y(m_reng);
        } while (m_board(x, y) != EMPTY);

        color = distr_color(m_reng);
    }

    // State transitions.
    // ------------------

    void m_set_state_wait_ball()
    {
        m_state = State::WAIT_BALL;
    }

    void m_reset_state_wait_ball(int src_x, int src_y, int color)
    {
        m_board(src_x, src_y) = color;
        m_set_state_wait_ball();
    }

    void m_set_state_wait_dest(int src_x, int src_y)
    {
        m_waitd_src_x = src_x;
        m_waitd_src_y = src_y;
        m_waitd_color = m_board(src_x, src_y);
        m_waitd_time = 0.0;
        m_board(src_x, src_y) = EMPTY;
        m_state = State::WAIT_DEST;
    }

    void m_reset_state_wait_dest(int src_x, int src_y)
    {
        m_board(m_waitd_src_x, m_waitd_src_y) = m_waitd_color;
        m_set_state_wait_dest(src_x, src_y);
    }

    void m_set_state_deal(int count)
    {
        if (m_board.free_fields() <= count) {
            m_set_state_gameover();
            return;
        }

        m_deal_count = count;
        m_deal_time = DEAL_PERIOD;
        m_deal_positions.clear();
        m_state = State::DEAL;
    }

    void m_set_state_gameover()
    {
        m_gameover_time = GAMEOVER_PERIOD;
        m_gameover_index = 0;
        m_state = State::GAMEOVER;
    }

    void m_set_state_score(const std::vector<std::pair<int, int>>& changes, State next_state)
    {
        std::unordered_set<std::pair<int, int>> streaks;
        bool success = false;

        for (const auto& p : changes) {
            success |= m_board.find_streak(p, std::inserter(streaks, begin(streaks)));
        }

        if (!success) {
            switch (next_state) {
            case State::DEAL:
                m_set_state_deal(DEAL_COUNT_INGAME);
                return;
            case State::WAIT_BALL:
                m_set_state_wait_ball();
                return;
            default:
                throw std::domain_error("m_set_state_score: bad next_state argument");
            }
        }

        int score_incr = 0;
        int x_sum = 0, y_sum = 0;
        for (const auto& p : streaks) {
            m_board(p.first, p.second) = EMPTY;
            ++score_incr;
            x_sum += p.first;
            y_sum += p.second;
        }
        m_score += score_incr;

        m_score_time = SCORE_PERIOD;
        m_score_cx = double(x_sum) / double(streaks.size()) + 0.5;
        m_score_cy = double(y_sum) / double(streaks.size()) + 0.5;
        m_score_incr = score_incr;
        m_state = State::SCORE;
    }

    void m_set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
    {
        std::deque<std::pair<int, int>> path;

        if (!m_board.find_path({ src_x, src_y }, { dst_x, dst_y }, path)) {
            m_reset_state_wait_ball(src_x, src_y, color);
            return;
        }

        m_move_path = std::move(path);
        m_move_time = MOVE_PERIOD;
        m_move_dst_x = dst_x;
        m_move_dst_y = dst_y;
        m_move_color = color;
        m_state = State::MOVE;
    }

    // Tick implementations.
    // ---------------------

    void m_tick_deal(double dt)
    {
        if ((m_deal_time -= dt) > 0) return;
        else m_deal_time = DEAL_PERIOD;

        int x, y, color;
        m_new_ball(x, y, color);
        m_board(x, y) = color;
        m_deal_positions.emplace_back(x, y);

        if ((--m_deal_count) == 0) {
            m_set_state_score(m_deal_positions, State::WAIT_BALL);
        }
    }

    void m_tick_wait_dest(double dt)
    {
        m_waitd_time += dt;
        m_waitd_time = fmod(m_waitd_time, MOVE_PERIOD);
    }

    void m_tick_gameover(double dt)
    {
        if ((m_gameover_time -= dt) > 0) {
            return;
        }

        m_gameover_time = GAMEOVER_PERIOD;
        m_gameover_index = (m_gameover_index + 1) % BALL_COLORS.size();
    }

    void m_tick_move(double dt)
    {
        if ((m_move_time -= dt) > 0) return;
        else m_move_time = MOVE_PERIOD;

        m_move_path.pop_front();

        if (m_move_path.size() == 1) {
            m_board(m_move_dst_x, m_move_dst_y) = m_move_color;
            m_set_state_score({ { m_move_dst_x, m_move_dst_y } }, State::DEAL);
        }
    }

    void m_tick_score(double dt)
    {
        if ((m_score_time -= dt) > 0) return;
        m_set_state_wait_ball();
    }

    // Draw implementations.
    // ---------------------

    void m_draw_wait_dest()
    {
        glm::mat3 transf = m_current_transform();

        double factor = double(m_waitd_time) / MOVE_PERIOD * 3.14;

        double x = double(m_waitd_src_x) + 0.5;
        double y = double(m_waitd_src_y) + 0.5 - sin(factor) * BALL_JUMP_H;

        draw_ball(x, y, m_waitd_color, BALL_RADIUS, transf);
    }

    void m_draw_move()
    {
        glm::mat3 transf = m_current_transform();

        double x1 = m_move_path[0].first;
        double y1 = m_move_path[0].second;
        double x2 = m_move_path[1].first;
        double y2 = m_move_path[1].second;

        double mv_factor = m_move_time / MOVE_PERIOD;
        double bmp_factor = (1.0 - mv_factor) * 3.14;

        double x = x1 * mv_factor + x2 * (1.0 - mv_factor) + 0.5;
        double y = y1 * mv_factor + y2 * (1.0 - mv_factor) + 0.5 - sin(bmp_factor) * BALL_JUMP_H;

        draw_ball(x, y, m_move_color, BALL_RADIUS, transf);
    }

    void m_draw_score()
    {
        glm::vec3 text_center = glm::vec3 { m_score_cx, m_score_cy, 1 } * m_current_transform();
        al_draw_textf(
            m_score_font, SCORE_COLOR,
            text_center.x, text_center.y,
            ALLEGRO_ALIGN_CENTRE,
            "+%d", m_score_incr);
    }

    void m_draw_gameover()
    {
        al_draw_text(
            m_gameover_font, BALL_COLORS[m_gameover_index],
            GAMEOVER_SHIFT_X, GAMEOVER_SHIFT_Y,
            ALLEGRO_ALIGN_CENTRE,
            "Game Over");
    }

    // Common implementations.
    // -----------------------

    void m_on_key(int key, bool down)
    {
        if (key == ALLEGRO_KEY_ESCAPE && down) {
            m_alive = false;
            return;
        }

        switch (m_state) {
        case State::GAMEOVER:
            m_board.clear();
            m_score = 0;
            m_set_state_deal(DEAL_COUNT_INIT);
            break;
        default:
            break;
        }
    }

    void m_on_button(int button, bool down)
    {
        if (!down) return;

        int tx = m_cursor_tile.first;
        int ty = m_cursor_tile.second;
        if (!m_board.has(tx, ty)) {
            return;
        }

        switch (m_state) {
        case State::WAIT_BALL:
            if (m_board(tx, ty) != EMPTY) {
                m_set_state_wait_dest(tx, ty);
            }
            break;
        case State::WAIT_DEST:
            if (tx == m_waitd_src_x && ty == m_waitd_src_y) {
                m_reset_state_wait_ball(m_waitd_src_x, m_waitd_src_y, m_waitd_color);
            } else if (m_board(tx, ty) != EMPTY) {
                m_reset_state_wait_dest(tx, ty);
            } else {
                m_set_state_move(m_waitd_src_x, m_waitd_src_y, tx, ty, m_waitd_color);
            }
            break;
        default:
            break;
        }
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
    }

    void m_tick(double dt)
    {
        switch (m_state) {
        case State::DEAL:
            m_tick_deal(dt);
            break;
        case State::MOVE:
            m_tick_move(dt);
            break;
        case State::SCORE:
            m_tick_score(dt);
            break;
        case State::WAIT_DEST:
            m_tick_wait_dest(dt);
            break;
        case State::GAMEOVER:
            m_tick_gameover(dt);
            break;
        default:
            break;
        }
    }

    void m_draw(double)
    {
        glm::mat3 transf = m_current_transform();

        al_clear_to_color(BG_COLOR);

        draw_board(m_board, m_cursor_tile.first, m_cursor_tile.second, transf);

        al_draw_textf(
            m_score_font, SCORE_COLOR,
            0, 0,
            ALLEGRO_ALIGN_LEFT,
            "Score : %d", m_score);

        switch (m_state)
        {
        case State::WAIT_DEST:
            m_draw_wait_dest();
            break;
        case State::MOVE:
            m_draw_move();
            break;
        case State::SCORE:
            m_draw_score();
            break;
        case State::GAMEOVER:
            m_draw_gameover();
            break;
        default:
            break;
        }

        al_flip_display();
    }

public:
    Kulki() :
        m_reng { m_rdev() },
        m_alive { true },
        m_board { BOARD_W, BOARD_H },
        m_gameover_font { al_load_font("prstartk.ttf", -GAMEOVER_FONT_SIZE, 0) },
        m_score_font { al_load_font("prstartk.ttf", -SCORE_FONT_SIZE, 0) },
        m_cursor_screen { -1, -1 },
        m_cursor_tile { -1, -1 },
        m_score { 0 }
    {
        m_set_state_deal(DEAL_COUNT_INIT);
    }

    ~Kulki()
    {
        al_destroy_font(m_gameover_font);
        al_destroy_font(m_score_font);
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