#include <allegro5/allegro_primitives.h>

#include "dick.h"
#include "kulki_context.h"
#include "menu_state.h"

std::pair<double, double> MenuState::m_compute_dimensions()
{
    const double height = al_get_font_line_height(m_context->m_constants.menu_font);
    const int n = m_entries.size();

    double max_width = 0.0;
    for (const auto& entry : m_entries) {
        double width = al_get_text_width(m_context->m_constants.menu_font, entry.c_str());
        if (width > max_width) {
            max_width = width;
        }
    }

    return {
        2.0 * (m_context->m_constants.menu_padding + m_context->m_constants.menu_margin) + max_width,
        n * (height + 2.0 * m_context->m_constants.menu_padding + m_context->m_constants.menu_margin) +
            m_context->m_constants.menu_margin
    };
}

void MenuState::m_select()
{
    const std::string& entry = m_entries[m_current];

    if (entry == "New game") {
        m_context->m_board.clear();
        m_context->m_score = 0;
        m_context->gen_next_deal(m_context->m_constants.deal_count_init);
        t_transition_required = true;
        m_next_state.reset(new DealState { m_context, m_context->m_constants.deal_period });

    } else if (entry == "High score") {
        m_context->m_score = -1;
        t_transition_required = true;
        m_next_state.reset(new HighScoreState { m_context, -1 });

    } else if (entry == "Exit") {
        t_is_over = true;

    } else {
        throw std::runtime_error("Incogerent menu configuration");

    }
}

MenuState::MenuState(KulkiContext* const context) :
    m_context { context },
    m_entries { "New game", "High score", "Exit" },
    m_current { 0 }
{
    std::tie(m_width, m_height) = m_compute_dimensions();
}

void MenuState::on_button(int button, bool down)
{
    if (button == 1 && down) {
        m_select();
    }
}

void MenuState::draw(double)
{
    const double height = al_get_font_line_height(m_context->m_constants.menu_font);
    const double x = double(m_context->m_constants.screen_w - m_width) / 2.0;
    double y = double(m_context->m_constants.screen_h - m_height) / 2.0;

    for (decltype(m_entries)::size_type i = 0; i < m_entries.size(); ++i) {

        const std::string& entry = m_entries[i];

        const double x1 = x + m_context->m_constants.menu_margin;
        const double y1 = y + m_context->m_constants.menu_margin;
        const double x2 = x + m_width - m_context->m_constants.menu_margin;
        const double y2 = y + m_context->m_constants.menu_margin +
            height + 2.0 * m_context->m_constants.menu_padding;
        al_draw_filled_rectangle(x1, y1, x2, y2, m_context->m_constants.menu_bg_color_l);
        al_draw_rectangle(x1, y1, x2, y2, m_context->m_constants.menu_bg_color_d, 3);

        const double cx = x + m_width / 2.0;
        const double cy = y + m_context->m_constants.menu_margin +
            m_context->m_constants.menu_padding + height / 2.0;
        const auto color = (i == m_current)
            ? m_context->m_constants.menu_select_color
            : m_context->m_constants.menu_regular_color;
        al_draw_textf(
                m_context->m_constants.menu_font,
                color,
                cx, cy - height / 2,
                ALLEGRO_ALIGN_CENTRE,
                "%s", entry.c_str());

        y += height + 2.0 * m_context->m_constants.menu_padding + m_context->m_constants.menu_margin;

        if (m_context->m_cursor_screen.first > x1 && m_context->m_cursor_screen.first < x2 &&
            m_context->m_cursor_screen.second > y1 && m_context->m_cursor_screen.second < y2) {
            m_current = i;
        }
    }
}

std::shared_ptr<dick::StateNode> MenuState::next_state()
{
    return std::move(m_next_state);
}

