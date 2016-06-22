// Copyright (C) 2015 Krzysztof Stachowiak

#include <sstream>
#include <allegro5/allegro_primitives.h>

#include "dick.h"
#include "high_score.h"
#include "kulki_context.h"

class HighScoreDisplayState : public dick::StateNode {

    KulkiContext* const m_context;

    HighScore m_high_score;

    std::vector<int> m_ball_counts;
    double m_period;
    double m_timer;

public:
    HighScoreDisplayState(KulkiContext* context) :
        m_context { context },
        m_high_score { HighScore::load("high_score", m_context->m_const.highscore_max_entries) }
    {
        m_ball_counts = m_high_score.get_ball_counts();
        m_period = 5.0;
        m_timer = m_period;
    }

    void on_key(dick::Key, bool down) override
    {
        if (down) {
            t_transition_required = true;
        }
    }

    void on_button(dick::Button, bool down) override
    {
        if (down) {
            t_transition_required = true;
        }
    }

    void tick(double dt) override
    {
        m_timer -= dt;
        if (m_timer <= 0) {
            m_timer = m_period;
            std::rotate(
                begin(m_ball_counts),
                begin(m_ball_counts) + 1,
                end(m_ball_counts));
        }
    }

    void draw(double) override
    {
        const double text_height = al_get_font_line_height(m_context->m_const.menu_font);
        const double line_height = text_height + 15;
        double y;

        m_context->draw_veil();

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
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_menu_state(m_context);
    }

};

std::shared_ptr<dick::StateNode> make_highscore_display_state(KulkiContext *context)
{
    return std::make_shared<HighScoreDisplayState>(context);
}
