// Copyright (C) 2015 Krzysztof Stachowiak

#include <memory>

#include <allegro.h>

#include <glm/glm.hpp>

#include "board.h"
#include "high_score.h"

struct KulkiContext;

struct KulkiState {

    enum class Enum {
        DEAL,
        WAIT_BALL,
        WAIT_DEST,
        MOVE,
        GAMEOVER,
        SCORE
    };

    virtual void on_key(int key, bool down) {}
    virtual void on_button(int button, bool down) {}
    virtual void on_cursor(int x, int y) {}
    virtual void tick(double dt) {}
    virtual void draw(const glm::mat3& transf) {}
};

class MenuState : public KulkiState {

    Board* const m_board;
    KulkiContext* const m_context;

    ALLEGRO_FONT* m_menu_font;

    int* m_score;
    bool* m_alive;

    const std::vector<std::string> m_entries;
    unsigned m_current;
    double m_width, m_height;

    std::pair<double, double> m_compute_dimensions();
    void m_select();

public:
    MenuState(Board* board, KulkiContext* const context, ALLEGRO_FONT* menu_font, int* score, bool* alive);
    void reset() {}
    void on_key(int key, bool down) override;
    void draw(const glm::mat3&) override;
};

class DealState : public KulkiState {

    std::random_device m_rdev;
    std::default_random_engine m_reng;

    Board* const m_board;
    KulkiContext* const m_context;

    double m_time;
    int m_count;
    std::vector<std::pair<int, int>> m_positions;

    void m_new_ball(Board& b, int& x, int& y, int& color);

public:
    DealState(Board* board, KulkiContext* context);
    void reset(double time, int count);
    void tick(double dt) override;
};

class WaitBallState : public KulkiState {
    Board* const m_board;
    KulkiContext* const m_context;

    std::pair<int, int>* m_cursor_tile;

public:
    WaitBallState(Board* board, KulkiContext* context, std::pair<int, int> *cursor_tile);
    void on_button(int button, bool down) override;
};

class WaitDestState : public KulkiState {
    Board* const m_board;
    KulkiContext* const m_context;

    std::pair<int, int>* m_cursor_tile;

    int m_src_x, m_src_y;
    int m_color;
    double m_time;

public:
    WaitDestState(Board* board, KulkiContext* context, std::pair<int, int>* cursor_tile);
    void reset(int src_x, int src_y, int color, double time);

    int get_src_x() const { return m_src_x; }
    int get_src_y() const { return m_src_y; }
    int get_color() const { return m_color; }

    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_button(int button, bool down) override;
};

class MoveState : public KulkiState {

    Board* const m_board;
    KulkiContext* const m_context;

    std::deque<std::pair<int, int>> m_path;
    double m_time;
    int m_dst_x, m_dst_y;
    int m_color;

public:
    MoveState(Board* board, KulkiContext* context);
    void reset(std::deque<std::pair<int, int>> path, double time, int dst_x, int dst_y, int color);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
};

class ScoreState : public KulkiState {

    KulkiContext* const m_context;
    ALLEGRO_FONT* m_score_font;

    double m_time;
    double m_cx, m_cy;
    int m_incr;

public:
    ScoreState(KulkiContext* context, ALLEGRO_FONT* score_font);
    void reset(double time, double cx, double cy, int incr);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
};

class HighScoreState : public KulkiState {

    KulkiContext* const m_context;
    ALLEGRO_FONT* m_menu_font;

    enum {
        HIGH_SCORE_INPUT,
        HIGH_SCORE_DISPLAY
    } m_phase;

    int m_score;
    std::unique_ptr<HighScore> m_high_score;
    std::string m_input_name;

    std::vector<std::string> m_gen_outries(const std::vector<HighScore::Entry>& entries);

public:
    HighScoreState(KulkiContext* context, ALLEGRO_FONT* menu_font);
    void reset(int score);
    void on_key(int key, bool down) override;
    void draw(const glm::mat3&) override;
};

class GameoverState : public KulkiState {
    KulkiContext* const m_context;
    ALLEGRO_FONT* m_gameover_font;

    int* m_score;

    double m_time;
    int m_index;

public:
    GameoverState(KulkiContext* context, ALLEGRO_FONT* gameover_font);
    void reset(int *score, double time, int index);
    void tick(double dt) override;
    void draw(const glm::mat3& transf) override;
    void on_key(int key, bool down) override;
};

struct KulkiContext {

    Board* m_board;
    int* m_score;
    bool* m_alive;

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

