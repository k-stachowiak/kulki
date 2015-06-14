#include "move_state.h"
#include "kulki_context.h"

MoveState::MoveState(KulkiContext* context) :
    m_context { context }
{}

void MoveState::reset(std::deque<std::pair<int, int>> path, double time, int dst_x, int dst_y, int color)
{
    m_path = std::move(path);
    m_time = time;
    m_dst_x = dst_x;
    m_dst_y = dst_y;
    m_color = color;
}

void MoveState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    else m_time = config::MOVE_PERIOD;

    m_path.pop_front();

    if (m_path.size() == 1) {
        m_context->m_board(m_dst_x, m_dst_y) = m_color;
        m_context->set_state_score({ { m_dst_x, m_dst_y } }, true);
    }
}

void MoveState::draw(double)
{
    double x1 = m_path[0].first;
    double y1 = m_path[0].second;
    double x2 = m_path[1].first;
    double y2 = m_path[1].second;

    double mv_factor = double(m_time) / config::MOVE_PERIOD;
    double bmp_factor = double(m_time) / config::MOVE_PERIOD * 3.14;
    double sqz_factor = double(m_time) / config::MOVE_PERIOD * 2.0 * 3.14;

    double x = x1 * mv_factor + x2 * (1.0 - mv_factor) + 0.5;
    double y = y1 * mv_factor + y2 * (1.0 - mv_factor) + 0.5 - sin(bmp_factor) * config::BALL_JUMP_H;

    double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

    m_context->draw_ball(x, y, m_color, config::BALL_RADIUS, squeeze, m_context->m_current_transform());
}

