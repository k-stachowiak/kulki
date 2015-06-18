#ifndef DEAL_STATE_H
#define DEAL_STATE_H

#include <vector>

#include "state_node.h"

struct KulkiContext;

class DealState : public StateNode {

    KulkiContext* const m_context;

    double m_period;
    int m_current_x, m_current_y, m_current_color;
    double m_time;
    std::vector<std::pair<int, int>> m_positions;

    void deal_next();

public:
    DealState(KulkiContext* context);
    void reset(double time);
    void tick(double dt) override;
    void draw(double weight) override;
};

#endif
