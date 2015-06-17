#include <cassert>
#include <random>
#include <set>

#include <allegro5/allegro_primitives.h>

#include "kulki_context.h"

#include "deal_state.h"
#include "game_over_state.h"
#include "high_score_state.h"
#include "menu_state.h"
#include "move_state.h"
#include "score_state.h"
#include "wait_ball_state.h"
#include "wait_dest_state.h"

namespace {
    std::random_device device;
    std::mt19937 engine { device() };
}

KulkiContext::KulkiContext() :

    m_board { config::BOARD_W, config::BOARD_H },
    m_alive { true },

    m_gameover_font { m_resources.get_font("data/prstartk.ttf", -config::GAMEOVER_FONT_SIZE) },
    m_score_font { m_resources.get_font("data/prstartk.ttf", -config::SCORE_FONT_SIZE) },
    m_menu_font { m_resources.get_font("data/prstartk.ttf", -config::MENU_FONT_SIZE) },
    m_ball_bmp { m_resources.get_bitmap("data/ball2.png") },
    m_tile_bmp { m_resources.get_bitmap("data/tile.png") },
    m_cursor_screen { -1, -1 },
    m_cursor_tile { -1, -1 },

    m_ball_count { config::COLOR_COUNT },

    m_menu_state { this },
    m_deal_state { this },
    m_wait_ball_state { this },
    m_wait_dest_state { this },
    m_move_state { this },
    m_score_state { this },
    m_gameover_state { this },
    m_high_score_state { this },
    m_current_state { nullptr },

    m_score { 0 },
    m_streak { 0 }
{
    assert(m_ball_count <= static_cast<int>(config::BALL_COLORS.size()));
    set_state_menu();
}

void KulkiContext::gen_next_deal(int count)
{
    std::uniform_int_distribution<int> distr_color(0, m_ball_count - 1);
    for (int i = 0; i < count; ++i) {
        m_next_deal.push_back(distr_color(engine));
    }
}

glm::mat3 KulkiContext::m_current_transform()
{
    return scale(config::FIELD_W) * translate(config::BOARD_SHIFT_X, config::BOARD_SHIFT_Y);
}

void KulkiContext::draw_field(
        const glm::vec3& top_left, const glm::vec3& bot_right,
        bool fill, const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;

    double image_w = al_get_bitmap_width(m_tile_bmp);
    double image_h = al_get_bitmap_height(m_tile_bmp);
    double xc = (x1 + x2) / 2.0;
    double yc = (y1 + y2) / 2.0;

    al_draw_rotated_bitmap(
            m_tile_bmp,
            image_w * 0.5,
            image_h * 0.5,
            xc, yc,
            0.0,
            0);

    if (fill) {
        al_draw_filled_rectangle(x1, y1, x2, y2, config::FIELD_COLOR);
    } else {
        al_draw_rectangle(x1, y1, x2, y2, config::FIELD_COLOR, config::FIELD_THICK);
    }

}

void KulkiContext::draw_ball(
        double x, double y, int color, double r,
        double squeeze,
        const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;

    float red, green, blue;
    al_unmap_rgb_f(config::BALL_COLORS[color], &red, &green, &blue);

    ALLEGRO_COLOR filtered_color = al_map_rgb_f(
            red * config::BALL_COLOR_FILTER.r,
            green * config::BALL_COLOR_FILTER.g,
            blue * config::BALL_COLOR_FILTER.b);

    double image_w = al_get_bitmap_width(m_ball_bmp);
    double xscale = 2.0 * r / image_w;
    double image_h = al_get_bitmap_height(m_ball_bmp);
    double yscale = 2.0 * r / image_h;

    al_draw_tinted_scaled_rotated_bitmap(
            m_ball_bmp,
            filtered_color,
            image_w * 0.5,
            image_h * 0.5,
            c.x, c.y,
            xscale, yscale * squeeze,
            0.0,
            0);
}

