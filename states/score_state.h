// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef SCORE_STATE_H
#define SCORE_STATE_H

#include "dick.h"

class ScoreState : public dick::StateNode {

    KulkiContext* const m_context;

    double m_time;
    double m_cx, m_cy;
    int m_incr;
    bool m_next_deal;
    std::shared_ptr<StateNode> m_next_state;

public:
    ScoreState(KulkiContext* context, const std::vector<std::pair<int, int>>& changes, bool next_deal);
    void tick(double dt) override;
    void draw(double weight) override;
    std::shared_ptr<StateNode> next_state() override;
};

#endif
