#include "kulki_context.h"
#include "score_state.h"

ScoreState::ScoreState(KulkiContext* context) :
    m_context { context },
    m_period { m_context->m_config.get_real("SCORE_PERIOD") },
    m_score_color(m_context->m_config.get_color("SCORE_COLOR"))
{}

void ScoreState::reset(const std::vector<std::pair<int, int>>& changes, bool next_deal)
{
    m_next_deal = next_deal;
    std::unordered_set<std::pair<int, int>> scored;
    bool success = false;

    for (const auto& change : changes) {
        success |= m_context->m_board.find_streak(
                change,
                std::inserter(scored, begin(scored)),
                m_context->m_config.get_integer("SERIE_MIN"));
    }

    if (!success) {
        m_context->m_streak = 0;
        m_time = 0;
        return;
    }

    ++m_context->m_streak;
    m_time = m_period;

    int x_sum = 0, y_sum = 0;
    for (const auto& p : scored) {
        m_context->m_board(p.first, p.second) = m_context->m_empty_field;
        x_sum += p.first;
        y_sum += p.second;
    }

    int score_incr = scored.size() * m_context->m_streak;
    m_context->m_score += score_incr;

    // Setup state for further phase.
    m_next_deal = false;
    m_cx = double(x_sum) / double(scored.size()) + 0.5;
    m_cy = double(y_sum) / double(scored.size()) + 0.5;
    m_incr = score_incr;
}

void ScoreState::draw(double)
{
    if (m_time <= 0) return;

    glm::vec3 text_center = glm::vec3 { m_cx, m_cy, 1 } * m_context->m_current_transform();
    al_draw_textf(
        m_context->m_score_font, m_score_color,
        text_center.x, text_center.y,
        ALLEGRO_ALIGN_CENTRE,
        "+%d", m_incr);
}

void ScoreState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    if (m_next_deal) {
        m_context->set_state_deal();
    } else {
        m_context->set_state_wait_ball();
    }
}

