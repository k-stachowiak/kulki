#include "wait_ball_state.h"
#include "kulki_context.h"

WaitBallState::WaitBallState(KulkiContext* context) :
    m_context { context }
{}

void WaitBallState::on_key(int key, bool down)
{
    if (down && key == ALLEGRO_KEY_ESCAPE) {
        m_context->set_state_high_score();
    }
}

void WaitBallState::on_button(int button, bool down)
{
    int tx = m_context->m_cursor_tile->first;
    int ty = m_context->m_cursor_tile->second;
    if (m_context->m_board->has(tx, ty) && (*m_context->m_board)(tx, ty) != config::EMPTY) {
        m_context->set_state_wait_dest(tx, ty);
    }
}

