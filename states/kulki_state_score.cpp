#include "kulki_context.h"
#include "score_state.h"

ScoreState::ScoreState(
        KulkiContext* context, ALLEGRO_FONT* score_font,
        double time, double cx, double cy, int incr) :
    m_context { context },
    m_score_font { score_font },
    m_time { time },
    m_cx { cx },
    m_cy { cy },
    m_incr { incr }
{}

void ScoreState::draw(const glm::mat3& transf)
{
    glm::vec3 text_center = glm::vec3 { m_cx, m_cy, 1 } * transf;
    al_draw_textf(
        m_score_font, config::SCORE_COLOR,
        text_center.x, text_center.y,
        ALLEGRO_ALIGN_CENTRE,
        "+%d", m_incr);
}

void ScoreState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    m_context->set_state_wait_ball();
}

