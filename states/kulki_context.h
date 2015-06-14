#ifndef CONTEXT_H
#define CONTEXT_H

#include "resources.h"
#include "state_node.h"
#include "board.h"
#include "menu_state.h"
#include "deal_state.h"
#include "wait_ball_state.h"
#include "wait_dest_state.h"
#include "move_state.h"
#include "score_state.h"
#include "high_score_state.h"
#include "game_over_state.h"

struct KulkiContext : public StateNode {

    Resources m_resources;
    Board m_board;
    bool m_alive;

    ALLEGRO_FONT* m_gameover_font;
    ALLEGRO_FONT* m_score_font;
    ALLEGRO_FONT *m_menu_font;
    ALLEGRO_BITMAP *m_ball_bmp;
    ALLEGRO_BITMAP *m_tile_bmp;
    std::pair<int, int> m_cursor_screen;
    std::pair<int, int> m_cursor_tile;
    int m_ball_count;

    MenuState m_menu_state;
    DealState m_deal_state;
    WaitBallState m_wait_ball_state;
    WaitDestState m_wait_dest_state;
    MoveState m_move_state;
    ScoreState m_score_state;
    GameoverState m_gameover_state;
    HighScoreState m_high_score_state;

    StateNode *m_current_state;
    std::vector<int> m_next_deal;
    int m_score;
    int m_streak;

    KulkiContext();

    void gen_next_deal(int count);

    glm::mat3 m_current_transform();

    void draw_field(const glm::vec3& top_left, const glm::vec3& bot_right, bool fill, const glm::mat3& transf);
    void draw_ball(double x, double y, int color, double r, double squeeze, const glm::mat3& transf);
    void draw_board(const Board& b, const glm::mat3& transf);

    bool is_done() const;
    void on_kill();
    void on_key(int key, bool down);
    void on_button(int button, bool down);
    void on_cursor(int x, int y);
    void tick(double dt);
    void draw(double weight);

    void set_state_menu();

    void set_state_wait_ball();
    void reset_state_wait_ball(int src_x, int src_y, int color);

    void set_state_wait_dest(int src_x, int src_y);
    void reset_state_wait_dest(int src_x, int src_y);

    void set_state_deal();
    void set_state_gameover();
    void set_state_score(const std::vector<std::pair<int, int>>& changes, bool next_deal);
    void set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color);
    void set_state_high_score();
};

#endif
