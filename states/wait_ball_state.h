#ifndef WAIT_BALL_STATE_H
#define WAIT_BALL_STATE_H

#include "dick.h"

struct KulkiContext;

class WaitBallState : public dick::StateNode {
    KulkiContext* const m_context;
    bool m_usure_phase;
    std::shared_ptr<StateNode> m_next_state;

public:
    WaitBallState(KulkiContext* context);
    void on_key(int key, bool down) override;
    void on_button(int button, bool down) override;
    void draw(double weight) override;
    std::shared_ptr<StateNode> next_state() override;
};

#endif
