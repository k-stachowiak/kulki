#include "wait_dest_state.h"
#include "kulki_context.h"

WaitDestState::WaitDestState(KulkiContext* context, int src_x, int src_y) :
    m_context { context },
    m_src_x { src_x },
    m_src_y { src_y },
    m_color { context->m_var.m_board(src_x, src_y) }
{
    m_context->m_var.m_board(src_x, src_y) = m_context->m_const.empty_field;
}

void WaitDestState::on_key(int key, bool down)
{
    if (down && key == ALLEGRO_KEY_ESCAPE) {
        t_transition_required = true;
        m_context->m_var.m_board(m_src_x, m_src_y) = m_color;
        m_next_state = std::shared_ptr<StateNode> {
            new WaitBallState { m_context }
        };
    }
}

void WaitDestState::on_button(int button, bool down)
{
    if (!down) {
        return;
    }

    int tx = m_context->m_var.m_cursor_tile.first;
    int ty = m_context->m_var.m_cursor_tile.second;

    if (m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {

        // Restore current ball
        m_context->m_var.m_board(m_src_x, m_src_y) = m_color;

        // Remember new ball
        m_src_x = tx;
        m_src_y = ty;
        m_color = m_context->m_var.m_board(tx, ty);

        // Remove new ball from board
        m_context->m_var.m_board(m_src_x, m_src_y) = m_context->m_const.empty_field;
        return;
    }

    if (tx == m_src_x && ty == m_src_y) {
        m_context->m_var.m_board(m_src_x, m_src_y) = m_color;
        t_transition_required = true;
        m_next_state.reset(new WaitBallState { m_context });

    } else {
        std::deque<std::pair<int, int>> path;
        if (m_context->m_var.m_board.find_path({ m_src_x, m_src_y }, { tx, ty }, path)) {
            t_transition_required = true;
            m_next_state.reset(new MoveState {
                m_context,
                std::move(path),
                m_context->m_const.move_period,
                tx, ty,
                m_color
            });
        }
    }
}

void WaitDestState::tick(double dt)
{
    m_time += dt;
    m_time = fmod(m_time, m_context->m_const.bump_period);
}

void WaitDestState::draw(double)
{
    double bmp_factor = double(m_time) / m_context->m_const.bump_period * 3.14;
    double sqz_factor = double(m_time) / m_context->m_const.bump_period * 2.0 * 3.14;

    double x = double(m_src_x) + 0.5;
    double y = double(m_src_y) + 0.5 - sin(bmp_factor) * m_context->m_const.ball_jump_h;

    double squeeze = -cos(sqz_factor - 0.75 * 3.14) * 0.1 + 0.9;

    m_context->draw_ball(x, y, m_color, m_context->m_const.ball_radius, squeeze, m_context->current_transform());
}

std::shared_ptr<dick::StateNode> WaitDestState::next_state()
{
    return std::move(m_next_state);
}

