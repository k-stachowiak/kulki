// Copyright (C) 2015 Krzysztof Stachowiak

#include <sstream>
#include <allegro5/allegro_primitives.h>

#include "dick.h"
#include "high_score.h"
#include "kulki_context.h"

class HighScoreDisplayState : public dick::StateNode {

    KulkiContext* const m_context;

    std::unique_ptr<dick::GUI::WidgetContainer> m_ui;

    HighScore m_high_score;

    std::vector<int> m_ball_counts;
    int m_ball_index;
    double m_period;
    double m_timer;

    std::unique_ptr<dick::GUI::Widget> m_make_ball_spin()
    {
        auto larrow = m_context->m_gui.make_button(
            m_context->m_gui.make_image(m_context->m_const.larrow_bmp),
            [this]()
            {
                m_ball_index = (m_ball_index - 1) % m_ball_counts.size();
            });

        auto rarrow = m_context->m_gui.make_button(
            m_context->m_gui.make_image(m_context->m_const.rarrow_bmp),
            [this]()
            {
                m_ball_index = (m_ball_index + 1) % m_ball_counts.size();
            });

        auto rail = m_context->m_gui.make_container_rail(
            dick::GUI::Direction::RIGHT,
            rarrow->get_size().x * 1.25);

        rail->insert(std::move(larrow), dick::GUI::Alignment::MIDDLE);
        rail->insert(std::move(rarrow), dick::GUI::Alignment::MIDDLE);

        rail->insert(
            m_context->m_gui.make_label_ex(
                std::to_string(m_ball_counts[m_ball_index]) + " balls",
                m_context->m_const.menu_font),
            dick::GUI::Alignment::MIDDLE);

        std::unique_ptr<dick::GUI::Widget> result = std::move(rail);

        return result;
    }

    std::unique_ptr<dick::GUI::Widget> m_make_score_container()
    {
        const double text_height = al_get_font_line_height(m_context->m_const.menu_font);
        double max_width = 0;

        std::vector<HighScoreEntry> entries =
            m_high_score.get_entries_for_balls(m_ball_counts[m_ball_index]);
        std::reverse(begin(entries), end(entries));

        auto name_rail = m_context->m_gui.make_container_rail(
                dick::GUI::Direction::DOWN,
                text_height * 1.125);

        auto score_rail = m_context->m_gui.make_container_rail(
                dick::GUI::Direction::DOWN,
                text_height * 1.125);

        for (const auto& entry: entries) {
            double name_width = al_get_text_width(
                    m_context->m_const.menu_font,
                    entry.name.c_str());
            if (name_width > max_width) {
                max_width = name_width;
            }
            name_rail->insert(
                m_context->m_gui.make_label_ex(
                    entry.name,
                    m_context->m_const.menu_font));
            score_rail->insert(
                m_context->m_gui.make_label_ex(
                    std::to_string(entry.score),
                    m_context->m_const.menu_font));
        }

        score_rail->set_offset({ 2 * max_width, 0.0 });

        auto container = m_context->m_gui.make_container_free();
        container->insert(std::move(name_rail));
        container->insert(std::move(score_rail));

        auto panel = m_context->m_gui.make_container_panel();
        panel->insert(std::unique_ptr<dick::GUI::Widget> { container.release() });

        std::unique_ptr<dick::GUI::Widget> result = std::move(panel);

        return result;
    }

    void m_rebuild_ui()
    {
        auto head_widget = m_make_ball_spin();
        auto content_widget = m_make_score_container();
        auto done_button = m_context->m_gui.make_button(
            m_context->m_gui.make_label_ex(
                "Done",
                m_context->m_const.menu_font),
            [this]()
            {
                t_transition_required = true;
            });

        auto container = m_context->m_gui.make_container_box(
             dick::GUI::Direction::DOWN,
             20.0);
        container->insert(std::move(head_widget));
        container->insert(std::move(content_widget));
        container->insert(std::move(done_button));

        m_ui = m_context->m_gui.make_container_panel();
        m_ui->insert(std::unique_ptr<dick::GUI::Widget> { container.release() });
        m_ui->align(
            {
                m_context->m_const.screen_w / 2.0,
                m_context->m_const.screen_h * 0.1
            },
            dick::GUI::Alignment::CENTER | dick::GUI::Alignment::TOP);
    }

public:
    HighScoreDisplayState(KulkiContext* context) :
        m_context { context },
        m_high_score { HighScore::load("high_score", m_context->m_const.highscore_max_entries) },
        m_ball_counts { m_high_score.get_ball_counts() },
        m_ball_index { 0 },
        m_period { 5.0 },
        m_timer { m_period }
    {
        std::sort(begin(m_ball_counts), end(m_ball_counts));
        m_rebuild_ui();
    }

    void on_key(dick::Key, bool down) override
    {
        if (down) {
            t_transition_required = true;
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (down) {
            m_ui->on_click(button);
            m_rebuild_ui();
            m_timer = m_period;
        }
    }

    void tick(double dt) override
    {
        m_timer -= dt;
        if (m_timer <= 0) {
            m_timer = m_period;
            m_ball_index = (m_ball_index + 1) % m_ball_counts.size();
            m_rebuild_ui();
        }
    }

    void draw(double) override
    {
        m_context->draw_veil();
        m_ui->on_draw();
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
