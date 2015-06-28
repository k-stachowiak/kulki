#ifndef DEAL_STATE_H
#define DEAL_STATE_H

#include <vector>

#include "dick.h"

struct KulkiContext;

class DealState : public dick::StateNode {

    KulkiContext* const m_context;

    int m_current_x, m_current_y, m_current_color;
    double m_time;
    std::vector<std::pair<int, int>> m_positions;

    void deal_next();

public:
    DealState(KulkiContext* context, double time);
    void tick(double dt) override;
    void draw(double weight) override;

    std::shared_ptr<StateNode> next_state() override;
};

#endif
