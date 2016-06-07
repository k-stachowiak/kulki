// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <vector>
#include <string>

#include "dick.h"

struct KulkiContext;

class MenuState : public dick::StateNode {

    KulkiContext* const m_context;

    const std::vector<std::pair<std::string, dick::GUI::Callback>> m_entries;
    double m_width, m_height;

    std::shared_ptr<StateNode> m_next_state;

    std::pair<double, double> m_compute_dimensions();
    void m_select();
    void m_on_new_game();
    void m_on_high_score();
    void m_on_exit();

public:
    MenuState(KulkiContext* const context);
    void draw(double weight) override;
    std::shared_ptr<StateNode> next_state() override;
};


#endif
