// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef WAIT_DEST_STATE_H
#define WAIT_DEST_STATE_H

#include "dick.h"

struct KulkiContext;

class WaitDestState : public dick::StateNode {
    KulkiContext* const m_context;

    int m_src_x, m_src_y;
    int m_color;
    double m_time;

    std::shared_ptr<StateNode> m_next_state;

public:
    WaitDestState(KulkiContext* context, int src_x, int src_y);

    void on_key(dick::Key key, bool down) override;
    void on_button(dick::Button button, bool down) override;
    void tick(double dt) override;
    void draw(double weight) override;
    std::shared_ptr<StateNode> next_state() override;
};

#endif
