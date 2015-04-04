// Copyright (C) 2015 Krzysztof Stachowiak

#include <sstream>

#include "kulki_states.h"

void draw_ball(double x, double y, int color, double r, const glm::mat3& transf);

std::pair<double, double> MenuState::m_compute_dimensions()
{
    const double height = al_get_font_line_height(m_menu_font);
    const int n = m_entries.size();

    double max_width = 0.0;
    for (const auto& entry : m_entries) {
        double width = al_get_text_width(m_menu_font, entry.c_str());
        if (width > max_width) {
            max_width = width;
        }
    }

    return {
        2.0 * (config::MENU_PADDING + config::MENU_MARGIN) + max_width,
        n * (height + 2.0 * config::MENU_PADDING + config::MENU_MARGIN) +
            config::MENU_MARGIN
    };
}

void MenuState::m_select()
{
    const std::string& entry = m_entries[m_current];

    if (entry == "New game") {
        m_board->clear();
        *m_score = 0;
        m_context->set_state_deal(config::DEAL_COUNT_INIT);

    } else if (entry == "Exit") {
        *m_alive = false;

    } else {
        throw std::runtime_error("Incogerent menu configuration");

    }
}

MenuState::MenuState(Board* board, KulkiContext* const context, ALLEGRO_FONT* menu_font, int* score, bool* alive) :
    m_board { board },
    m_context { context },
    m_menu_font { menu_font },
    m_score { score },
    m_alive { alive },
    m_entries { "New game", "Exit" },
    m_current { 0 }
{
    std::tie(m_width, m_height) = m_compute_dimensions();
}

void MenuState::on_key(int key, bool down)
{
    if (!down) {
        return;
    }

    switch (key) {
    case ALLEGRO_KEY_DOWN:
        m_current = (m_current - 1) % m_entries.size();
        break;
    case ALLEGRO_KEY_UP:
        m_current = (m_current + 1) % m_entries.size();
        break;
    case ALLEGRO_KEY_ENTER:
        m_select();
        break;
    default:
        break;
    }
}

void MenuState::draw(const glm::mat3&)
{
    const double height = al_get_font_line_height(m_menu_font);
    const double x = double(config::SCREEN_W - m_width) / 2.0;
    double y = double(config::SCREEN_H - m_height) / 2.0;

    for (decltype(m_entries)::size_type i = 0; i < m_entries.size(); ++i) {

        const std::string& entry = m_entries[i];

        const double x1 = x + config::MENU_MARGIN;
        const double y1 = y + config::MENU_MARGIN;
        const double x2 = x + m_width - config::MENU_MARGIN;
        const double y2 = y + config::MENU_MARGIN + height + 2.0 * config::MENU_PADDING;
        al_draw_filled_rectangle(x1, y1, x2, y2, config::MENU_BG_COLOR_L);
        al_draw_rectangle(x1, y1, x2, y2, config::MENU_BG_COLOR_D, 3);

        const double cx = x + m_width / 2.0;
        const double cy = y + config::MENU_MARGIN + config::MENU_PADDING + height / 2.0;
        const auto color = (i == m_current) ? config::MENU_SELECT_COLOR : config::MENU_REGULAR_COLOR;
        al_draw_textf(m_menu_font, color, cx, cy - height / 2, ALLEGRO_ALIGN_CENTRE, "%s", entry.c_str());

        y += height + 2.0 * config::MENU_PADDING + config::MENU_MARGIN;
    }
}

void DealState::m_new_ball(Board& b, int& x, int& y, int& color)
{
    std::uniform_int_distribution<int> distr_x(0, b.m_width - 1);
    std::uniform_int_distribution<int> distr_y(0, b.m_height - 1);
    std::uniform_int_distribution<int> distr_color(0, config::BALL_COLORS.size() - 1);

    do {
        x = distr_x(m_reng);
        y = distr_y(m_reng);
    } while (b(x, y) != config::EMPTY);

    color = distr_color(m_reng);
}

DealState::DealState(Board* board, KulkiContext* context) :
    m_reng { m_rdev() },
    m_board { board },
    m_context { context }
{}

void DealState::reset(double time, int count)
{
    m_time = time;
    m_count = count;
    m_positions.clear();
}

