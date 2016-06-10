// Copyright (C) 2015 Krzysztof Stachowiak

#include "dick.h"
#include "kulki_context.h"

class WaitBallState : public dick::StateNode {

    KulkiContext* const m_context;
    bool m_usure_phase;
    std::unique_ptr<dick::GUI::WidgetContainer> m_usure_rail;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;
    std::shared_ptr<StateNode> m_next_state;

public:
    WaitBallState(KulkiContext* context) :
        m_context { context },
        m_usure_phase { false }
    {
        m_giveup_button = m_context->m_gui.make_button(
            m_context->m_gui.make_label("Give up"),
            [this](){ m_usure_phase = true; }
        );

        m_giveup_button->set_offset(
            dick::GUI::Widget::align(
                { static_cast<double>(context->m_const.screen_w), 0.0 },
                m_giveup_button->get_size(),
                dick::GUI::Alignment::TOP | dick::GUI::Alignment::RIGHT
            )
        );

        auto yes_no_rail = m_context->m_gui.make_container_rail(
            dick::GUI::Direction::RIGHT,
            30
        );

        yes_no_rail->insert(
            m_context->m_gui.make_button_sized(
                m_context->m_gui.make_label("Yes"),
                [this](){
                    t_transition_required = true;
                    m_next_state = make_gameover_state(m_context);
                },
                { 200, 75 }
            )
        );

        yes_no_rail->insert(
            m_context->m_gui.make_button_sized(
                m_context->m_gui.make_label("No"),
                [this](){
                    m_usure_phase = false;
                },
                { 200, 75 }
            )
        );

        m_usure_rail = m_context->m_gui.make_container_rail(
            dick::GUI::Direction::DOWN,
            30,
            {
                context->m_const.screen_w / 2.0,
                context->m_const.screen_h / 2.0
            }
        );

        m_usure_rail->insert(
            m_context->m_gui.make_label_ex(
                "Are you sure?",
                m_context->m_const.score_font
            ),
            dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER
        );

        m_usure_rail->insert(
            std::move(yes_no_rail),
            dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER
        );
    }

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        switch (key) {
        case dick::Key::ESCAPE:
            m_usure_phase = true;
            break;

        default:
            break;
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (down) {
            m_usure_rail->on_click(button);
            m_giveup_button->on_click(button);
        }

        if (!down || t_transition_required) {
            return;
        }
        int tx = m_context->m_var.m_cursor_tile.first;
        int ty = m_context->m_var.m_cursor_tile.second;
        if (m_context->m_var.m_board.has(tx, ty) && m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {
            t_transition_required = true;
            m_next_state = make_wait_dest_state(m_context, tx, ty);
        }
    }

    void draw(double) override
    {
        if (m_usure_phase) {
            m_usure_rail->draw();
        } else {
            m_giveup_button->draw();
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
