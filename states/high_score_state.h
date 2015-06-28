#ifndef HIGH_SCORE_STATE_H
#define HIGH_SCORE_STATE_H

#include "dick.h"
#include "high_score.h"

struct KulkiContext;

class HighScoreState : public dick::StateNode {

    KulkiContext* const m_context;

    HighScore m_high_score;
    int m_balls;
    int m_score;

    std::string m_name;
    enum {
        HIGH_SCORE_INPUT,
        HIGH_SCORE_DISPLAY
    } m_phase;

    std::vector<int> m_ball_counts;
    double m_period;
    double m_timer;

    std::vector<std::string> m_gen_outries(const std::vector<HighScoreEntry>& entries);

public:
    HighScoreState(KulkiContext* context, int score);
    void on_key(int key, bool down) override;
    void tick(double dt) override;
    void draw(double) override;
    std::shared_ptr<StateNode> next_state() override;
};

#endif
