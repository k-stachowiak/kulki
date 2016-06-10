// Copyright (C) 2015 Krzysztof Stachowiak

#include <deque>

#include "dick.h"
#include "kulki_context.h"

class MoveState : public dick::StateNode {

    KulkiContext* const m_context;

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
        m_path { std::move(path) },
        m_time { time },
        m_dst_x { dst_x },
        m_dst_y { dst_y },
        m_color { color }
    {}

    void tick(double dt) override
    {
        if ((m_time -= dt) > 0) return;
        else m_time = m_context->m_const.move_period;

        m_path.pop_front();

        if (m_path.size() == 1) {
            m_context->m_var.m_board(m_dst_x, m_dst_y) = m_color;
            t_transition_required = true;
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
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_score_state(
            m_context,
            std::vector<std::pair<int, int>> { { m_dst_x, m_dst_y } },
            true
        );
    }

};

std::shared_ptr<dick::StateNode> make_move_state(
        KulkiContext* context,
        std::deque<std::pair<int, int>> path,
        double time, int dst_x, int dst_y, int color)
{
    return std::make_shared<MoveState>(context, path, time, dst_x, dst_y, color);
}
