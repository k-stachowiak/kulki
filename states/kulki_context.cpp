// Copyright (C) 2015 Krzysztof Stachowiak

#include <cassert>
#include <algorithm>
#include <random>
#include <set>

#include <allegro5/allegro_primitives.h>

#include "kulki_context.h"

KulkiContext::KulkiContext(KulkiConfig &config) :
    m_const { m_resources, config },
    m_var { m_const, config },
    m_input_state { new dick::InputState },
    m_gui {
        m_input_state,
        m_resources
    },
    m_machine { make_menu_state(this) }
{}

glm::mat3 KulkiContext::current_transform()
{
    return scale(m_const.field_w) * translate(m_const.board_shift_x, m_const.board_shift_y);
}

std::unique_ptr<dick::GUI::Widget> KulkiContext::make_score_label()
{
    return m_gui.make_label_ex(
        "Score : " + std::to_string(m_var.m_score),
        m_const.score_font,
        { 4, 4 });
}

std::unique_ptr<dick::GUI::Widget> KulkiContext::make_giveup_button(
        dick::GUI::Callback on_click)
{
    auto result = m_gui.make_button(m_gui.make_label("Give up"), on_click);
    result->align(
        { static_cast<double>(m_const.screen_w) - 2, 0.0 + 4 },
        dick::GUI::Alignment::TOP | dick::GUI::Alignment::RIGHT);
    return result;
}

std::unique_ptr<dick::GUI::Widget> KulkiContext::make_giveup_dialog(
        dick::GUI::Callback on_yes,
        dick::GUI::Callback on_no)
{
    auto result = m_gui.make_dialog_yes_no(
        "Give up?",
        on_yes, on_no,
        {
            m_const.screen_w / 2.0,
            m_const.screen_h / 2.0
        });
    result->align(
        {
            static_cast<double>(m_const.screen_w / 2),
            static_cast<double>(m_const.screen_h / 2)
        },
        dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER);
    return result;
}

void KulkiContext::draw_field(
        const glm::vec3& top_left,
        const glm::vec3& bot_right,
        bool fill,
        const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;

    double image_w = al_get_bitmap_width(m_const.tile_bmp);
    double image_h = al_get_bitmap_height(m_const.tile_bmp);
    double xc = (x1 + x2) / 2.0;
    double yc = (y1 + y2) / 2.0;

    al_draw_rotated_bitmap(
            m_const.tile_bmp,
            image_w * 0.5,
            image_h * 0.5,
            xc, yc,
            0.0,
            0);

    if (fill) {
        al_draw_filled_rectangle(
                x1, y1, x2, y2,
                al_map_rgb_f(
                    m_const.field_color.r,
                    m_const.field_color.g,
                    m_const.field_color.b));
    } else {
        al_draw_rectangle(
                x1, y1, x2, y2,
                al_map_rgb_f(
                    m_const.field_color.r,
                    m_const.field_color.g,
                    m_const.field_color.b),
                m_const.field_thick);
    }

}

void KulkiContext::draw_ball(
        double x,
        double y,
        int color_index,
        double r,
        double squeeze,
        const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;

    ALLEGRO_COLOR color = al_map_rgb_f(
            m_const.ball_colors[color_index].r,
            m_const.ball_colors[color_index].g,
            m_const.ball_colors[color_index].b);

    double image_w = al_get_bitmap_width(m_const.ball_bmp);
    double xscale = 2.0 * r / image_w;
    double image_h = al_get_bitmap_height(m_const.ball_bmp);
    double yscale = 2.0 * r / image_h;

    al_draw_tinted_scaled_rotated_bitmap(
            m_const.ball_bmp,
            color,
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
                glm::vec3 { x + m_const.field_margin, y + m_const.field_margin, 1.0 },
                glm::vec3 { x + 1.0 - m_const.field_margin, y + 1.0 - m_const.field_margin, 1.0 },
                x == m_var.m_cursor_tile.first && y == m_var.m_cursor_tile.second,
                transf);
            if (b(x, y) != m_const.empty_field) {
                draw_ball(x + 0.5, y + 0.5, b(x, y), m_const.ball_radius, 1.0, transf);
            }
        }
    }
}

bool KulkiContext::is_over() const
{
    return m_machine.is_over();
}

void KulkiContext::on_key(dick::Key key, bool down)
{
    m_input_state->on_key(key, down);
    m_machine.on_key(key, down);
}

void KulkiContext::on_button(dick::Button button, bool down)
{
    m_input_state->on_button(button, down);
    m_machine.on_button(button, down);
}

void KulkiContext::on_cursor(dick::DimScreen position)
{
    glm::mat3 inv = glm::inverse(current_transform());
    glm::vec3 screen_pos { position.x, position.y, 1 };
    glm::vec3 tile_pos = screen_pos * inv;

    m_var.m_cursor_tile.first = floor(tile_pos.x);
    m_var.m_cursor_tile.second = floor(tile_pos.y);

    m_input_state->on_cursor(position);
    m_machine.on_cursor(position);
}

void KulkiContext::tick(double dt)
{
    m_machine.tick(dt);
}

void KulkiContext::draw(double weight)
{
    glm::mat3 transf = current_transform();

    al_clear_to_color(al_map_rgb_f(
                m_const.bg_color.r,
                m_const.bg_color.g,
                m_const.bg_color.b));

    int next_fields_to_draw = std::max(
            m_const.deal_count_ingame,
            static_cast<int>(m_var.m_next_deal.size()));

    for (int i = 0; i < next_fields_to_draw; ++i) {
        draw_field(
                { -2.0 + m_const.field_margin, i + m_const.field_margin, 1.0 },
                { -1.0 - m_const.field_margin, i + 1.0 - m_const.field_margin, 1.0 },
                false,
                transf);
    }

    for (int i = 0; i < m_var.m_next_deal.size(); ++i) {
        draw_ball(
                -2.0 + 0.5,
                i + 0.5,
                m_var.m_next_deal[i],
                m_const.ball_radius,
                1.0,
                transf);
    }

    draw_board(m_var.m_board, transf);
    m_machine.draw(weight);

    al_flip_display();
}

