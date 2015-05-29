#ifndef HIGH_SCORE_STATE_H
#define HIGH_SCORE_STATE_H

#include "kulki_state.h"

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
    HighScoreState(KulkiContext* context, ALLEGRO_FONT* menu_font, int score);
    void on_key(int key, bool down) override;
    void draw(const glm::mat3&) override;
};

#endif
