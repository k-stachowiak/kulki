#ifndef CONTEXT_H
#define CONTEXT_H

#include "kulki_state.h"
#include "menu_state.h"
#include "deal_state.h"
#include "wait_ball_state.h"
#include "wait_dest_state.h"
#include "move_state.h"
#include "score_state.h"
#include "high_score_state.h"
#include "game_over_state.h"

struct KulkiContext {

    Board* m_board;
    int* m_score;
    bool* m_alive;

    std::pair<int, int>* m_cursor_tile;
    ALLEGRO_FONT *m_score_font;
    ALLEGRO_FONT *m_gameover_font;
    ALLEGRO_FONT *m_menu_font;

    MenuState m_menu_state;
    DealState m_deal_state;
    WaitBallState m_wait_ball_state;
    WaitDestState m_wait_dest_state;
    MoveState m_move_state;
    ScoreState m_score_state;
    GameoverState m_gameover_state;
    HighScoreState m_high_score_state;

    KulkiState *m_current_state;

    KulkiContext(Board* board, int* score, bool* alive, std::pair<int, int>* cursor_tile,
            ALLEGRO_FONT* score_font, ALLEGRO_FONT* gameover_font, ALLEGRO_FONT* menu_font);

    void set_state_menu();

    void set_state_wait_ball();
    void reset_state_wait_ball(int src_x, int src_y, int color);

    void set_state_wait_dest(int src_x, int src_y);
    void reset_state_wait_dest(int src_x, int src_y);

    void set_state_deal(int count);
    void set_state_gameover();
    void set_state_score(const std::vector<std::pair<int, int>>& changes, KulkiState::Enum next_state);
    void set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color);
    void set_state_high_score();
};

#endif
