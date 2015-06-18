// Copyright (C) 2015 Krzysztof Stachowiak

#include "game_over_state.h"
#include "kulki_context.h"

GameoverState::GameoverState(KulkiContext* context) :
    m_context { context },
    m_period { m_context->m_config.get_real("GAMEOVER_PERIOD") },
    m_gameover_shift_x { m_context->m_config.get_integer("GAMEOVER_SHIFT_X") },
    m_gameover_shift_y { m_context->m_config.get_integer("GAMEOVER_SHIFT_Y") }
{}

void GameoverState::reset(double time, int index)
{
    m_time = time;
    m_index = index;
}

void GameoverState::tick(double dt)
{
    if ((m_time -= dt) > 0) {
        return;
    }

    m_time = m_period;
    m_index = (m_index + 1) % m_context->m_ball_colors.size();
}

void GameoverState::draw(double)
{
    al_draw_text(
        m_context->m_gameover_font, m_context->m_ball_colors[m_index],
        m_gameover_shift_x, m_gameover_shift_y,
        ALLEGRO_ALIGN_CENTRE,
        "Game Over");
}

void GameoverState::on_key(int key, bool down)
{
    if (down) {
        m_context->set_state_high_score();
    }
}
