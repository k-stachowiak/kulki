// Copyright (C) 2015 Krzysztof Stachowiak

#include "dick.h"
#include "kulki_context.h"

class WaitBallState : public dick::StateNode {

    KulkiContext* const m_context;
    bool m_usure_phase;
    std::unique_ptr<dick::GUI::Widget> m_score_label;
    std::unique_ptr<dick::GUI::Widget> m_usure_dialog;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;
    std::shared_ptr<StateNode> m_next_state;

public:
    WaitBallState(KulkiContext* context) :
        m_context { context },
        m_usure_phase { false }
    {
        m_score_label = m_context->make_score_label();
        m_giveup_button = m_context->make_giveup_button(
            [this]()
            {
                m_usure_phase = true;
            });

        m_usure_dialog = m_context->make_giveup_dialog(
            [this]()
            {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            },
            [this]()
            {
                m_usure_phase = false;
            });
    }

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        if (m_usure_phase) {
            if (key == dick::Key::ENTER) {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            } else if (key == dick::Key::ESCAPE) {
                m_usure_phase = false;
            }
        } else {
            if (key == dick::Key::ESCAPE) {
                m_usure_phase = true;
            }
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (!down) {
            return;
        }

        if (m_usure_phase) {
            m_usure_dialog->on_click(button);
        } else {
            m_giveup_button->on_click(button);
            int tx = m_context->m_var.m_cursor_tile.first;
            int ty = m_context->m_var.m_cursor_tile.second;
            if (!m_context->m_var.m_board.has(tx, ty)) {
                return;
            }
            if (m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {
                    t_transition_required = true;
                    m_next_state = make_wait_dest_state(m_context, tx, ty);
            }
        }
    }

    void draw(double) override
    {
        if (m_usure_phase) {
            m_context->draw_veil();
        }

        m_score_label->on_draw();
        m_giveup_button->on_draw();

        if (m_usure_phase) {
            m_usure_dialog->on_draw();
        }
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return std::move(m_next_state);
    }
};

std::shared_ptr<dick::StateNode> make_wait_ball_state(
        KulkiContext *context)
{
    return std::make_shared<WaitBallState>(context);
}
