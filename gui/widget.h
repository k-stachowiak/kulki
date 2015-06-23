#ifndef WIDGET_H
#define WIDGET_H

#include <glm/glm.hpp>
#include "resources.h"

using WidgetOffset = glm::vec2;
using WidgetSize = glm::vec2;

enum class WidgetVerticalAlignment {
    Top, Middle, Bottom
};

enum class WidgetHorizontalAlignment {
    Left, Center, Right
};

struct WidgetAlignment {
    WidgetVerticalAlignment vertical;
    WidgetHorizontalAlignment horizontal;
};

class Widget {
public:
    WidgetSize t_current_size;
    virtual void draw(const WidgetOffset &offset) const = 0;
    virtual WidgetSize min_size() const = 0;
    virtual bool request_size(const WidgetSize &size) = 0;
};

void gui_init(Resources* resources, const std::string &font_filename);

WidgetSize gui_text_size(const std::string &text, int font_size);

WidgetOffset gui_text_offset(
        const WidgetOffset &offset,
        const std::string &text,
        int font_size,
        WidgetAlignment alignment);

#endif
