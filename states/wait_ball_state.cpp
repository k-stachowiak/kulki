// Copyright (C) 2015 Krzysztof Stachowiak

#include "wait_ball_state.h"
#include "kulki_context.h"

WaitBallState::WaitBallState(KulkiContext* context) :
    m_context { context },
    m_usure_phase { false }
{}

void WaitBallState::on_key(dick::Key key, bool down)
{
    if (!down) {
        return;
    }

    switch (key) {
    case dick::Key::ESCAPE:
        m_usure_phase = true;
        break;

    default:
        break;
    }
}

void WaitBallState::on_button(dick::Button, bool down)
{
    if (!down || t_transition_required) {
        return;
    }
    int tx = m_context->m_var.m_cursor_tile.first;
    int ty = m_context->m_var.m_cursor_tile.second;
    if (m_context->m_var.m_board.has(tx, ty) && m_context->m_var.m_board(tx, ty) != m_context->m_const.empty_field) {
        t_transition_required = true;
        m_next_state.reset(new WaitDestState { m_context, tx, ty });
    }
}


void WaitBallState::draw(double)
{
    const double button_padding = 15.0;

    m_context->m_gui.set_current_font(m_context->m_const.score_font);
    m_context->m_gui.transform_reset();

    if (m_usure_phase) {

        m_context->m_gui.transform_push_screen_align(
                dick::GUI::Alignment::MIDDLE | dick::GUI::Alignment::CENTER);
        m_context->m_gui.set_current_widget_alignment(
                dick::GUI::Alignment::BOTTOM |
                dick::GUI::Alignment::CENTER);
        m_context->m_gui.transform_push_shift({ 0.0, -5.0 });
        m_context->m_gui.label("Are you sure?");

        m_context->m_gui.set_current_widget_alignment(
                dick::GUI::Alignment::TOP |
                dick::GUI::Alignment::RIGHT);
        m_context->m_gui.transform_push_shift({ -5.0, 5.0 });
        m_context->m_gui.button_text(
                { button_padding, button_padding },
                [this](){
                    t_transition_required = true;
                    m_next_state = std::shared_ptr<StateNode> { new GameoverState { m_context } };
                },
                "Yes");
        m_context->m_gui.transform_pop();

        m_context->m_gui.set_current_widget_alignment(
                dick::GUI::Alignment::TOP |
                dick::GUI::Alignment::LEFT);
        m_context->m_gui.transform_push_shift({ +5.0, 5.0 });
        m_context->m_gui.button_text(
                { button_padding, button_padding },
                [this](){
                    m_usure_phase = false;
                },
                "No");
        m_context->m_gui.transform_pop();

    } else {
        m_context->m_gui.transform_push_screen_align(
                dick::GUI::Alignment::TOP | dick::GUI::Alignment::RIGHT);
        m_context->m_gui.set_current_widget_alignment(
                dick::GUI::Alignment::TOP |
                dick::GUI::Alignment::RIGHT);
        m_context->m_gui.transform_push_shift({ -5.0, +3.0 });
        m_context->m_gui.button_text(
                { button_padding, button_padding },
                [this](){ m_usure_phase = true; },
                "Give up");
    }
}

std::shared_ptr<dick::StateNode> WaitBallState::next_state()
{
    return std::move(m_next_state);
}
