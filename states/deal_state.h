#ifndef DEAL_STATE_H
#define DEAL_STATE_H

#include "kulki_state.h"

class DealState : public KulkiState {

    KulkiContext* const m_context;

    int m_current_x, m_current_y, m_current_color;
    double m_time;
    std::vector<std::pair<int, int>> m_positions;

    void deal_next();

public:
    DealState(KulkiContext* context);
    void reset(double time);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
};

#endif
