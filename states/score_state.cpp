#include "kulki_context.h"
#include "score_state.h"

ScoreState::ScoreState(KulkiContext* context, const std::vector<std::pair<int, int>>& changes, bool next_deal) :
    m_context { context },
    m_next_deal { next_deal }
{
    std::unordered_set<std::pair<int, int>> scored;
    bool success = false;

    for (const auto& change : changes) {
        success |= m_context->m_var.m_board.find_streak(
                change,
                std::inserter(scored, begin(scored)),
                m_context->m_const.score_serie_min);
    }

    if (!success) {
        m_context->m_var.m_streak = 0;
        m_time = 0;
        return;
    }

    ++m_context->m_var.m_streak;
    m_time = m_context->m_const.score_period;

    int x_sum = 0, y_sum = 0;
    for (const auto& p : scored) {
        m_context->m_var.m_board(p.first, p.second) = m_context->m_const.empty_field;
        x_sum += p.first;
        y_sum += p.second;
    }

    int score_incr = scored.size() * m_context->m_var.m_streak;
    m_context->m_var.m_score += score_incr;

    // Setup state for further phase.
    m_next_deal = false;
    m_cx = double(x_sum) / double(scored.size()) + 0.5;
    m_cy = double(y_sum) / double(scored.size()) + 0.5;
    m_incr = score_incr;
}

void ScoreState::draw(double)
{
    if (m_time <= 0) return;

    glm::vec3 text_center = glm::vec3 { m_cx, m_cy, 1 } * m_context->current_transform();
    al_draw_textf(
        m_context->m_const.score_font, m_context->m_const.score_color,
        text_center.x, text_center.y,
        ALLEGRO_ALIGN_CENTRE,
        "+%d", m_incr);
}

void ScoreState::tick(double dt)
{
    if ((m_time -= dt) > 0)
        return;

    if (m_next_deal) {
        t_transition_required = true;
        if (m_context->m_var.m_board.free_fields() < m_context->m_const.deal_count_ingame) {
            m_next_state.reset(new GameoverState { m_context });
        } else {
            m_context->gen_next_deal(m_context->m_const.deal_count_ingame);
            m_next_state.reset(new DealState { m_context, m_context->m_const.deal_period });
        }

    } else {
        t_transition_required = true;
        if (m_context->m_var.m_board.free_fields() == 0)
            m_next_state.reset(new GameoverState { m_context });
        else
            m_next_state.reset(new WaitBallState { m_context });

    }
}

std::shared_ptr<dick::StateNode> ScoreState::next_state()
{
    return std::move(m_next_state);
}
