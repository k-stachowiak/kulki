#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "kulki_state.h"

class GameoverState : public KulkiState {
    KulkiContext* const m_context;

    int* m_score;

    double m_time;
    int m_index;

public:
    GameoverState(KulkiContext* context);
    void reset(int *score, double time, int index);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_key(int key, bool down) override;
};

#endif
