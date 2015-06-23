#ifndef WIDGET_BAR_H
#define WIDGET_BAR_H

#include <string>

#include "widget.h"

class WidgetButton : public Widget {
    double m_padding;
    std::string m_text;
    int m_font_size;
    WidgetAlighment m_text_alignment;

public:
    WidgetButton() = default;

    WidgetButton(const std::string& text, int font_size, double padding) :
        m_text { text },
        m_font_size { font_size },
        m_padding { padding }
    {
        t_current_size = min_size();
    }

    void draw(const WidgetOffset &offset) const
    {
        WidtetSize size = min_size();
        WidgetSize padded_size = size - 2.0 * m_padding;
        WidgetOffset text_offset = offset + m_padding;

        switch (m_text_alignment.horizontal) {
        case WidgetHorizontalAlignment::Left:
            break;
        case WidgetHorizontalAlignment::Center:
            text_offset.x += padded_size.x / 2.0;
            break;
        case WidgetHorizontalAlignment::Right:
            text_offset.x += padded_size.x;
            break;
        }

        switch (m_text_alignment.vertical) {
        case WidgetVerticalAlignment::Top:
            break;
        case WidgetVerticalAlignment::Middle:
            text_offset.y += padded_size.y / 2.0;
            break;
        case WidgetVerticalAlignment::Right:
            text_offset.y += padded_size.y;
            break;
        }

        al_draw_filled_rectangle(
            offset.x,
            offset.y,
            offset.x + size.x,
            offset.y + size.y,
            al_map_rgba_f(0.125, 0.125, 0.125, 0.75));

        al_draw_text(
            m_font,
            text_offset.x,
            text_offset.y,
            0,
            m_text.c_str());
    }

    WidgetSize min_size() const
    {
        WidgetSize text_size = gui_compute_size(m_text, m_font_size);
        return text_size + 2.0 * m_padding;
    }

    bool request_size(const WidgetSize &size)
    {
        return false;
    }
};

#endif
