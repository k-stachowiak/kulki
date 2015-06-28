#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <vector>

#include "kulki_context.h"

struct KulkiContext;

class MenuState : public dick::StateNode {

    KulkiContext* const m_context;

    const std::vector<std::string> m_entries;
    unsigned m_current;
    double m_width, m_height;

    std::shared_ptr<StateNode> m_next_state;

    std::pair<double, double> m_compute_dimensions();
    void m_select();

public:
    MenuState(KulkiContext* const context);
    void on_button(int button, bool down) override;
    void draw(double weight) override;
    std::shared_ptr<StateNode> next_state() override;
};


#endif
