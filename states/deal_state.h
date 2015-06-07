#ifndef DEAL_STATE_H
#define DEAL_STATE_H

#include "kulki_state.h"

class DealState : public KulkiState {

    KulkiContext* const m_context;

    double m_time;
    std::vector<std::pair<int, int>> m_positions;

public:
    DealState(KulkiContext* context);
    void reset(double time);
    void tick(double dt) override;
};

#endif
