#include "move_state.h"
#include "kulki_context.h"

MoveState::MoveState(Board* board, KulkiContext* context) :
    m_board { board },
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
        (*m_board)(m_dst_x, m_dst_y) = m_color;
        m_context->set_state_score({ { m_dst_x, m_dst_y } }, KulkiState::Enum::DEAL);
    }
}

void MoveState::draw(const glm::mat3& transf)
{
    double x1 = m_path[0].first;
    double y1 = m_path[0].second;
    double x2 = m_path[1].first;
    double y2 = m_path[1].second;

    double mv_factor = m_time / config::MOVE_PERIOD;
    double bmp_factor = (1.0 - mv_factor) * 3.14;

    double x = x1 * mv_factor + x2 * (1.0 - mv_factor) + 0.5;
    double y = y1 * mv_factor + y2 * (1.0 - mv_factor) + 0.5 - sin(bmp_factor) * config::BALL_JUMP_H;

    draw_ball(x, y, m_color, config::BALL_RADIUS, transf);
}

