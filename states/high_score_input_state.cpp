#include <string>

#include "dick.h"
#include "high_score.h"
#include "kulki_context.h"

class HighScoreInputState : public dick::StateNode {

    KulkiContext* const m_context;
    HighScore m_high_score;
    std::string m_name;

public:
    HighScoreInputState(KulkiContext *context) :
        m_context { context },
        m_high_score { HighScore::load("high_score", m_context->m_const.highscore_max_entries) }
    {}

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        if (key >= dick::Key::A && key <= dick::Key::Z) {
            m_name.push_back(
                static_cast<int>(key) -
                static_cast<int>(dick::Key::A) +
                'A');

        } else if (key == dick::Key::SPACE) {
            m_name.push_back(' ');

        } else if (key == dick::Key::BACKSPACE && !m_name.empty()) {
            m_name.pop_back();

        } else if (key == dick::Key::ENTER) {
            m_high_score.add_entry({
                m_context->m_var.m_ball_count,
                m_context->m_var.m_score,
                m_name });
            HighScore::store("high_score", m_high_score);
            t_transition_required = true;
        }
    }

    void draw(double) override
    {
        m_context->draw_veil();

        al_draw_textf(
            m_context->m_const.menu_font,
            al_map_rgb_f(1, 1, 1),
            m_context->m_const.screen_w / 2,
            m_context->m_const.screen_w / 2,
            ALLEGRO_ALIGN_CENTRE,
            "name: %s",
            m_name.c_str());
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return make_highscore_display_state(m_context);
    }

};

std::shared_ptr<dick::StateNode> make_highscore_input_state(KulkiContext *context)
{
    return std::make_shared<HighScoreInputState>(context);
}
