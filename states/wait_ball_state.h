#ifndef WAIT_BALL_STATE_H
#define WAIT_BALL_STATE_H

#include "kulki_state.h"

class WaitBallState : public KulkiState {
    KulkiContext* const m_context;
    bool m_usure_phase;

public:
    WaitBallState(KulkiContext* context);
    void on_key(int key, bool down) override;
    void on_button(int button, bool down) override;
    void draw(const glm::mat3& transf) override;
};

#endif
