#include "kulki_context.h"

#include "deal_state.h"
#include "game_over_state.h"
#include "high_score_state.h"
#include "menu_state.h"
#include "move_state.h"
#include "score_state.h"
#include "wait_ball_state.h"
#include "wait_dest_state.h"

KulkiContext::KulkiContext(
        Board* board,
        int* score,
        bool* alive,
        std::pair<int, int>* cursor_tile,
        ALLEGRO_FONT* score_font,
        ALLEGRO_FONT* gameover_font,
        ALLEGRO_FONT* menu_font) :
    m_board { board },
    m_score { score },
    m_menu_state { board, this, menu_font, score, alive },
    m_deal_state { board, this },
    m_wait_ball_state { board, this, cursor_tile },
    m_wait_dest_state { board, this, cursor_tile },
    m_move_state { board, this },
    m_score_state { this, score_font },
    m_gameover_state { this, gameover_font },
    m_high_score_state { this, menu_font },
    m_current_state { nullptr }
{}

void KulkiContext::set_state_menu()
{
    m_menu_state.reset();
    m_current_state = &m_menu_state;
}

void KulkiContext::set_state_wait_ball()
{
    m_current_state = &m_wait_ball_state;
}

void KulkiContext::reset_state_wait_ball(int src_x, int src_y, int color)
{
    (*m_board)(src_x, src_y) = color;
    set_state_wait_ball();
}

void KulkiContext::set_state_wait_dest(int src_x, int src_y)
{
    m_wait_dest_state.reset(src_x, src_y, (*m_board)(src_x, src_y), 0);
    (*m_board)(src_x, src_y) = config::EMPTY;
    m_current_state = &m_wait_dest_state;
}

void KulkiContext::reset_state_wait_dest(int src_x, int src_y)
{
    (*m_board)(m_wait_dest_state.get_src_x(), m_wait_dest_state.get_src_y()) = m_wait_dest_state.get_color();
    set_state_wait_dest(src_x, src_y);
}

void KulkiContext::set_state_deal(int count)
{
    if (m_board->free_fields() <= count) {
        set_state_high_score();
        return;
    }

    m_deal_state.reset(config::DEAL_PERIOD, count);
    m_current_state = &m_deal_state;
}

void KulkiContext::set_state_gameover()
{
    m_gameover_state.reset(m_score, config::GAMEOVER_PERIOD, 0);
    m_current_state = &m_gameover_state;
}

void KulkiContext::set_state_score(const std::vector<std::pair<int, int>>& changes, KulkiState::Enum next_state)
{
    std::unordered_set<std::pair<int, int>> streaks;
    bool success = false;

    for (const auto& p : changes) {
        success |= m_board->find_streak(p, std::inserter(streaks, begin(streaks)));
    }

    if (!success) {
        switch (next_state) {
        case KulkiState::Enum::DEAL:
            set_state_deal(config::DEAL_COUNT_INGAME);
            return;
        case KulkiState::Enum::WAIT_BALL:
            set_state_wait_ball();
            return;
        default:
            throw std::domain_error("m_set_state_score: bad next_state argument");
        }
    }

    int score_incr = 0;
    int x_sum = 0, y_sum = 0;
    for (const auto& p : streaks) {
        (*m_board)(p.first, p.second) = config::EMPTY;
        ++score_incr;
        x_sum += p.first;
        y_sum += p.second;
    }
    (*m_score) += score_incr;

    m_score_state.reset(
            config::SCORE_PERIOD,
            double(x_sum) / double(streaks.size()) + 0.5,
            double(y_sum) / double(streaks.size()) + 0.5,
            score_incr);

    m_current_state = &m_score_state;
}

void KulkiContext::set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
{
    std::deque<std::pair<int, int>> path;

    if (!m_board->find_path({ src_x, src_y }, { dst_x, dst_y }, path)) {
        reset_state_wait_ball(src_x, src_y, color);
        return;
    }

    m_move_state.reset(std::move(path), config::MOVE_PERIOD, dst_x, dst_y, color);
    m_current_state = &m_move_state;
}

void KulkiContext::set_state_high_score()
{
    m_high_score_state.reset(*m_score);
    m_current_state = &m_high_score_state;
}
