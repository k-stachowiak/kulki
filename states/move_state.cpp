// Copyright (C) 2015 Krzysztof Stachowiak

#include <deque>

#include "dick.h"
#include "kulki_context.h"

class MoveState : public dick::StateNode {

    KulkiContext* const m_context;

    bool m_usure_phase;
    std::unique_ptr<dick::GUI::Widget> m_score_label;
    std::unique_ptr<dick::GUI::Widget> m_usure_dialog;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;

    std::shared_ptr<StateNode> m_next_state;
    std::deque<std::pair<int, int>> m_path;
    double m_time;
    int m_dst_x, m_dst_y;
    int m_color;

public:
    MoveState(
            KulkiContext* context,
            std::deque<std::pair<int, int>> path,
            double time, int dst_x, int dst_y, int color) :
        m_context { context },
        m_usure_phase { false },
        m_path { std::move(path) },
        m_time { time },
        m_dst_x { dst_x },
        m_dst_y { dst_y },
        m_color { color }
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
    }

    void tick(double dt) override
    {
        if (m_usure_phase) {
            // nop
        } else {
            if ((m_time -= dt) > 0) return;
            else m_time = m_context->m_const.move_period;

            m_path.pop_front();

            if (m_path.size() == 1) {
                m_context->m_var.m_board(m_dst_x, m_dst_y) = m_color;
                m_next_state = make_score_state(
                    m_context,
                    std::vector<std::pair<int, int>> { { m_dst_x, m_dst_y } },
                    true);
                t_transition_required = true;
            }
        }
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

        m_giveup_button->on_click(button);
        if (m_usure_phase) {
            m_usure_dialog->on_click(button);
        }
    }

    void draw(double) override
    {
        double x1 = m_path[0].first;
        double y1 = m_path[0].second;
        double x2 = m_path[1].first;
        double y2 = m_path[1].second;

        double mv_factor = double(m_time) / m_context->m_const.move_period;
        double bmp_factor = double(m_time) / m_context->m_const.move_period * 3.14;
        double sqz_factor = double(m_time) / m_context->m_const.move_period * 2.0 * 3.14;

        double x = x1 * mv_factor + x2 * (1.0 - mv_factor) + 0.5;
        double y = y1 * mv_factor + y2 * (1.0 - mv_factor) + 0.5 - sin(bmp_factor) * m_context->m_const.ball_jump_h;

        double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

        m_context->draw_ball(x, y, m_color, m_context->m_const.ball_radius, squeeze, m_context->current_transform());

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
        return m_next_state;
    }

};

std::shared_ptr<dick::StateNode> make_move_state(
        KulkiContext* context,
        std::deque<std::pair<int, int>> path,
        double time, int dst_x, int dst_y, int color)
{
    return std::make_shared<MoveState>(context, path, time, dst_x, dst_y, color);
}
