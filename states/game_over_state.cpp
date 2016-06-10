// Copyright (C) 2015 Krzysztof Stachowiak

#include "dick.h"
#include "kulki_context.h"

class GameoverState : public dick::StateNode {
    KulkiContext* const m_context;
    double m_time;
    int m_index;

public:
    GameoverState(KulkiContext* context) :
        m_context { context },
        m_time { context->m_const.gameover_period },
        m_index { 0 }
    {}

    void on_key(dick::Key, bool down) override
    {
        if (down) {
            t_transition_required = true;
        }
    }

    void on_button(dick::Button, bool down) override
    {
        if (down) {
            t_transition_required = true;
        }
    }

    void tick(double dt) override
    {
        if ((m_time -= dt) > 0) {
            return;
        }

        m_time = m_context->m_const.gameover_period;
        m_index = (m_index + 1) % m_context->m_const.ball_colors.size();
    }

    void draw(double) override
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

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_highscore_state(m_context, m_context->m_var.m_score);
    }

};

std::shared_ptr<dick::StateNode> make_gameover_state(
        KulkiContext *context)
{
    return std::make_shared<GameoverState>(context);
}
