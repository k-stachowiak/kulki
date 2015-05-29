#include "kulki_context.h"

#include "kulki_state_deal.h"
#include "kulki_state_game_over.h"
#include "kulki_state_high_score.h"
#include "kulki_state_menu.h"
#include "kulki_state_move.h"
#include "kulki_state_score.h"
#include "kulki_state_wait_ball.h"
#include "kulki_state_wait_dest.h"

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
    m_cursor_tile { cursor_tile },
    m_score_font { score_font },
    m_gameover_font { gameover_font },
    m_menu_font { menu_font }
{}

void KulkiContext::set_state_menu()
{
    m_current_state.reset(new MenuState { m_board, this, m_menu_font, m_score, m_alive });
}

void KulkiContext::set_state_wait_ball()
{
    m_current_state.reset(new WaitBallState { m_board, this, m_cursor_tile });
}

void KulkiContext::reset_state_wait_ball(int src_x, int src_y, int color)
{
    (*m_board)(src_x, src_y) = color;
    set_state_wait_ball();
}

void KulkiContext::set_state_wait_dest(int src_x, int src_y)
{
    m_current_state.reset(new WaitDestState {
        m_board,
        this,
        m_cursor_tile,
        src_x,
        src_y,
        (*m_board)(src_x, src_y),
        0
    });
}

void KulkiContext::reset_state_wait_dest(int src_x, int src_y)
{
//wut...
    (*m_board)(m_wait_dest_state->get_src_x(), m_wait_dest_state->get_src_y()) = m_wait_dest_state->get_color();
    set_state_wait_dest(src_x, src_y);
}

void KulkiContext::set_state_deal(int count)
{
    if (m_board->free_fields() <= count) {
        set_state_high_score();
    } else {
        m_current_state.reset(new DealState { m_board, this, config::DEAL_PERIOD, count });
    }
}

void KulkiContext::set_state_gameover()
{
    m_current_state.reset(new GameoverState {
        this,
        m_gameover_state,
        m_score,
        config::GAMEOVER_PERIOD,
        0
    });
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

    m_current_state.reset(new ScoreState {
        this,
        m_score_font,
        config::SCORE_PERIOD,
        double(x_sum) / double(streaks.size()) + 0.5,
        double(y_sum) / double(streaks.size()) + 0.5,
        score_incr
    });
}

void KulkiContext::set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
{
    std::deque<std::pair<int, int>> path;

    if (!m_board->find_path({ src_x, src_y }, { dst_x, dst_y }, path)) {
        reset_state_wait_ball(src_x, src_y, color);

    } else {
        m_current_state.reset(new MoveState {
            m_board, this,
            std::move(path),
            config::MOVE_PERIOD,
            dst_x, dst_y,
            color
        });

    }
}

void KulkiContext::set_state_high_score()
{
    m_current_state.reset(new HighScoreState { this, m_menu_font, *m_score});
}

