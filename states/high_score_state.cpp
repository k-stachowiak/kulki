// Copyright (C) 2015 Krzysztof Stachowiak

#include <sstream>
#include <allegro5/allegro_primitives.h>

#include "dick.h"
#include "high_score.h"
#include "kulki_context.h"

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

public:
    HighScoreState(KulkiContext* context, int score) :
        m_context { context },
        m_high_score { HighScore::load("high_score", m_context->m_const.highscore_max_entries) },
        m_balls { m_context->m_var.m_ball_count },
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

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        switch (m_phase) {
        case HIGH_SCORE_INPUT:
            if (key >= dick::Key::A && key <= dick::Key::Z) {
                m_name.push_back(static_cast<int>(key) + 'A' - 1);

            } else if (key == dick::Key::SPACE) {
                m_name.push_back(' ');

            } else if (key == dick::Key::BACKSPACE && !m_name.empty()) {
                m_name.pop_back();

            } else if (key == dick::Key::ENTER) {
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

    void on_button(dick::Button, bool down) override
    {
        if (!down) {
            return;
        }

        switch (m_phase) {
        case HIGH_SCORE_INPUT:
            break;

        case HIGH_SCORE_DISPLAY:
            HighScore::store("high_score", m_high_score);
            t_transition_required = true;
            break;
        }
    }

    void tick(double dt) override
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

    void draw(double) override
    {
        const double text_height = al_get_font_line_height(m_context->m_const.menu_font);
        const double line_height = text_height + 15;
        double y;

        m_context->draw_veil();

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

            std::vector<HighScoreEntry> entries = m_high_score.get_entries_for_balls(m_ball_counts.front());
            std::reverse(begin(entries), end(entries));

            y = m_context->m_const.screen_h / 2 - (entries.size() / 2) * line_height;

            al_draw_textf(
                m_context->m_const.menu_font,
                al_map_rgb_f(1, 1, 1),
                m_context->m_const.screen_w / 2, 60.0,
                ALLEGRO_ALIGN_CENTER,
                "High score for %d balls",
                m_ball_counts.front());

            for (const auto& entry: entries) {

                al_draw_textf(
                    m_context->m_const.menu_font,
                    al_map_rgb_f(1, 1, 1),
                    100.0, y,
                    ALLEGRO_ALIGN_LEFT,
                    "%s", entry.name.c_str());

                al_draw_textf(
                    m_context->m_const.menu_font,
                    al_map_rgb_f(1, 1, 1),
                    m_context->m_const.screen_w - 100.0, y,
                    ALLEGRO_ALIGN_RIGHT,
                    "%d", entry.score);

                y += line_height;
            }
            break;
        }
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_menu_state(m_context);
    }

};

std::shared_ptr<dick::StateNode> make_highscore_state(
        KulkiContext *context,
        int score)
{
    return std::make_shared<HighScoreState>(context, score);
}
