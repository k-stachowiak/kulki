#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "dick.h"

struct KulkiContext;

class GameoverState : public dick::StateNode {
    KulkiContext* const m_context;
    double m_time;
    int m_index;

public:
    GameoverState(KulkiContext* context);
    void on_key(int key, bool down) override;
    void tick(double dt) override;
    void draw(double weight) override;
    std::shared_ptr<dick::StateNode> next_state() override;
};

#endif
