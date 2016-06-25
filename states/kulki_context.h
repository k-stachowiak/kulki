// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef CONTEXT_H
#define CONTEXT_H

#include "kulki_constants.h"
#include "kulki_variables.h"
#include "dick.h"
#include "board.h"

struct KulkiContext : public dick::PlatformClient {

    dick::Resources m_resources;
    KulkiConstants m_const;
    KulkiVariables m_var;
    std::shared_ptr<dick::InputState> m_input_state;
    dick::GUI m_gui;
    dick::StateMachine m_machine;

    KulkiContext(KulkiConfig &config);

    glm::mat3 current_transform();

    std::unique_ptr<dick::GUI::Widget> make_score_label();
    std::unique_ptr<dick::GUI::Widget> make_giveup_button(dick::GUI::Callback on_click);
    std::unique_ptr<dick::GUI::Widget> make_giveup_dialog(
            dick::GUI::Callback on_yes,
            dick::GUI::Callback on_no);
    std::unique_ptr<dick::GUI::Widget> make_range_int_spin_ex(
            const std::string &name,
            int *value, int min, int max, void *font);
    std::unique_ptr<dick::GUI::Widget> make_range_int_spin(
            const std::string &name,
            int *value, int min, int max);

    void draw_field(
            const glm::vec3& top_left,
            const glm::vec3& bot_right,
            bool fill,
            const glm::mat3& transf);

    void draw_ball(
            double x,
            double y,
            int color_index,
            double r,
            double squeeze,
            const glm::mat3& transf);

    void draw_board(
            const Board& b,
            const glm::mat3& transf);

    void draw_veil();

    bool is_over() const override;
    void on_key(dick::Key key, bool down) override;
    void on_button(dick::Button button, bool down) override;
    void on_cursor(dick::DimScreen position) override;
    void tick(double dt) override;
    void draw(double weight) override;
};

std::shared_ptr<dick::StateNode> make_deal_state(
        KulkiContext *context,
        double time);

std::shared_ptr<dick::StateNode> make_gameover_state(
        KulkiContext *context);

std::shared_ptr<dick::StateNode> make_highscore_display_state(
        KulkiContext *context);

std::shared_ptr<dick::StateNode> make_highscore_input_state(
        KulkiContext *context);

std::shared_ptr<dick::StateNode> make_menu_state(
        KulkiContext *context);

std::shared_ptr<dick::StateNode> make_move_state(
        KulkiContext* context,
        std::deque<std::pair<int, int>> path,
        double time, int dst_x, int dst_y, int color);

std::shared_ptr<dick::StateNode> make_score_state(
        KulkiContext* context,
        const std::vector<std::pair<int, int>>& changes,
        bool next_deal);

std::shared_ptr<dick::StateNode> make_wait_ball_state(
        KulkiContext *context);

std::shared_ptr<dick::StateNode> make_wait_dest_state(
        KulkiContext *context,
        int src_x,
        int src_y);

std::shared_ptr<dick::StateNode> make_options_state(
        KulkiContext *context);
#endif
