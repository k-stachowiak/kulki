#include "wait_ball_state.h"
#include "kulki_context.h"

WaitBallState::WaitBallState(KulkiContext* context) :
    m_context { context },
    m_usure_phase { false }
{}

void WaitBallState::on_key(int key, bool down)
{
    if (!down) {
        return;
    }

    switch (key) {
    case ALLEGRO_KEY_ESCAPE:
        if (m_usure_phase) {
            m_usure_phase = false;
        } else {
            m_usure_phase = true;
        }
        break;

    case ALLEGRO_KEY_Y:
        t_transition_required = true;
        m_next_state = std::shared_ptr<StateNode> { new GameoverState { m_context } };
        break;

    case ALLEGRO_KEY_N:
        if (m_usure_phase) {
            m_usure_phase = false;
        }
        break;

    default:
        break;
    }
}

void WaitBallState::on_button(int button, bool down)
{
    if (!down) {
        return;
    }
    int tx = m_context->m_var.m_cursor_tile.first;
    int ty = m_context->m_var.m_cursor_tile.second;
    if (m_context->m_var.m_board.has(tx, ty) && m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {
        t_transition_required = true;
        m_next_state.reset(new WaitDestState { m_context, tx, ty });
    }
}


void WaitBallState::draw(double)
{
    if (m_usure_phase) {
        al_draw_text(
            m_context->m_const.menu_font, al_map_rgb_f(1, 1, 1),
            m_context->m_const.screen_w / 2.0,
            m_context->m_const.screen_h / 2.0,
            ALLEGRO_ALIGN_CENTRE,
            "Are you sure? [y/n]");
    }
}

std::shared_ptr<dick::StateNode> WaitBallState::next_state()
{
    return std::move(m_next_state);
}
