#ifndef WAIT_DEST_STATE_H
#define WAIT_DEST_STATE_H

#include "kulki_state.h"

class WaitDestState : public KulkiState {
    Board* const m_board;
    KulkiContext* const m_context;

    std::pair<int, int>* m_cursor_tile;

    int m_src_x, m_src_y;
    int m_color;
    double m_time;

public:
    WaitDestState(
        Board* board, KulkiContext* context, std::pair<int, int>* cursor_tile,
        int src_x, int src_y, int color, double time);

    int get_src_x() const { return m_src_x; }
    int get_src_y() const { return m_src_y; }
    int get_color() const { return m_color; }

    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_button(int button, bool down) override;
};

#endif
