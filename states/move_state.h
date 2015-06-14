#ifndef MOVE_STATE_H
#define MOVE_STATE_H

#include <deque>

#include "state_node.h"

struct KulkiContext;

class MoveState : public StateNode {

    KulkiContext* const m_context;

    std::deque<std::pair<int, int>> m_path;
    double m_time;
    int m_dst_x, m_dst_y;
    int m_color;

public:
    MoveState(KulkiContext* context);
    void reset(std::deque<std::pair<int, int>> path, double time, int dst_x, int dst_y, int color);
    void tick(double dt) override;
    void draw(double weight) override;
};

#endif
