#include <sstream>

#include "high_score_state.h"
#include "kulki_context.h"

std::vector<std::string> HighScoreState::m_gen_outries(const std::vector<HighScoreEntry>& entries)
{
    std::vector<std::string> result;
    for (const auto& e : entries) {
        std::stringstream ss;
        int padding = m_context->m_const.highscore_characters - e.str_len();
        ss << e.name << std::string(padding, '.') << e.score;
        result.push_back(ss.str());
    }
    return result;
}

HighScoreState::HighScoreState(KulkiContext* context, int score) :
    m_context { context },
    m_high_score { HighScore::load("high_score", m_context->m_const.highscore_max_entries) },
    m_balls { m_context->m_const.ball_count },
    m_score { score }
{
    if (score != -1 && m_high_score.can_insert(m_balls, m_score)) {
        m_phase = HIGH_SCORE_INPUT;
        m_name.clear();
    } else {
        m_phase = HIGH_SCORE_DISPLAY;
        m_ball_counts = m_high_score.get_ball_counts();
        m_period = 5.0;
        m_timer = m_period;
    }
}

void HighScoreState::on_key(int key, bool down)
{
    if (!down) {
        return;
    }

    switch (m_phase) {
    case HIGH_SCORE_INPUT:
        if (key >= ALLEGRO_KEY_A && key <= ALLEGRO_KEY_Z) {
            m_name.push_back(key + 'A' - 1);

        } else if (key == ALLEGRO_KEY_SPACE) {
            m_name.push_back(' ');

        } else if (key == ALLEGRO_KEY_BACKSPACE && !m_name.empty()) {
            m_name.pop_back();

        } else if (key == ALLEGRO_KEY_ENTER) {
            m_high_score.add_entry({ m_balls, m_score, m_name });
            m_phase = HIGH_SCORE_DISPLAY;
            m_ball_counts = m_high_score.get_ball_counts();
            m_period = 5.0;
            m_timer = m_period;

        }
        break;

    case HIGH_SCORE_DISPLAY:
        HighScore::store("high_score", m_high_score);
        t_transition_required = true;
        break;
    }
}

void HighScoreState::tick(double dt)
{
    switch (m_phase) {
    case HIGH_SCORE_INPUT:
        break;

    case HIGH_SCORE_DISPLAY:
        m_timer -= dt;
        if (m_timer <= 0) {
            m_timer = m_period;
            std::rotate(
                begin(m_ball_counts),
                begin(m_ball_counts) + 1,
                end(m_ball_counts));
        }
        break;
    }

}

void HighScoreState::draw(double)
{
    const double text_height = al_get_font_line_height(m_context->m_const.menu_font);
    const double line_height = text_height + 20;
    double y;

    switch (m_phase) {
    case HIGH_SCORE_INPUT:
        al_draw_textf(
            m_context->m_const.menu_font,
            al_map_rgb_f(1, 1, 1),
            m_context->m_const.screen_w / 2,
            m_context->m_const.screen_w / 2,
            ALLEGRO_ALIGN_CENTRE,
            "typename: %s",
            m_name.c_str());
        break;

    case HIGH_SCORE_DISPLAY:

        std::vector<std::string> outries = m_gen_outries(
            m_high_score.get_entries_for_balls(
                m_ball_counts.front()));
        std::reverse(begin(outries), end(outries));

        al_draw_textf(
            m_context->m_const.menu_font,
            al_map_rgb_f(1, 1, 1),
            m_context->m_const.screen_w / 2, 100.0,
            ALLEGRO_ALIGN_CENTER,
            "High score for %d balls",
            m_ball_counts.front());

        y = m_context->m_const.screen_h / 2 - (outries.size() / 2) * line_height;

        for (const std::string& outry : outries) {
            al_draw_text(
                m_context->m_const.menu_font,
                al_map_rgb_f(1, 1, 1),
                m_context->m_const.screen_w / 2, y,
                ALLEGRO_ALIGN_CENTRE,
                outry.c_str());
            y += line_height;
        }
        break;
    }
}

std::shared_ptr<dick::StateNode> HighScoreState::next_state()
{
    return std::shared_ptr<dick::StateNode> { new MenuState { m_context } };
}