void DealState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    else m_time = config::DEAL_PERIOD;

    int x, y, color;
    m_new_ball(*m_board, x, y, color);
    (*m_board)(x, y) = color;
    m_positions.emplace_back(x, y);

    if ((--m_count) == 0) {
        m_context->set_state_score(m_positions, KulkiState::Enum::WAIT_BALL);
    }
}

WaitBallState::WaitBallState(Board* board, KulkiContext* context, std::pair<int, int> *cursor_tile) :
    m_board { board },
    m_context { context },
    m_cursor_tile { cursor_tile }
{}

void WaitBallState::on_button(int button, bool down)
{
    int tx = m_cursor_tile->first;
    int ty = m_cursor_tile->second;
    if ((*m_board)(tx, ty) != config::EMPTY) {
        m_context->set_state_wait_dest(tx, ty);
    }
}

WaitDestState::WaitDestState(Board* board, KulkiContext* context, std::pair<int, int>* cursor_tile) :
    m_board { board },
    m_context { context },
    m_cursor_tile { cursor_tile }
{}

void WaitDestState::reset(int src_x, int src_y, int color, double time)
{
    m_src_x = src_x;
    m_src_y = src_y;
    m_color = color;
    m_time = time;
}

void WaitDestState::tick(double dt)
{
    m_time += dt;
    m_time = fmod(m_time, config::MOVE_PERIOD);
}

void WaitDestState::draw(const glm::mat3& transf)
{
    double factor = double(m_time) / config::MOVE_PERIOD * 3.14;
    double x = double(m_src_x) + 0.5;
    double y = double(m_src_y) + 0.5 - sin(factor) * config::BALL_JUMP_H;
    draw_ball(x, y, m_color, config::BALL_RADIUS, transf);
}

void WaitDestState::on_button(int button, bool down)
{
    int tx = m_cursor_tile->first;
    int ty = m_cursor_tile->second;

    if (tx == m_src_x && ty == m_src_y)
        m_context->reset_state_wait_ball(m_src_x, m_src_y, m_color);

    else if ((*m_board)(tx, ty) != config::EMPTY)
        m_context->reset_state_wait_dest(tx, ty);

    else
        m_context->set_state_move(m_src_x, m_src_y, tx, ty, m_color);
}

MoveState::MoveState(Board* board, KulkiContext* context) :
    m_board { board },
    m_context { context }
{}

void MoveState::reset(std::deque<std::pair<int, int>> path, double time, int dst_x, int dst_y, int color)
{
    m_path = std::move(path);
    m_time = time;
    m_dst_x = dst_x;
    m_dst_y = dst_y;
    m_color = color;
}

void MoveState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    else m_time = config::MOVE_PERIOD;

    m_path.pop_front();

    if (m_path.size() == 1) {
        (*m_board)(m_dst_x, m_dst_y) = m_color;
        m_context->set_state_score({ { m_dst_x, m_dst_y } }, KulkiState::Enum::DEAL);
    }
}

void MoveState::draw(const glm::mat3& transf)
{
    double x1 = m_path[0].first;
    double y1 = m_path[0].second;
    double x2 = m_path[1].first;
    double y2 = m_path[1].second;

    double mv_factor = m_time / config::MOVE_PERIOD;
    double bmp_factor = (1.0 - mv_factor) * 3.14;

    double x = x1 * mv_factor + x2 * (1.0 - mv_factor) + 0.5;
    double y = y1 * mv_factor + y2 * (1.0 - mv_factor) + 0.5 - sin(bmp_factor) * config::BALL_JUMP_H;

    draw_ball(x, y, m_color, config::BALL_RADIUS, transf);
}

ScoreState::ScoreState(KulkiContext* context, ALLEGRO_FONT* score_font) :
    m_context { context },
    m_score_font { score_font }
{}

void ScoreState::reset(double time, double cx, double cy, int incr)
{
    m_time = time;
    m_cx = cx;
    m_cy = cy;
    m_incr = incr;
}

void ScoreState::draw(const glm::mat3& transf)
{
    glm::vec3 text_center = glm::vec3 { m_cx, m_cy, 1 } * transf;
    al_draw_textf(
        m_score_font, config::SCORE_COLOR,
        text_center.x, text_center.y,
        ALLEGRO_ALIGN_CENTRE,
        "+%d", m_incr);
}

