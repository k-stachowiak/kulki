#ifndef MOVE_STATE_H
#define MOVE_STATE_H

#include "kulki_state.h"

class MoveState : public KulkiState {

    Board* const m_board;
    KulkiContext* const m_context;

    std::deque<std::pair<int, int>> m_path;
    double m_time;
    int m_dst_x, m_dst_y;
    int m_color;

public:
    MoveState(
            Board* board, KulkiContext* context,
            std::deque<std::pair<int, int>>&& path,
            double time, int dst_x, int dst_y, int color);

    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
};

#endif
