#ifndef CONTEXT_H
#define CONTEXT_H

#include "kulki_constants.h"
#include "kulki_variables.h"
#include "dick.h"
#include "board.h"
#include "menu_state.h"
#include "deal_state.h"
#include "wait_ball_state.h"
#include "wait_dest_state.h"
#include "move_state.h"
#include "score_state.h"
#include "high_score_state.h"
#include "game_over_state.h"

struct KulkiContext : public dick::PlatformClient {

    dick::Resources m_resources;
    KulkiConstants m_const;
    KulkiVariables m_var;
    dick::StateMachine m_machine;

    KulkiContext(KulkiConfig &config);

    void gen_next_deal(int count);

    glm::mat3 current_transform();

    void draw_field(const glm::vec3& top_left, const glm::vec3& bot_right, bool fill, const glm::mat3& transf);
    void draw_ball(double x, double y, int color, double r, double squeeze, const glm::mat3& transf);
    void draw_board(const Board& b, const glm::mat3& transf);

    bool is_over() const override;
    void on_key(int key, bool down) override;
    void on_button(int button, bool down) override;
    void on_cursor(dick::DimScreen position) override;
    void tick(double dt) override;
    void draw(double weight) override;
};

#endif
