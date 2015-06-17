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
        m_context->set_state_gameover();
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
    int tx = m_context->m_cursor_tile.first;
    int ty = m_context->m_cursor_tile.second;
    if (m_context->m_board.has(tx, ty) && m_context->m_board(tx, ty) != config::EMPTY) {
        m_context->set_state_wait_dest(tx, ty);
    }
}


void WaitBallState::draw(double)
{
    if (m_usure_phase) {
        al_draw_text(
            m_context->m_menu_font, al_map_rgb_f(1, 1, 1),
            config::SCREEN_W / 2.0,
            config::SCREEN_H / 2.0,
            ALLEGRO_ALIGN_CENTRE,
            "Are you sure? [y/n]");
    }
}