void KulkiContext::draw_board(const Board& b, const glm::mat3& transf)
{
    for (int x = 0; x < b.m_width; ++x) {
        for (int y = 0; y < b.m_height; ++y) {
            draw_field(
                glm::vec3 { x + config::FIELD_MARGIN, y + config::FIELD_MARGIN, 1.0 },
                glm::vec3 { x + 1.0 - config::FIELD_MARGIN, y + 1.0 - config::FIELD_MARGIN, 1.0 },
                x == m_cursor_tile.first && y == m_cursor_tile.second,
                transf);
            if (b(x, y) != config::EMPTY) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), config::BALL_RADIUS, 1.0, transf);
            }
        }
    }
}

bool KulkiContext::is_done() const
{
    return !m_alive;
}

void KulkiContext::on_kill()
{
    m_alive = false;
}

void KulkiContext::on_key(int key, bool down)
{
    m_current_state->on_key(key, down);
}

void KulkiContext::on_button(int button, bool down)
{
    m_current_state->on_button(button, down);
}

void KulkiContext::on_cursor(int x, int y)
{
    m_cursor_screen.first = x;
    m_cursor_screen.second = y;

    glm::mat3 inv = glm::inverse(m_current_transform());
    glm::vec3 screen_pos { x, y, 1 };
    glm::vec3 tile_pos = screen_pos * inv;

    m_cursor_tile.first = floor(tile_pos.x);
    m_cursor_tile.second = floor(tile_pos.y);

    m_current_state->on_cursor(x, y);
}

void KulkiContext::tick(double dt)
{
    m_current_state->tick(dt);
}

void KulkiContext::draw(double weight)
{
    glm::mat3 transf = m_current_transform();

    al_clear_to_color(config::BG_COLOR);

    al_draw_textf(m_score_font,
            config::SCORE_COLOR, config::SCORE_SHIFT_X, config::SCORE_SHIFT_Y, ALLEGRO_ALIGN_LEFT,
            "Score : %d", m_score);

    int ball_index = 0;
    for (int color : m_next_deal) {
        draw_ball(-2 + 0.5, ball_index++ + 0.5, color, config::BALL_RADIUS, 1.0, transf);
    }

    draw_board(m_board, transf);
    m_current_state->draw(weight);

    al_flip_display();
}

void KulkiContext::set_state_menu()
{
    m_menu_state.reset();
    m_current_state = &m_menu_state;
}

void KulkiContext::set_state_wait_ball()
{
    m_current_state = &m_wait_ball_state;
}

void KulkiContext::reset_state_wait_ball(int src_x, int src_y, int color)
{
    m_board(src_x, src_y) = color;
    set_state_wait_ball();
}

void KulkiContext::set_state_wait_dest(int src_x, int src_y)
{
    m_wait_dest_state.reset(src_x, src_y, m_board(src_x, src_y), 0);
    m_board(src_x, src_y) = config::EMPTY;
    m_current_state = &m_wait_dest_state;
}

void KulkiContext::reset_state_wait_dest(int src_x, int src_y)
{
    m_board(m_wait_dest_state.get_src_x(), m_wait_dest_state.get_src_y()) = m_wait_dest_state.get_color();
    set_state_wait_dest(src_x, src_y);
}

void KulkiContext::set_state_deal()
{
    if (m_board.free_fields() <= config::DEAL_COUNT_INGAME) {
        set_state_gameover();
        return;
    }

    m_deal_state.reset(config::DEAL_PERIOD);
    m_current_state = &m_deal_state;
}

void KulkiContext::set_state_gameover()
{
    m_gameover_state.reset(config::GAMEOVER_PERIOD, 0);
    m_current_state = &m_gameover_state;
}

void KulkiContext::set_state_score(const std::vector<std::pair<int, int>>& changes, bool next_deal)
{
    m_score_state.reset(changes, next_deal);
    m_current_state = &m_score_state;
}

void KulkiContext::set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
{
    std::deque<std::pair<int, int>> path;

    if (!m_board.find_path({ src_x, src_y }, { dst_x, dst_y }, path)) {
        reset_state_wait_ball(src_x, src_y, color);
        return;
    }

    m_move_state.reset(std::move(path), config::MOVE_PERIOD, dst_x, dst_y, color);
    m_current_state = &m_move_state;
}

void KulkiContext::set_state_high_score()
{
    m_high_score_state.reset(m_ball_count, m_score);
    m_current_state = &m_high_score_state;
}
