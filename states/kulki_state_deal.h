#ifndef DEAL_STATE_H
#define DEAL_STATE_H

#include "kulki_state.h"

class DealState : public KulkiState {

    std::random_device m_rdev;
    std::default_random_engine m_reng;

    Board* const m_board;
    KulkiContext* const m_context;

    double m_time;
    int m_count;
    std::vector<std::pair<int, int>> m_positions;

    void m_new_ball(Board& b, int& x, int& y, int& color);

public:
    DealState(Board* board, KulkiContext* context, double time, int count);
    void tick(double dt) override;
};

#endif
