#ifndef SCORE_STATE_H
#define SCORE_STATE_H

#include "state_node.h"

class ScoreState : public StateNode {

    KulkiContext* const m_context;

    double m_period;
    ALLEGRO_COLOR m_score_color;
    double m_time;
    double m_cx, m_cy;
    int m_incr;
    bool m_next_deal;

public:
    ScoreState(KulkiContext* context);
    void reset(const std::vector<std::pair<int, int>>& changes, bool next_deal);
    void tick(double dt) override;
    void draw(double weight) override;
};

#endif
