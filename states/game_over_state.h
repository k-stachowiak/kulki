#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "kulki_state.h"

class GameoverState : public KulkiState {
    KulkiContext* const m_context;

    double m_time;
    int m_index;

public:
    GameoverState(KulkiContext* context);
    void reset(double time, int index);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_key(int key, bool down) override;
};

#endif
