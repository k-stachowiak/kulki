#ifndef SCORE_STATE_H
#define SCORE_STATE_H

#include "kulki_state.h"

class ScoreState : public KulkiState {

    KulkiContext* const m_context;

    double m_time;
    double m_cx, m_cy;
    int m_incr;

public:
    ScoreState(KulkiContext* context);
    void reset(double time, double cx, double cy, int incr);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
};

#endif
