#ifndef WAIT_BALL_STATE_H
#define WAIT_BALL_STATE_H

#include "kulki_state.h"

class WaitBallState : public KulkiState {
    Board* const m_board;
    KulkiContext* const m_context;

    std::pair<int, int>* m_cursor_tile;

public:
    WaitBallState(Board* board, KulkiContext* context, std::pair<int, int> *cursor_tile);
    void on_button(int button, bool down) override;
};

#endif