void ScoreState::tick(double dt)
{
    if ((m_time -= dt) > 0) return;
    m_context->set_state_wait_ball();
}

std::vector<std::string> HighScoreState::m_gen_outries(const std::vector<HighScore::Entry>& entries)
{
    std::vector<std::string> result;
    for (const auto& e : m_high_score->get_entries()) {
        std::stringstream ss;
        int padding = config::HIGHSCORE_CHARACTERS - e.str_len();
        ss << e.name << std::string(padding, '.') << e.score;
        result.push_back(ss.str());
    }
    return result;
}

HighScoreState::HighScoreState(KulkiContext* context, ALLEGRO_FONT* menu_font) :
    m_context { context },
    m_menu_font { menu_font }
{}

void HighScoreState::reset(int score)
{
    m_score = score;
    m_high_score.reset(new HighScore { "hs" });
    if (m_high_score->insert_position(m_score) != -1) {
        std::cout << "insert position = " << m_high_score->insert_position(m_score) << std::endl;
        m_phase = HIGH_SCORE_INPUT;
        m_input_name.clear();
    } else {
        m_phase = HIGH_SCORE_DISPLAY;
    }
}

void HighScoreState::on_key(int key, bool down)
{
    if (!down) {
        return;
    }

    switch (m_phase) {
    case HIGH_SCORE_INPUT:
        if ((key >= ALLEGRO_KEY_A && key <= ALLEGRO_KEY_Z) || key == ALLEGRO_KEY_SPACE) {
            m_input_name.push_back(key + 'A' - 1);
        } else if (key == ALLEGRO_KEY_BACKSPACE) {
            m_input_name.pop_back();
        } else if (key == ALLEGRO_KEY_ENTER) {
            m_high_score->add_entry(HighScore::Entry { m_input_name, m_score });
            m_phase = HIGH_SCORE_DISPLAY;
        } else if (key == ALLEGRO_KEY_ESCAPE) {
            m_phase = HIGH_SCORE_DISPLAY;
        }
        break;

    case HIGH_SCORE_DISPLAY:
        m_high_score.reset();
        m_context->set_state_gameover();
        break;
    }
}

void HighScoreState::draw(const glm::mat3&)
{
    const double text_height = al_get_font_line_height(m_menu_font);
    const double line_height = text_height + 20;
    const std::vector<std::string> outries = m_gen_outries(m_high_score->get_entries());
    double y = config::SCREEN_H / 2 - (outries.size() / 2) * line_height;
    for (decltype(outries)::size_type i = 0; i < outries.size(); ++i) {
        if (m_phase == HIGH_SCORE_INPUT && i == (unsigned)m_high_score->insert_position(m_score)) {
            al_draw_text(m_menu_font, al_map_rgb_f(1, 1, 1), config::SCREEN_W / 2, y, ALLEGRO_ALIGN_CENTRE, m_input_name.c_str());
        } else {
            const auto& outry = outries.at(i);
            al_draw_text(m_menu_font, al_map_rgb_f(1, 1, 1), config::SCREEN_W / 2, y, ALLEGRO_ALIGN_CENTRE, outry.c_str());
        }
        y += line_height;
    }
}

GameoverState::GameoverState(KulkiContext* context, ALLEGRO_FONT* gameover_font) :
    m_context { context },
    m_gameover_font { gameover_font }
{}

void GameoverState::reset(int *score, double time, int index)
{
    m_score = score;
    m_time = time;
    m_index = index;
}

void GameoverState::tick(double dt)
{
    if ((m_time -= dt) > 0) {
        return;
    }

    m_time = config::GAMEOVER_PERIOD;
    m_index = (m_index + 1) % config::BALL_COLORS.size();
}

void GameoverState::draw(const glm::mat3& transf)
{
    al_draw_text(
        m_gameover_font, config::BALL_COLORS[m_index],
        config::GAMEOVER_SHIFT_X, config::GAMEOVER_SHIFT_Y,
        ALLEGRO_ALIGN_CENTRE,
        "Game Over");
}

void GameoverState::on_key(int key, bool down)
{
    if (down) {
        m_context->set_state_menu();
    }
}

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

