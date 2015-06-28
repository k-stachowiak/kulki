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

KulkiContext::KulkiContext(KulkiConfig &config) :

    m_constants { m_resources, config },
    m_board { m_constants.board_w, m_constants.board_h, m_constants.empty_field },

    m_cursor_screen { -1, -1 },
    m_cursor_tile { -1, -1 },
    m_score { 0 },
    m_streak { 0 },

    m_machine { std::shared_ptr<dick::StateNode> { new MenuState { this } } }

{}

void KulkiContext::gen_next_deal(int count)
{
    std::uniform_int_distribution<int> distr_color(0, m_constants.ball_count - 1);
    for (int i = 0; i < count; ++i) {
        m_next_deal.push_back(distr_color(engine));
    }
}

glm::mat3 KulkiContext::m_current_transform()
{
    return scale(m_constants.field_w) * translate(m_constants.board_shift_x, m_constants.board_shift_y);
}

void KulkiContext::draw_field(
        const glm::vec3& top_left, const glm::vec3& bot_right,
        bool fill, const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;

    double image_w = al_get_bitmap_width(m_constants.tile_bmp);
    double image_h = al_get_bitmap_height(m_constants.tile_bmp);
    double xc = (x1 + x2) / 2.0;
    double yc = (y1 + y2) / 2.0;

    al_draw_rotated_bitmap(
            m_constants.tile_bmp,
            image_w * 0.5,
            image_h * 0.5,
            xc, yc,
            0.0,
            0);

    if (fill) {
        al_draw_filled_rectangle(x1, y1, x2, y2, m_constants.field_color);
    } else {
        al_draw_rectangle(x1, y1, x2, y2, m_constants.field_color, m_constants.field_thick);
    }

}

void KulkiContext::draw_ball(
        double x, double y, int color, double r,
        double squeeze,
        const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;

    float red, green, blue;
    al_unmap_rgb_f(m_constants.ball_colors[color], &red, &green, &blue);

    ALLEGRO_COLOR filtered_color = al_map_rgb_f(
            red * m_constants.ball_color_filter.r,
            green * m_constants.ball_color_filter.g,
            blue * m_constants.ball_color_filter.b);

    double image_w = al_get_bitmap_width(m_constants.ball_bmp);
    double xscale = 2.0 * r / image_w;
    double image_h = al_get_bitmap_height(m_constants.ball_bmp);
    double yscale = 2.0 * r / image_h;

    al_draw_tinted_scaled_rotated_bitmap(
            m_constants.ball_bmp,
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
                glm::vec3 { x + m_constants.field_margin, y + m_constants.field_margin, 1.0 },
                glm::vec3 { x + 1.0 - m_constants.field_margin, y + 1.0 - m_constants.field_margin, 1.0 },
                x == m_cursor_tile.first && y == m_cursor_tile.second,
                transf);
            if (b(x, y) != m_constants.empty_field) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), m_constants.ball_radius, 1.0, transf);
            }
        }
    }
}

bool KulkiContext::is_over() const
{
    return m_machine.is_over();
}

void KulkiContext::on_key(int key, bool down)
{
    m_machine.on_key(key, down);
}

void KulkiContext::on_button(int button, bool down)
{
    m_machine.on_button(button, down);
}

void KulkiContext::on_cursor(dick::DimScreen position)
{
    m_cursor_screen.first = position.x;
    m_cursor_screen.second = position.y;

    glm::mat3 inv = glm::inverse(m_current_transform());
    glm::vec3 screen_pos { position.x, position.y, 1 };
    glm::vec3 tile_pos = screen_pos * inv;

    m_cursor_tile.first = floor(tile_pos.x);
    m_cursor_tile.second = floor(tile_pos.y);

    m_machine.on_cursor(position);
}

void KulkiContext::tick(double dt)
{
    m_machine.tick(dt);
}

void KulkiContext::draw(double weight)
{
    glm::mat3 transf = m_current_transform();

    al_clear_to_color(m_constants.bg_color);

    al_draw_textf(m_constants.score_font,
            m_constants.score_color, m_constants.score_shift_x, m_constants.score_shift_y, ALLEGRO_ALIGN_LEFT,
            "Score : %d", m_score);

    int ball_index = 0;
    for (int color : m_next_deal) {
        draw_ball(-2 + 0.5, ball_index++ + 0.5, color, m_constants.ball_radius, 1.0, transf);
    }

    draw_board(m_board, transf);
    m_machine.draw(weight);

    al_flip_display();
}

