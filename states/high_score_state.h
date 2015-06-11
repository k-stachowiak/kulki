#ifndef HIGH_SCORE_STATE_H
#define HIGH_SCORE_STATE_H

#include "kulki_state.h"

class HighScoreState : public KulkiState {

    KulkiContext* const m_context;

    HighScore m_high_score;

    enum {
        HIGH_SCORE_INPUT,
        HIGH_SCORE_DISPLAY
    } m_phase;

    int m_balls;
    int m_score;
    std::string m_name;

    std::vector<int> m_ball_counts;
    double m_period;
    double m_timer;

    std::vector<std::string> m_gen_outries(const std::vector<HighScoreEntry>& entries);

public:
    HighScoreState(KulkiContext* context);
    void reset(int balls, int score);
    void on_key(int key, bool down) override;
    void tick(double dt) override;
    void draw(const glm::mat3&) override;
};

#endif
