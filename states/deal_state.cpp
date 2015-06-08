#include "deal_state.h"
#include "kulki_context.h"

namespace {
    std::random_device device;
    std::mt19937 engine { device() };
}

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

    std::uniform_int_distribution<int> distr_x(0, m_context->m_board->m_width - 1);
    std::uniform_int_distribution<int> distr_y(0, m_context->m_board->m_height - 1);

    int new_color = m_context->m_next_deal.back();
    m_context->m_next_deal.pop_back();

    int new_x, new_y;
    do {
        new_x = distr_x(engine);
        new_y = distr_y(engine);
    } while ((*m_context->m_board)(new_x, new_y) != config::EMPTY);

    (*m_context->m_board)(new_x, new_y) = new_color;
    m_positions.emplace_back(new_x, new_y);

    if (m_context->m_next_deal.empty()) {
        m_context->gen_next_deal(config::DEAL_COUNT_INGAME);
        m_context->set_state_score(m_positions, KulkiState::Enum::WAIT_BALL);
    }
}

