#include "deal_state.h"
#include "kulki_context.h"

void DealState::m_new_ball(Board& b, int& x, int& y, int& color)
{
    std::uniform_int_distribution<int> distr_x(0, b.m_width - 1);
    std::uniform_int_distribution<int> distr_y(0, b.m_height - 1);
    std::uniform_int_distribution<int> distr_color(0, config::BALL_COLORS.size() - 1);

    do {
        x = distr_x(m_reng);
        y = distr_y(m_reng);
    } while (b(x, y) != config::EMPTY);

    color = distr_color(m_reng);
}

DealState::DealState(Board* board, KulkiContext* context, double time, int count) :
    m_reng { m_rdev() },
    m_board { board },
    m_context { context },
    m_time { time },
    m_count { count }
{}

void DealState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    else m_time = config::DEAL_PERIOD;

    int x, y, color;
    m_new_ball(*m_board, x, y, color);
    (*m_board)(x, y) = color;
    m_positions.emplace_back(x, y);

    if ((--m_count) == 0) {
        m_context->set_state_score(m_positions, KulkiState::Enum::WAIT_BALL);
    }
}

