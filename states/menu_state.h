#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "kulki_state.h"

class MenuState : public KulkiState {

    Board* const m_board;
    KulkiContext* const m_context;

    ALLEGRO_FONT* m_menu_font;

    int* m_score;
    bool* m_alive;
    std::pair<int, int> m_cursor;

    const std::vector<std::string> m_entries;
    unsigned m_current;
    double m_width, m_height;

    std::pair<double, double> m_compute_dimensions();
    void m_select();

public:
    MenuState(Board* board, KulkiContext* const context, ALLEGRO_FONT* menu_font, int* score, bool* alive);
    void reset();
    void on_button(int button, bool down) override;
    void on_cursor(int x, int y) override;
    void draw(const glm::mat3&) override;
};


#endif
