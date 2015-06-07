#include "deal_state.h"
#include "kulki_context.h"

DealState::DealState(KulkiContext* context) :
    m_context { context }
{}

void DealState::reset(double time)
{
    m_time = time;
    m_positions.clear();
}

void DealState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    else m_time = config::DEAL_PERIOD;

    auto new_ball = m_context->m_next_deal.back();
    m_context->m_next_deal.pop_back();

    int x = std::get<0>(new_ball);
    int y = std::get<1>(new_ball);
    int color = std::get<2>(new_ball);

    (*m_context->m_board)(x, y) = color;
    m_positions.emplace_back(x, y);

    if (m_context->m_next_deal.empty()) {
        m_context->gen_next_deal(config::DEAL_COUNT_INGAME);
        m_context->set_state_score(m_positions, KulkiState::Enum::WAIT_BALL);
    }
}

