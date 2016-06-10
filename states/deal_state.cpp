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
        m_time { time }
    {
        deal_next();
    }

    void tick(double dt) override
    {
        if ((m_time -= dt) > 0) return;
        else m_time = m_context->m_const.deal_period;

        m_context->m_var.m_board(m_current_x, m_current_y) = m_current_color;
        m_positions.emplace_back(m_current_x, m_current_y);

        if (m_context->m_var.m_next_deal.empty()) {
            m_context->m_var.gen_next_deal(m_context->m_const.deal_count_ingame);
            t_transition_required = true;
        } else {
            deal_next();
        }
    }

    void draw(double) override
    {
        double squeeze = 1.0 - double(m_time) / m_context->m_const.deal_period;
        double x = double(m_current_x) + 0.5;
        double y = double(m_current_y) + 0.5;
        m_context->draw_ball(
            x, y,
            m_current_color,
            m_context->m_const.ball_radius,
            squeeze,
            m_context->current_transform());
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_score_state(m_context, m_positions, false);
    }

};

std::shared_ptr<dick::StateNode> make_deal_state(
        KulkiContext *context,
        double time)
{
    return std::make_shared<DealState>(context, time);
}
