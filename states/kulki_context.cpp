#include <cassert>
#include <random>
#include <set>

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

KulkiContext::KulkiContext(
        Board* board,
        bool* alive,
        Resources &resources,
        std::pair<int, int>* cursor_tile,
        int ball_count) :

    m_board { board },
    m_alive { alive },

    m_gameover_font { resources.get_font("data/prstartk.ttf", -config::GAMEOVER_FONT_SIZE) },
    m_score_font { resources.get_font("data/prstartk.ttf", -config::SCORE_FONT_SIZE) },
    m_menu_font { resources.get_font("data/prstartk.ttf", -config::MENU_FONT_SIZE) },
    m_ball_bmp { resources.get_bitmap("data/ball2.png") },
    m_tile_bmp { resources.get_bitmap("data/tile.png") },

    m_cursor_tile { cursor_tile },
    m_ball_count { ball_count },

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
}

void KulkiContext::gen_next_deal(int count)
{
    std::uniform_int_distribution<int> distr_color(0, m_ball_count - 1);
    for (int i = 0; i < count; ++i) {
        m_next_deal.push_back(distr_color(engine));
    }
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
                x == m_cursor_tile->first && y == m_cursor_tile->second,
                transf);
            if (b(x, y) != config::EMPTY) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), config::BALL_RADIUS, 1.0, transf);
            }
        }
    }
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
    (*m_board)(src_x, src_y) = color;
    set_state_wait_ball();
}

void KulkiContext::set_state_wait_dest(int src_x, int src_y)
{
    m_wait_dest_state.reset(src_x, src_y, (*m_board)(src_x, src_y), 0);
    (*m_board)(src_x, src_y) = config::EMPTY;
    m_current_state = &m_wait_dest_state;
}

void KulkiContext::reset_state_wait_dest(int src_x, int src_y)
{
    (*m_board)(m_wait_dest_state.get_src_x(), m_wait_dest_state.get_src_y()) = m_wait_dest_state.get_color();
    set_state_wait_dest(src_x, src_y);
}

void KulkiContext::set_state_deal()
{
    if (m_board->free_fields() <= config::DEAL_COUNT_INGAME) {
        set_state_high_score();
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

void KulkiContext::set_state_score(const std::vector<std::pair<int, int>>& changes, KulkiState::Enum next_state)
{
    std::unordered_set<std::pair<int, int>> scored;
    bool success = false;

    for (const auto& p : changes) {
        success |= m_board->find_streak(p, std::inserter(scored, begin(scored)));
    }

    if (success) {
        ++m_streak;

    } else {
        m_streak = 0;
        switch (next_state) {
        case KulkiState::Enum::DEAL:
            set_state_deal();
            return;
        case KulkiState::Enum::WAIT_BALL:
            set_state_wait_ball();
            return;
        default:
            throw std::domain_error("m_set_state_score: bad next_state argument");
        }
    }

    int x_sum = 0, y_sum = 0;
    for (const auto& p : scored) {
        (*m_board)(p.first, p.second) = config::EMPTY;
        x_sum += p.first;
        y_sum += p.second;
    }

    int score_incr = scored.size() * m_streak;
    m_score += score_incr;

    m_score_state.reset(
            config::SCORE_PERIOD,
            double(x_sum) / double(scored.size()) + 0.5,
            double(y_sum) / double(scored.size()) + 0.5,
            score_incr);

    m_current_state = &m_score_state;
}

void KulkiContext::set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
{
    std::deque<std::pair<int, int>> path;

    if (!m_board->find_path({ src_x, src_y }, { dst_x, dst_y }, path)) {
        reset_state_wait_ball(src_x, src_y, color);
        return;
    }

    m_move_state.reset(std::move(path), config::MOVE_PERIOD, dst_x, dst_y, color);
    m_current_state = &m_move_state;
}

void KulkiContext::set_state_high_score()
{
    m_high_score_state.reset(m_score);
    m_current_state = &m_high_score_state;
}
