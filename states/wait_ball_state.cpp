#include "wait_ball_state.h"
#include "kulki_context.h"

WaitBallState::WaitBallState(Board* board, KulkiContext* context, std::pair<int, int> *cursor_tile) :
    m_board { board },
    m_context { context },
    m_cursor_tile { cursor_tile }
{}

void WaitBallState::on_button(int button, bool down)
{
    int tx = m_cursor_tile->first;
    int ty = m_cursor_tile->second;
    if (m_board->has(tx, ty) && (*m_board)(tx, ty) != config::EMPTY) {
        m_context->set_state_wait_dest(tx, ty);
    }
}

