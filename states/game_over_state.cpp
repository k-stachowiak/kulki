// Copyright (C) 2015 Krzysztof Stachowiak

#include "game_over_state.h"
#include "kulki_context.h"

GameoverState::GameoverState(KulkiContext* context) :
    m_context { context },
    m_time { context->m_const.gameover_period },
    m_index { 0 }
{}

void GameoverState::on_key(dick::Key, bool down)
{
    if (down) {
        t_transition_required = true;
    }
}

void GameoverState::on_button(dick::Button, bool down)
{
    if (down) {
        t_transition_required = true;
    }
}

void GameoverState::tick(double dt)
{
    if ((m_time -= dt) > 0) {
        return;
    }

    m_time = m_context->m_const.gameover_period;
    m_index = (m_index + 1) % m_context->m_const.ball_colors.size();
}

void GameoverState::draw(double)
{
    al_draw_text(
        m_context->m_const.gameover_font,
        al_map_rgb_f(
            m_context->m_const.ball_colors[m_index].r,
            m_context->m_const.ball_colors[m_index].g,
            m_context->m_const.ball_colors[m_index].b
        ),
        m_context->m_const.gameover_shift_x,
        m_context->m_const.gameover_shift_y,
        ALLEGRO_ALIGN_CENTRE,
        "Game Over");
}

std::shared_ptr<dick::StateNode> GameoverState::next_state()
{
    return std::shared_ptr<StateNode> { new HighScoreState { m_context, m_context->m_var.m_score } };
}
