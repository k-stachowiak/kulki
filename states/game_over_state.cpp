// Copyright (C) 2015 Krzysztof Stachowiak

#include "game_over_state.h"
#include "kulki_context.h"

GameoverState::GameoverState(KulkiContext* context, ALLEGRO_FONT* gameover_font) :
    m_context { context },
    m_gameover_font { gameover_font }
{}

void GameoverState::reset(int *score, double time, int index)
{
    m_score = score;
    m_time = time;
    m_index = index;
}

void GameoverState::tick(double dt)
{
    if ((m_time -= dt) > 0) {
        return;
    }

    m_time = config::GAMEOVER_PERIOD;
    m_index = (m_index + 1) % config::BALL_COLORS.size();
}

void GameoverState::draw(const glm::mat3& transf)
{
    al_draw_text(
        m_gameover_font, config::BALL_COLORS[m_index],
        config::GAMEOVER_SHIFT_X, config::GAMEOVER_SHIFT_Y,
        ALLEGRO_ALIGN_CENTRE,
        "Game Over");
}

void GameoverState::on_key(int key, bool down)
{
    if (down) {
        m_context->set_state_menu();
    }
}
