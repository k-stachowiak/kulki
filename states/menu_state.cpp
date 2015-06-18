#include <allegro5/allegro_primitives.h>

#include "state_node.h"
#include "kulki_context.h"
#include "menu_state.h"

std::pair<double, double> MenuState::m_compute_dimensions()
{
    const double height = al_get_font_line_height(m_context->m_menu_font);
    const int n = m_entries.size();

    double max_width = 0.0;
    for (const auto& entry : m_entries) {
        double width = al_get_text_width(m_context->m_menu_font, entry.c_str());
        if (width > max_width) {
            max_width = width;
        }
    }

    return {
        2.0 * (m_padding + m_margin) + max_width,
        n * (height + 2.0 * m_padding + m_margin) + m_margin
    };
}

void MenuState::m_select()
{
    const std::string& entry = m_entries[m_current];

    if (entry == "New game") {
        m_context->m_board.clear();
        m_context->m_score = 0;
        m_context->gen_next_deal(m_context->m_config.get_integer("DEAL_COUNT_INIT"));
        m_context->set_state_deal();

    } else if (entry == "High score") {
        m_context->m_score = -1;
        m_context->set_state_high_score();

    } else if (entry == "Exit") {
        m_context->m_alive = false;

    } else {
        throw std::runtime_error("Incogerent menu configuration");

    }
}

MenuState::MenuState(KulkiContext* const context) :
    m_context { context },
    m_padding { m_context->m_config.get_real("MENU_PADDING") },
    m_margin { m_context->m_config.get_real("MENU_MARGIN") },
    m_bg_color_l(m_context->m_config.get_color("MENU_BG_COLOR_L")),
    m_bg_color_d(m_context->m_config.get_color("MENU_BG_COLOR_D")),
    m_regular_color(m_context->m_config.get_color("MENU_REGULAR_COLOR")),
    m_select_color(m_context->m_config.get_color("MENU_SELECT_COLOR")),
    m_entries { "New game", "High score", "Exit" },
    m_current { 0 }
{
    std::tie(m_width, m_height) = m_compute_dimensions();
}

void MenuState::reset()
{
}

void MenuState::on_button(int button, bool down)
{
    if (button == 1 && down) {
        m_select();
    }
}

void MenuState::on_cursor(int x, int y)
{
    m_context->m_cursor_tile.first = x;
    m_context->m_cursor_tile.second = y;
}

void MenuState::draw(double)
{
    const double height = al_get_font_line_height(m_context->m_menu_font);
    const double x = double(m_context->m_screen_w - m_width) / 2.0;
    double y = double(m_context->m_screen_h- m_height) / 2.0;

    for (decltype(m_entries)::size_type i = 0; i < m_entries.size(); ++i) {

        const std::string& entry = m_entries[i];

        const double x1 = x + m_margin;
        const double y1 = y + m_margin;
        const double x2 = x + m_width - m_margin;
        const double y2 = y + m_margin + height + 2.0 * m_padding;
        al_draw_filled_rectangle(x1, y1, x2, y2, m_bg_color_l);
        al_draw_rectangle(x1, y1, x2, y2, m_bg_color_d, 3);

        const double cx = x + m_width / 2.0;
        const double cy = y + m_margin + m_padding + height / 2.0;
        const auto color = (i == m_current) ? m_select_color : m_regular_color;
        al_draw_textf(
                m_context->m_menu_font,
                color,
                cx, cy - height / 2,
                ALLEGRO_ALIGN_CENTRE,
                "%s", entry.c_str());

        y += height + 2.0 * m_padding + m_margin;

        if (m_context->m_cursor_tile.first > x1 && m_context->m_cursor_tile.first < x2 &&
            m_context->m_cursor_tile.second > y1 && m_context->m_cursor_tile.second < y2) {
            m_current = i;
        }
    }
}

