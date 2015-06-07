#ifndef WAIT_DEST_STATE_H
#define WAIT_DEST_STATE_H

#include "kulki_state.h"

class WaitDestState : public KulkiState {
    KulkiContext* const m_context;

    int m_src_x, m_src_y;
    int m_color;
    double m_time;

public:
    WaitDestState(KulkiContext* context);
    void reset(int src_x, int src_y, int color, double time);

    int get_src_x() const { return m_src_x; }
    int get_src_y() const { return m_src_y; }
    int get_color() const { return m_color; }

    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_key(int key, bool down) override;
    void on_button(int button, bool down) override;
};

#endif
