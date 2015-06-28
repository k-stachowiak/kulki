#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "widget.h"

namespace {

    dick::Resources *g_resources = nullptr ;
    std::string g_font_filename = "";

}

void gui_init(dick::Resources *resources, const std::string &font_filename)
{
    g_resources = resources;
    g_font_filename = font_filename;
}

WidgetSize gui_text_size(const std::string &text, int font_size)
{
    ALLEGRO_FONT *font = static_cast<ALLEGRO_FONT*>(g_resources->get_font(g_font_filename, font_size));
    return {
        al_get_text_width(font, text.c_str()),
        al_get_font_ascent(font)
    };
}

WidgetOffset gui_text_offset(
        const WidgetOffset &offset,
        const std::string &text,
        int font_size,
        WidgetAlignment alignment)
{
    WidgetSize text_size = gui_text_size(text, font_size);

    double x_offset;
    switch (alignment.horizontal) {
    case WidgetHorizontalAlignment::Left:
        x_offset = 0;
        break;
    case WidgetHorizontalAlignment::Center:
        x_offset = -0.5 * text_size.x;
        break;
    case WidgetHorizontalAlignment::Right:
        x_offset = -text_size.x;
        break;
    }

    double y_offset;
    switch (alignment.vertical) {
    case WidgetVerticalAlignment::Top:
        y_offset = 0;
        break;
    case WidgetVerticalAlignment::Middle:
        y_offset = -0.5 * text_size.y;
        break;
    case WidgetVerticalAlignment::Bottom:
        y_offset = -text_size.y;
        break;
    }

    return {
        offset.x + x_offset,
        offset.y + y_offset
    };
}

