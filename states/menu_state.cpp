// Copyright (C) 2015 Krzysztof Stachowiak

#include <allegro5/allegro_primitives.h>
#include <vector>
#include <string>

#include "dick.h"
#include "kulki_context.h"

class MenuState : public dick::StateNode {

    KulkiContext* m_context;

    const std::vector<std::pair<std::string, dick::GUI::Callback>> m_entries;
    double m_width, m_height;

    std::unique_ptr<dick::GUI::WidgetContainer> m_button_rail;

    std::shared_ptr<StateNode> m_next_state;

    std::pair<double, double> m_compute_dimensions()
    {
        const double height = al_get_font_line_height(m_context->m_const.menu_font);
        const int n = m_entries.size();

        std::vector<double> widths;
        std::transform(
            begin(m_entries), end(m_entries), std::back_inserter(widths),
            [this](const auto& entry)
            {
                return al_get_text_width(m_context->m_const.menu_font, entry.first.c_str());
            });

        const double max_width = *std::max_element(begin(widths), end(widths));

        return {
            2.0 * (m_context->m_const.menu_padding + m_context->m_const.menu_margin) + max_width,
            n * (height + 2.0 * m_context->m_const.menu_padding + m_context->m_const.menu_margin) +
                m_context->m_const.menu_margin
        };
    }

    void m_on_new_game()
    {
        m_context->m_var.m_board.clear();
        m_context->m_var.m_score = 0;
        m_context->m_var.gen_next_deal(m_context->m_const.deal_count_init);
        t_transition_required = true;
        m_next_state = make_deal_state(m_context, m_context->m_const.deal_period);
    }

    void m_on_high_score()
    {
        m_context->m_var.m_score = -1;
        t_transition_required = true;
        m_next_state = make_highscore_state(m_context, -1);
    }

    void m_on_exit()
    {
        t_is_over = true;
    }

public:
    MenuState(KulkiContext* context) :
        m_context { context },
        m_entries {
            { "New game", std::bind(&MenuState::m_on_new_game, this) },
            { "High score", std::bind(&MenuState::m_on_high_score, this) },
            { "Exit", std::bind(&MenuState::m_on_exit, this) }
        }
    {
        std::tie(m_width, m_height) = m_compute_dimensions();

        const double text_height = al_get_font_line_height(m_context->m_const.menu_font);

        m_button_rail = context->m_gui.make_container_rail(
            dick::GUI::Direction::DOWN,
            3 * text_height,
            {
                context->m_const.screen_w / 2.0,
                context->m_const.screen_h / 2.0
            });

        for (decltype(m_entries)::size_type i = 0; i < m_entries.size(); ++i) {

            const std::string& entry = m_entries[i].first;
            auto callback = m_entries[i].second;

            m_button_rail->insert(
                context->m_gui.make_button_sized(
                    context->m_gui.make_label_ex(entry, context->m_const.menu_font),
                    callback,
                    { m_width, 2 * text_height }
                ),
                dick::GUI::Alignment::BOTTOM | dick::GUI::Alignment::CENTER
            );
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (down) {
            m_button_rail->on_click(button);
        }
    }

    void draw(double) override
    {
        m_button_rail->on_draw();
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return std::move(m_next_state);
    }

};

std::shared_ptr<dick::StateNode> make_menu_state(
        KulkiContext *context)
{
    return std::make_shared<MenuState>(context);
}
