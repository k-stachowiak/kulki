// Copyright (C) 2015 Krzysztof Stachowiak

#include <iostream>
#include <vector>

#include "dick.h"
#include "kulki_context.h"

namespace {
    std::random_device device;
    std::mt19937 engine { device() };
}

class DealState : public dick::StateNode {

    KulkiContext* const m_context;

    bool m_usure_phase;
    std::unique_ptr<dick::GUI::Widget> m_score_label;
    std::unique_ptr<dick::GUI::Widget> m_usure_dialog;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;

    std::shared_ptr<StateNode> m_next_state;

    int m_current_x, m_current_y, m_current_color;
    double m_time;
    std::vector<std::pair<int, int>> m_positions;

    void deal_next()
    {
        std::uniform_int_distribution<int> distr_x(0, m_context->m_var.m_board.m_width - 1);
        std::uniform_int_distribution<int> distr_y(0, m_context->m_var.m_board.m_height - 1);

        int new_color = m_context->m_var.m_next_deal.back();
        m_context->m_var.m_next_deal.pop_back();

        int new_x, new_y;
        do {
            new_x = distr_x(engine);
            new_y = distr_y(engine);
        } while (m_context->m_var.m_board(new_x, new_y) != m_context->m_const.empty_field);

        m_current_x = new_x;
        m_current_y = new_y;
        m_current_color = new_color;
    }

public:
    DealState(KulkiContext* context, double time) :
        m_context { context },
        m_usure_phase { false },
        m_time { time }
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

        deal_next();
    }

    void tick(double dt) override
    {
        if (m_usure_phase) {
            // nop
        } else {
            if ((m_time -= dt) > 0) return;
            else m_time = m_context->m_const.deal_period;

            m_context->m_var.m_board(m_current_x, m_current_y) = m_current_color;
            m_positions.emplace_back(m_current_x, m_current_y);

            if (m_context->m_var.m_next_deal.empty()) {
                m_context->m_var.gen_next_deal(m_context->m_const.deal_count_ingame);
                m_next_state = make_score_state(m_context, m_positions, false);
                t_transition_required = true;
            } else {
                deal_next();
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
        if (m_usure_phase) {
            m_context->draw_veil();
        }
        double squeeze = 1.0 - double(m_time) / m_context->m_const.deal_period;
        double x = double(m_current_x) + 0.5;
        double y = double(m_current_y) + 0.5;
        m_context->draw_ball(
            x, y,
            m_current_color,
            m_context->m_const.ball_radius,
            squeeze,
            m_context->current_transform());

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

std::shared_ptr<dick::StateNode> make_deal_state(
        KulkiContext *context,
        double time)
{
    return std::make_shared<DealState>(context, time);
}
