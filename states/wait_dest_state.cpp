#include "wait_dest_state.h"
#include "kulki_context.h"

WaitDestState::WaitDestState(Board* board, KulkiContext* context, std::pair<int, int>* cursor_tile) :
    m_board { board },
    m_context { context },
    m_cursor_tile { cursor_tile }
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
    m_time = fmod(m_time, config::BUMP_PERIOD);
}

void WaitDestState::draw(const glm::mat3& transf)
{
    double bmp_factor = double(m_time) / config::BUMP_PERIOD * 3.14;
    double sqz_factor = double(m_time) / config::BUMP_PERIOD * 2.0 * 3.14;

    double x = double(m_src_x) + 0.5;
    double y = double(m_src_y) + 0.5 - sin(bmp_factor) * config::BALL_JUMP_H;

    double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

    m_context->draw_ball(x, y, m_color, config::BALL_RADIUS, squeeze, transf);
}

void WaitDestState::on_button(int button, bool down)
{
    int tx = m_cursor_tile->first;
    int ty = m_cursor_tile->second;

    if ((tx == m_src_x && ty == m_src_y) || !m_board->has(tx, ty)) {
        m_context->reset_state_wait_ball(m_src_x, m_src_y, m_color);
    } else if ((*m_board)(tx, ty) != config::EMPTY) {
        m_context->reset_state_wait_dest(tx, ty);
    } else {
        m_context->set_state_move(m_src_x, m_src_y, tx, ty, m_color);
    }
}

