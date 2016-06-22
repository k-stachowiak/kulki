// Copyright (C) 2015 Krzysztof Stachowiak

#include "dick.h"
#include "high_score.h"
#include "kulki_context.h"

class GameoverState : public dick::StateNode {
    KulkiContext* const m_context;
    std::unique_ptr<dick::GUI::Widget> m_message;

public:
    GameoverState(KulkiContext* context) :
        m_context { context }
    {
        m_message = m_context->m_gui.make_dialog_ok(
            "Game over",
            [this](){ t_transition_required = true; });
        m_message->align(
            {
                m_context->m_const.screen_w / 2.0,
                m_context->m_const.screen_h / 2.0
            },
            dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER);
    }

    void on_button(dick::Button button, bool down) override
    {
        if (down) {
            m_message->on_click(button);
        }
    }

    void draw(double) override
    {
        m_context->draw_veil();
        m_message->on_draw();
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        HighScore hs = HighScore::load("high_score", m_context->m_const.highscore_max_entries);
        if (hs.can_insert(m_context->m_var.m_ball_count, m_context->m_var.m_score)) {
            return make_highscore_input_state(m_context);
        } else {
            return make_highscore_display_state(m_context);
        }
    }

};

std::shared_ptr<dick::StateNode> make_gameover_state(
        KulkiContext *context)
{
    return std::make_shared<GameoverState>(context);
}
