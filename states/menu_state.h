#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <vector>

#include "kulki_context.h"

struct KulkiContext;

class MenuState : public StateNode {

    KulkiContext* const m_context;

    double m_padding;
    double m_margin;
    ALLEGRO_COLOR m_bg_color_l;
    ALLEGRO_COLOR m_bg_color_d;
    ALLEGRO_COLOR m_regular_color;
    ALLEGRO_COLOR m_select_color;

    const std::vector<std::string> m_entries;
    unsigned m_current;
    double m_width, m_height;

    std::pair<double, double> m_compute_dimensions();
    void m_select();

public:
    MenuState(KulkiContext* const context);
    void reset();
    void on_button(int button, bool down) override;
    void on_cursor(int x, int y) override;
    void draw(double weight) override;
};


#endif
