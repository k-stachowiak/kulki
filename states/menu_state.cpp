// Copyright (C) 2015 Krzysztof Stachowiak

#include <allegro5/allegro_primitives.h>

#include "dick.h"
#include "kulki_context.h"
#include "menu_state.h"

std::pair<double, double> MenuState::m_compute_dimensions()
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

void MenuState::m_on_new_game()
{
    m_context->m_var.m_board.clear();
    m_context->m_var.m_score = 0;
    m_context->m_var.gen_next_deal(m_context->m_const.deal_count_init);
    t_transition_required = true;
    m_next_state.reset(new DealState { m_context, m_context->m_const.deal_period });
}

void MenuState::m_on_high_score()
{
    m_context->m_var.m_score = -1;
    t_transition_required = true;
    m_next_state.reset(new HighScoreState { m_context, -1 });
}

void MenuState::m_on_exit()
{
    t_is_over = true;
}

MenuState::MenuState(KulkiContext* const context) :
    m_context { context },
    m_entries {
        { "New game", std::bind(&MenuState::m_on_new_game, this) },
        { "High score", std::bind(&MenuState::m_on_high_score, this) },
        { "Exit", std::bind(&MenuState::m_on_exit, this) }
    }
{
    std::tie(m_width, m_height) = m_compute_dimensions();
}

void MenuState::draw(double)
{
    const double text_height = al_get_font_line_height(m_context->m_const.menu_font);

    ALLEGRO_BITMAP *target = al_get_target_bitmap();
    al_draw_filled_rectangle(0, 0,
            al_get_bitmap_width(target),
            al_get_bitmap_height(target),
            al_map_rgba_f(0, 0, 0, 0.333));

    m_context->m_gui.set_current_font(m_context->m_const.menu_font);
    m_context->m_gui.set_current_widget_alignment(
            dick::GUI::Alignment::CENTER |
            dick::GUI::Alignment::TOP);

    m_context->m_gui.transform_reset();
    m_context->m_gui.transform_push_screen_align(
            dick::GUI::Alignment::CENTER |
            dick::GUI::Alignment::MIDDLE);
    m_context->m_gui.transform_push_box_align(
            dick::GUI::Alignment::MIDDLE,
            { m_width, m_height });

    for (decltype(m_entries)::size_type i = 0; i < m_entries.size(); ++i) {

        const std::string& entry = m_entries[i].first;
        auto callback = m_entries[i].second;

        m_context->m_gui.button_text_sized(
                { m_width, 2 * text_height },
                callback,
                entry.c_str());

        m_context->m_gui.transform_push_shift({ 0,
            text_height +
                2.0 * m_context->m_const.menu_padding +
                m_context->m_const.menu_margin });
    }
}

std::shared_ptr<dick::StateNode> MenuState::next_state()
{
    return std::move(m_next_state);
}

