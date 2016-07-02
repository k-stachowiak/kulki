// Copyright (C) 2015 Krzysztof Stachowiak

#include "dick.h"
#include "kulki_context.h"

class WaitDestState : public dick::StateNode {
    KulkiContext* const m_context;

    int m_src_x, m_src_y;
    int m_dst_x, m_dst_y;
    std::deque<std::pair<int, int>> m_path;
    int m_color;
    double m_time;

    bool m_usure_phase;
    std::unique_ptr<dick::GUI::Widget> m_score_label;
    std::unique_ptr<dick::GUI::Widget> m_usure_dialog;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;

    std::shared_ptr<StateNode> m_next_state;

public:
    WaitDestState(KulkiContext* context, int src_x, int src_y) :
        m_context { context },
        m_src_x { src_x },
        m_src_y { src_y },
        m_dst_x { src_x },
        m_dst_y { src_y },
        m_color { context->m_var.m_board(src_x, src_y) },
        m_usure_phase { false }
    {
        m_score_label = m_context->make_score_label();
        m_giveup_button = m_context->make_giveup_button(
            [this]()
            {
                m_usure_phase = true;
            });

        m_usure_dialog = m_context->make_giveup_dialog(
            [this]()
            {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            },
            [this]()
            {
                m_usure_phase = false;
            });

        m_context->m_var.m_board(src_x, src_y) = m_context->m_const.empty_field;
    }

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        if (m_usure_phase) {
            if (key == dick::Key::ENTER) {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            } else if (key == dick::Key::ESCAPE) {
                m_usure_phase = false;
            }
        } else {
            if (key == dick::Key::ESCAPE) {
                m_usure_phase = true;
            }
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (!down) {
            return;
        }

        if (m_usure_phase) {
            m_usure_dialog->on_click(button);
        } else {
            m_giveup_button->on_click(button);

            if (button != dick::Button::BUTTON_1) {
                return;
            }

            int tx = m_context->m_var.m_cursor_tile.first;
            int ty = m_context->m_var.m_cursor_tile.second;

            if (!m_context->m_var.m_board.has(tx, ty)) {
                return;
            }

            if (m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {

                // Restore current ball
                m_context->m_var.m_board(m_src_x, m_src_y) = m_color;

                // Remember new ball
                m_src_x = tx;
                m_src_y = ty;
                m_color = m_context->m_var.m_board(tx, ty);

                // Remove new ball from board
                m_context->m_var.m_board(m_src_x, m_src_y) = m_context->m_const.empty_field;
                return;
            }

            if (tx == m_src_x && ty == m_src_y) {
                m_context->m_var.m_board(m_src_x, m_src_y) = m_color;
                t_transition_required = true;
                m_next_state = make_wait_ball_state(m_context);

            } else {
                if (!m_path.empty()) {
                    t_transition_required = true;
                    m_next_state = make_move_state(
                        m_context,
                        std::move(m_path),
                        m_context->m_const.move_period,
                        tx, ty,
                        m_color
                    );
                }
            }
        }
    }

    void on_cursor(dick::DimScreen) override
    {
        int tx = m_context->m_var.m_cursor_tile.first;
        int ty = m_context->m_var.m_cursor_tile.second;

        if (!m_context->m_var.m_board.has(tx, ty)) {
            return;
        }

        if (tx == m_dst_x && ty == m_dst_y) {
            return;
        }

        m_dst_x = tx;
        m_dst_y = ty;

        m_path.clear();
        if (!m_context->m_var.m_board.find_path(
                    { m_src_x, m_src_y },
                    { m_dst_x, m_dst_y },
                    m_path)) {
            m_path.clear();
        }
    }

    void tick(double dt) override
    {
        if (m_usure_phase) {
            // nop
        } else {
            m_time += dt;
            m_time = fmod(m_time, m_context->m_const.bump_period);
        }
    }

    void draw(double) override
    {
        double bmp_factor = double(m_time) / m_context->m_const.bump_period * 3.14;
        double sqz_factor = double(m_time) / m_context->m_const.bump_period * 2.0 * 3.14;

        double x = double(m_src_x) + 0.5;
        double y = double(m_src_y) + 0.5 - sin(bmp_factor) * m_context->m_const.ball_jump_h;

        double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

        m_context->draw_ball(x, y, m_color, m_context->m_const.ball_radius, squeeze, m_context->current_transform());

        if (m_path.size() > 0) {
            auto step = m_path.begin() + 1;
            while (step != m_path.end()) {
                m_context->draw_feet(
                        step->first + 0.5,
                        step->second + 0.5,
                        m_context->current_transform());
                step++;
            }
        }

        if (m_usure_phase) {
            m_context->draw_veil();
        }

        m_score_label->on_draw();
        m_giveup_button->on_draw();
        if (m_usure_phase) {
            m_usure_dialog->on_draw();
        }
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return std::move(m_next_state);
    }

};

std::shared_ptr<dick::StateNode> make_wait_dest_state(
        KulkiContext *context,
        int src_x,
        int src_y)
{
    return std::make_shared<WaitDestState>(context, src_x, src_y);
}
