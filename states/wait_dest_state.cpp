#include "wait_dest_state.h"
#include "kulki_context.h"

WaitDestState::WaitDestState(KulkiContext* context) :
    m_context { context }
{}

void WaitDestState::reset(int src_x, int src_y, int color, double time)
{
    m_src_x = src_x;
    m_src_y = src_y;
    m_color = color;
    m_time = time;
}

void WaitDestState::tick(double dt)
{
    m_time += dt;
    m_time = fmod(m_time, m_context->m_constants.bump_period);
}

void WaitDestState::draw(double)
{
    double bmp_factor = double(m_time) / m_context->m_constants.bump_period * 3.14;
    double sqz_factor = double(m_time) / m_context->m_constants.bump_period * 2.0 * 3.14;

    double x = double(m_src_x) + 0.5;
    double y = double(m_src_y) + 0.5 - sin(bmp_factor) * m_context->m_constants.ball_jump_h;

    double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

    m_context->draw_ball(x, y, m_color, m_context->m_constants.ball_radius, squeeze, m_context->m_current_transform());
}

void WaitDestState::on_key(int key, bool down)
{
    if (down && key == ALLEGRO_KEY_ESCAPE) {
        m_context->reset_state_wait_ball(m_src_x, m_src_y, m_color);
    }
}

void WaitDestState::on_button(int button, bool down)
{
    if (!down) {
        return;
    }

    int tx = m_context->m_cursor_tile.first;
    int ty = m_context->m_cursor_tile.second;

    if ((tx == m_src_x && ty == m_src_y) || !m_context->m_board.has(tx, ty)) {
        m_context->reset_state_wait_ball(m_src_x, m_src_y, m_color);
    } else if (m_context->m_board(tx, ty) != m_context->m_constants.empty_field) {
        m_context->reset_state_wait_dest(tx, ty);
    } else {
        m_context->set_state_move(m_src_x, m_src_y, tx, ty, m_color);
    }
}

