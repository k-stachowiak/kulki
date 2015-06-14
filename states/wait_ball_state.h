#ifndef WAIT_BALL_STATE_H
#define WAIT_BALL_STATE_H

#include "state_node.h"

struct KulkiContext;

class WaitBallState : public StateNode {
    KulkiContext* const m_context;
    bool m_usure_phase;

public:
    WaitBallState(KulkiContext* context);
    void on_key(int key, bool down) override;
    void on_button(int button, bool down) override;
    void draw(double weight) override;
};

#endif
