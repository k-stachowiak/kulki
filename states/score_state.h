#ifndef SCORE_STATE_H
#define SCORE_STATE_H

#include "state_node.h"

class ScoreState : public StateNode {

    KulkiContext* const m_context;

    double m_time;
    double m_cx, m_cy;
    int m_incr;

public:
    ScoreState(KulkiContext* context);
    void reset(double time, double cx, double cy, int incr);
    void tick(double dt) override;
    void draw(double weight) override;
};

#endif
