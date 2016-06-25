// Copyright (C) 2015 Krzysztof Stachowiak

#include <vector>
#include <string>

#include "dick.h"
#include "kulki_context.h"

class OptionsState : public dick::StateNode {

    KulkiContext* m_context;
    std::unique_ptr<dick::GUI::WidgetContainer> m_button_rail;
    std::shared_ptr<StateNode> m_next_state;

    void rebuild_button_rail()
    {
        m_button_rail = m_context->m_gui.make_container_rail(
            dick::GUI::Direction::DOWN,
            60.0);

        m_button_rail->insert(
            m_context->make_range_int_spin_ex(
                "Balls", &m_context->m_var.m_ball_count, 3, 8,
                m_context->m_const.menu_font),
            dick::GUI::Alignment::TOP | dick::GUI::Alignment::CENTER);

        m_button_rail->insert(
            m_context->make_range_int_spin_ex(
                "Board width", &m_context->m_var.m_board_width, 5, 9,
                m_context->m_const.menu_font),
            dick::GUI::Alignment::TOP | dick::GUI::Alignment::CENTER);

        m_button_rail->insert(
            m_context->make_range_int_spin_ex(
                "Board height", &m_context->m_var.m_board_height, 5, 9,
                m_context->m_const.menu_font),
            dick::GUI::Alignment::TOP | dick::GUI::Alignment::CENTER);

        m_button_rail->insert(
            m_context->m_gui.make_button(
                m_context->m_gui.make_label_ex("Back", m_context->m_const.menu_font),
                [this]()
                {
                    m_context->m_var.reset_board();
                    t_transition_required = true;
                    m_next_state = make_menu_state(m_context);
                }),
            dick::GUI::Alignment::TOP | dick::GUI::Alignment::CENTER);

        m_button_rail->align(
            {
                m_context->m_const.screen_w / 2.0,
                m_context->m_const.screen_h / 2.0
            },
            dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER);
    }

public:
    OptionsState(KulkiContext* context) :
        m_context { context }
    {
        rebuild_button_rail();
    }

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        if (key == dick::Key::ESCAPE) {
            m_context->m_var.reset_board();
            t_transition_required = true;
            m_next_state = make_menu_state(m_context);
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (down) {
            m_button_rail->on_click(button);
            rebuild_button_rail();
        }
    }

    void draw(double) override
    {
        m_context->draw_veil();
        m_button_rail->on_draw();
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return std::move(m_next_state);
    }

};

std::shared_ptr<dick::StateNode> make_options_state(
        KulkiContext *context)
{
    return std::make_shared<OptionsState>(context);
}
