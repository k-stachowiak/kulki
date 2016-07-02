// Copyright (C) 2015 Krzysztof Stachowiak

#include "kulki_context.h"
#include "dick.h"

class ScoreState : public dick::StateNode {

    KulkiContext* const m_context;

    double m_time;
    double m_cx, m_cy;
    int m_incr;
    bool m_next_deal;
    std::shared_ptr<StateNode> m_next_state;

    bool m_usure_phase;
    std::unique_ptr<dick::GUI::Widget> m_score_label;
    std::unique_ptr<dick::GUI::Widget> m_usure_dialog;
    std::unique_ptr<dick::GUI::Widget> m_giveup_button;

public:
    ScoreState(KulkiContext* context, const std::vector<std::pair<int, int>>& changes, bool next_deal) :
        m_context { context },
        m_next_deal { next_deal },
        m_usure_phase { false }
    {
        m_score_label = m_context->make_score_label();
        m_giveup_button = m_context->make_giveup_button(
            [this]()
            {
                m_usure_phase = true;
            });

        m_usure_dialog = m_context->make_giveup_dialog(
            [this]()
            {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            },
            [this]()
            {
                m_usure_phase = false;
            });

        std::unordered_set<std::pair<int, int>> scored;
        bool success = false;

        for (const auto& change : changes) {
            success |= m_context->m_var.m_board.find_streak(
                    change,
                    std::inserter(scored, begin(scored)),
                    m_context->m_const.score_serie_min);
        }

        if (!success) {
            m_context->m_var.m_streak = 0;
            m_time = 0;
            return;
        }

        ++m_context->m_var.m_streak;
        m_time = m_context->m_const.score_period;

        int x_sum = 0, y_sum = 0;
        for (const auto& p : scored) {
            m_context->m_var.m_board(p.first, p.second) = m_context->m_const.empty_field;
            x_sum += p.first;
            y_sum += p.second;
        }

        int score_incr = scored.size() * m_context->m_var.m_streak;
        m_context->m_var.m_score += score_incr;

        // Setup state for further phase.
        m_next_deal = false;
        m_cx = double(x_sum) / double(scored.size()) + 0.5;
        m_cy = double(y_sum) / double(scored.size()) + 0.5;
        m_incr = score_incr;

    }

    void on_key(dick::Key key, bool down) override
    {
        if (!down) {
            return;
        }

        if (m_usure_phase) {
            if (key == dick::Key::ENTER) {
                t_transition_required = true;
                m_next_state = make_gameover_state(m_context);
            } else if (key == dick::Key::ESCAPE) {
                m_usure_phase = false;
            }
        } else {
            if (key == dick::Key::ESCAPE) {
                m_usure_phase = true;
            }
        }
    }

    void on_button(dick::Button button, bool down) override
    {
        if (!down) {
            return;
        }

        m_giveup_button->on_click(button);
        if (m_usure_phase) {
            m_usure_dialog->on_click(button);
        }
    }

    void draw(double) override
    {
        if (m_time > 0) {
            glm::vec3 text_center = glm::vec3 { m_cx, m_cy, 1 } * m_context->current_transform();
            al_draw_textf(
                m_context->m_const.score_font,
                al_map_rgb_f(
                    m_context->m_const.score_color.r,
                    m_context->m_const.score_color.g,
                    m_context->m_const.score_color.b),
                text_center.x, text_center.y,
                ALLEGRO_ALIGN_CENTRE,
                "+%d", m_incr);
        }

        if (m_usure_phase) {
            m_context->draw_veil();
        }

        m_score_label->on_draw();
        m_giveup_button->on_draw();
        if (m_usure_phase) {
            m_usure_dialog->on_draw();
        }
    }

    void tick(double dt) override
    {
        if (m_usure_phase) {
            // nop
        } else {
            if ((m_time -= dt) > 0)
                return;

            if (m_next_deal) {
                t_transition_required = true;
                if (m_context->m_var.m_board.free_fields() < m_context->m_const.deal_count_ingame) {
                    m_next_state = make_gameover_state(m_context);
                } else {
                    m_next_state = make_deal_state(m_context, m_context->m_const.deal_period);
                }

            } else {
                t_transition_required = true;
                if (m_context->m_var.m_board.free_fields() == 0) {
                    m_next_state = make_gameover_state(m_context);
                } else {
                    m_next_state = make_wait_ball_state(m_context);
                }
            }
        }
    }

    std::shared_ptr<dick::StateNode> next_state() override
    {
        return std::move(m_next_state);
    }

};

std::shared_ptr<dick::StateNode> make_score_state(
        KulkiContext* context,
        const std::vector<std::pair<int, int>>& changes,
        bool next_deal)
{
    return std::make_shared<ScoreState>(context, changes, next_deal);
}
