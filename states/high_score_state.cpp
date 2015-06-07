#include <sstream>

#include "high_score_state.h"
#include "kulki_context.h"

std::vector<std::string> HighScoreState::m_gen_outries(const std::vector<HighScore::Entry>& entries)
{
    std::vector<std::string> result;
    for (const auto& e : m_high_score->get_entries()) {
        std::stringstream ss;
        int padding = config::HIGHSCORE_CHARACTERS - e.str_len();
        ss << e.name << std::string(padding, '.') << e.score;
        result.push_back(ss.str());
    }
    return result;
}

HighScoreState::HighScoreState(KulkiContext* context) :
    m_context { context }
{}

void HighScoreState::reset(int score)
{
    m_score = score;
    m_high_score.reset(new HighScore { "high_score" });
    if (m_high_score->insert_position(m_score) != -1) {
        std::cout << "insert position = " << m_high_score->insert_position(m_score) << std::endl;
        m_phase = HIGH_SCORE_INPUT;
        m_input_name.clear();
    } else {
        m_phase = HIGH_SCORE_DISPLAY;
    }
}

void HighScoreState::on_key(int key, bool down)
{
    if (!down) {
        return;
    }

    switch (m_phase) {
    case HIGH_SCORE_INPUT:
        if ((key >= ALLEGRO_KEY_A && key <= ALLEGRO_KEY_Z) || key == ALLEGRO_KEY_SPACE) {
            m_input_name.push_back(key + 'A' - 1);
        } else if (key == ALLEGRO_KEY_BACKSPACE) {
            m_input_name.pop_back();
        } else if (key == ALLEGRO_KEY_ENTER) {
            m_high_score->add_entry(HighScore::Entry { m_input_name, m_score });
            m_phase = HIGH_SCORE_DISPLAY;
        } else if (key == ALLEGRO_KEY_ESCAPE) {
            m_phase = HIGH_SCORE_DISPLAY;
        }
        break;

    case HIGH_SCORE_DISPLAY:
        m_high_score.reset();
        m_context->set_state_gameover();
        break;
    }
}

void HighScoreState::draw(const glm::mat3&)
{
    const double text_height = al_get_font_line_height(m_context->m_menu_font);
    const double line_height = text_height + 20;
    const std::vector<std::string> outries = m_gen_outries(m_high_score->get_entries());
    double y = config::SCREEN_H / 2 - (outries.size() / 2) * line_height;
    for (decltype(outries)::size_type i = 0; i < outries.size(); ++i) {
        if (m_phase == HIGH_SCORE_INPUT && i == (unsigned)m_high_score->insert_position(m_score)) {
            al_draw_text(m_context->m_menu_font, al_map_rgb_f(1, 1, 1), config::SCREEN_W / 2, y, ALLEGRO_ALIGN_CENTRE, m_input_name.c_str());
        } else {
            const auto& outry = outries.at(i);
            al_draw_text(m_context->m_menu_font, al_map_rgb_f(1, 1, 1), config::SCREEN_W / 2, y, ALLEGRO_ALIGN_CENTRE, outry.c_str());
        }
        y += line_height;
    }
}

