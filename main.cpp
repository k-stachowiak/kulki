#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <random>
#include <deque>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

#include <glm/glm.hpp>

namespace {
    const int SCREEN_W = 800;
    const int SCREEN_H = 800;
    const double FPS = 50.0;

    const double FRAME_REST = 0.001;
    const double MAX_FRAME_TIME = 0.25;

    const ALLEGRO_COLOR BG_COLOR = al_map_rgb_f(0.5, 0.5, 0.5);

    const int EMPTY = -1;

    const ALLEGRO_COLOR FIELD_COLOR = al_map_rgb_f(1, 1, 0.5);
    const double FIELD_W = 64;
    const double FIELD_THICK = 2;
    const double FIELD_MARGIN = 0.025;

    const int BOARD_W = 9, BOARD_H = 9;
    const double BOARD_SHIFT_X = (SCREEN_W - (BOARD_W * FIELD_W)) / 2.0;
    const double BOARD_SHIFT_Y = (SCREEN_H - (BOARD_H * FIELD_W)) / 2.0;

    const double BALL_THICK = 4;
    const double BALL_RADIUS = 0.8 * (FIELD_W / 2.0);
    const std::array<ALLEGRO_COLOR, 8> BALL_COLORS {
        al_map_rgb_f(0, 0, 0),
        al_map_rgb_f(0, 0, 1),
        al_map_rgb_f(0, 1, 0),
        al_map_rgb_f(0, 1, 1),
        al_map_rgb_f(1, 0, 0),
        al_map_rgb_f(1, 0, 1),
        al_map_rgb_f(1, 1, 0),
        al_map_rgb_f(1, 1, 1)
    };

    const double DEAL_PERIOD = 0.25;
    const int DEAL_COUNT_INIT = 7, DEAL_COUNT_INGAME = 3;

    const double MOVE_PERIOD = 0.25;

    const ALLEGRO_COLOR GAMEOVER_COLOR = al_map_rgb_f(1, 0, 0);
    const int GAMEOVER_SHIFT_X = SCREEN_W / 2, GAMEOVER_SHIFT_Y = SCREEN_H / 2;
    const int GAMEOVER_FONT_SIZE = 64;
}

namespace std {
    template <> struct hash<std::pair<int, int>> {
        size_t operator()(const std::pair<int, int> &v) const {
            std::hash<int> int_hasher;
            return int_hasher(v.first) ^ int_hasher(v.second);
        }
    };
}

class Allegro {

    const int m_screen_w, m_screen_h;
    const double m_fps, m_spf;
    double m_current_time, m_accumulator;

    ALLEGRO_DISPLAY* m_display;
    ALLEGRO_EVENT_QUEUE* m_ev_queue;

    Allegro(const Allegro&);    // Not copyable ...
    Allegro(Allegro&&);         // ... or moveable.

    template <typename KeyEvent, typename ButtonEvent, typename CursorEvent>
    void m_process_events(
            KeyEvent key,
            ButtonEvent button,
            CursorEvent cursor,
            bool& alive)
    {
        ALLEGRO_EVENT event;

        while (!al_is_event_queue_empty(m_ev_queue)) {
            al_get_next_event(m_ev_queue, &event);
            switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                alive = false;
                return;

            case ALLEGRO_EVENT_KEY_DOWN:
                key(event.keyboard.keycode, true);
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key(event.keyboard.keycode, false);
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                button(event.mouse.button, true);
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                button(event.mouse.button, false);
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                cursor(event.mouse.x, event.mouse.y);
                break;

            default:
                break;
            }
        }
    }

    template <typename TickEvent, typename DrawEvent>
    void m_realtime_loop_step(TickEvent tick, DrawEvent draw, bool& alive)
    {
        const double new_time = al_get_time();
        
        double frame_time = new_time - m_current_time;

        if (frame_time > MAX_FRAME_TIME) {
            frame_time = MAX_FRAME_TIME;
        }

        m_current_time = new_time;
        m_accumulator += frame_time;

        while (m_accumulator >= m_spf) {
            tick(m_spf);
            if (!alive) return;
            else m_accumulator -= m_spf;
        }

        const double frame_weight = m_accumulator / m_spf;
        draw(frame_weight);
    }

public:
    Allegro() :
        m_screen_w { SCREEN_W }, m_screen_h { SCREEN_H },
        m_fps { FPS }, m_spf { 1.0 / m_fps },
        m_display { nullptr }, m_ev_queue { nullptr }
    {
        if (!al_init()) {
            std::cerr << "Failed initializing platform wrapper." << std::endl;
            exit(1);
        }

        if (!al_init_image_addon()) {
            std::cerr << "Failed initializing image addon." << std::endl;
            exit(1);
        }

        al_init_font_addon();
        if (!al_init_ttf_addon()) {
            std::cerr << "Failed initializing TTF addon." << std::endl;
            exit(1);
        }

        if(!al_init_acodec_addon()) {
            std::cerr << "Failed initializing acodec add-on." << std::endl;
            exit(1);
        }

        if(!al_init_primitives_addon()) {
            std::cerr << "Failed initializing primitives add-on." << std::endl;
            exit(1);
        }

        m_display = al_create_display(m_screen_w, m_screen_h);
        if (!m_display) {
            std::cerr << "Failed creating display." << std::endl;
            exit(1);
        }

        if (!al_install_keyboard()) {
            std::cerr << "Failed installing keyboard." << std::endl;
            exit(1);
        }

        if (!al_install_mouse()) {
            std::cerr << "Failed installing mouse." << std::endl;
            exit(1);
        }

        if(!al_install_audio()) {
            std::cerr << "Failed initializing audio." << std::endl;
            exit(1);
        }

        m_ev_queue = al_create_event_queue();
        if (!m_ev_queue) {
            std::cerr << "Failed creating event queue." << std::endl;
            exit(1);
        }

        al_register_event_source(m_ev_queue, al_get_display_event_source(m_display));
        al_register_event_source(m_ev_queue, al_get_keyboard_event_source());
        al_register_event_source(m_ev_queue, al_get_mouse_event_source());
    }

    ~Allegro()
    {
        al_destroy_event_queue(m_ev_queue);
        al_destroy_display(m_display);
    }

    template <
        typename KeyEvent,
        typename ButtonEvent,
        typename CursorEvent,
        typename TickEvent,
        typename DrawEvent>
    void realtime_loop(
            KeyEvent key,
            ButtonEvent button,
            CursorEvent cursor,
            TickEvent tick,
            DrawEvent draw,
            bool& alive)
    {
        m_current_time = al_get_time();
        m_accumulator = 0;

        while (true) {
            m_process_events(key, button, cursor, alive);
            if (!alive) return;

            m_realtime_loop_step(tick, draw, alive);
            if (!alive) return;

            al_rest(FRAME_REST);
        }
    }
    
};

struct Board {

    int m_width, m_height;
    std::vector<int> m_fields;

    // Semi-regular:
    Board() : m_width { 0 }, m_height { 0 } {};
    Board(const Board& x) : m_width { x.m_width }, m_height { x.m_height }, m_fields { x.m_fields } {}
    Board(Board&& x) : m_width { x.m_width }, m_height { x.m_height }, m_fields { std::move(x.m_fields) } {}
    Board& operator=(const Board& x)
    {
        m_width = x.m_width;
        m_height = x.m_height;
        m_fields = x.m_fields;
        return *this;
    }
    Board& operator=(Board&& x)
    {
        m_width = x.m_width;
        m_height = x.m_height;
        m_fields = std::move(x.m_fields);
        return *this;
    }

    // Custom constructor:
    Board(int w, int h) :
        m_width { w }, m_height { h },
        m_fields(w * h, EMPTY)
    {}

    // API:
    void clear()
    {
        std::fill(begin(m_fields), end(m_fields), EMPTY);
    }

    bool has(int x, int y) const
    {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    int &operator()(int x, int y)
    {
        return m_fields[y * m_width + x];
    }

    int operator()(int x, int y) const
    {
        return const_cast<Board&>(*this)(x, y);
    }
};

int free_fields(const Board& b)
{
    return std::count(begin(b.m_fields), end(b.m_fields), EMPTY);
}

bool find_path(
        const Board& b,
        std::pair<int, int> src, std::pair<int, int> dst,
        std::deque<std::pair<int, int>>& path)
{
    std::unordered_set<std::pair<int, int>> open;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>> preds;
    std::unordered_map<std::pair<int, int>, int> dists;

    for (int x = 0; x < b.m_width; ++x) {
        for (int y = 0; y < b.m_height; ++y) {
            dists[std::pair<int, int>(x, y)] = std::numeric_limits<int>::max();
        }
    }

    open.insert(src);
    dists[src] = 0;

    while (!open.empty()) {

        auto it = std::min_element(begin(open), end(open),
            [&dists](const std::pair<int, int>& x, const std::pair<int, int>& y) {
                return dists[x] < dists[y];
            });

        std::pair<int, int> u = *it;
        if (u == dst) {
            break;
        }

        std::vector<std::pair<int, int>> neighbors {
            { u.first - 1, u.second },
            { u.first + 1, u.second },
            { u.first, u.second - 1 },
            { u.first, u.second + 1 },
        };

        int new_dist = dists[u] + 1;
        for (const auto& v : neighbors) {
            if (b(v.first, v.second) != EMPTY) {
                continue;
            }
            if (new_dist < dists[v]) {
                dists[v] = new_dist;
                preds[v] = u;
                open.insert(v);
            }
        }

        open.erase(u);
    }

    if (dists[dst] == std::numeric_limits<int>::max()) {
        return false;
    } else {
        std::pair<int, int> n = dst;
        while (n != src) {
            path.push_front(n);
            n = preds[n];
        }
        path.push_front(n);
        return true;
    }
}

glm::mat3 translate(double x, double y)
{
    return {
        1, 0, x,
        0, 1, y,
        0, 0, 1
    };
}

glm::mat3 scale(double factor)
{
    return {
        factor, 0, 0,
        0, factor, 0,
        0, 0, 1
    };
}

// Drawing details.
// ----------------

void draw_field(
        const glm::vec3& top_left,
        const glm::vec3& bot_right,
        const glm::mat3& transf)
{
    glm::vec3 out_top_left = top_left * transf;
    glm::vec3 out_bot_right = bot_right * transf;
    double x1 = out_top_left.x, y1 = out_top_left.y;
    double x2 = out_bot_right.x, y2 = out_bot_right.y;
    al_draw_rectangle(x1, y1, x2, y2, FIELD_COLOR, FIELD_THICK);
}

void draw_ball(
        double x, double y, int color, double r,
        bool fill, const glm::mat3& transf)
{
    glm::vec3 c = glm::vec3 { x, y, 1 } * transf;

    if (fill) {
        al_draw_filled_circle(c.x, c.y, r, BALL_COLORS[color]);
    } else {
        al_draw_circle(c.x, c.y, r, BALL_COLORS[color], BALL_THICK);
    }
}

void draw_board(const Board& b, int hlx, int hly, const glm::mat3& transf)
{
    for (int x = 0; x < b.m_width; ++x) {
        for (int y = 0; y < b.m_height; ++y) {
            if (x != hlx || y != hly) {
                draw_field(
                    glm::vec3 { x + FIELD_MARGIN, y + FIELD_MARGIN, 1.0 },
                    glm::vec3 { x + 1.0 - FIELD_MARGIN, y + 1.0 - FIELD_MARGIN, 1.0 },
                    transf);
            }
            if (b(x, y) != EMPTY) {
                draw_ball(
                    x + 0.5, y + 0.5, b(x, y), BALL_RADIUS,
                    true, transf);
            }
        }
    }
}

enum KulkiState {
    DEAL,
    WAIT_BALL,
    WAIT_DEST,
    MOVE,
    GAMEOVER
};

class Kulki {

    std::default_random_engine m_reng;

    bool m_alive;
    Board m_board;
    ALLEGRO_FONT* m_font;

    std::pair<int, int> m_cursor_screen;
    std::pair<int, int> m_cursor_tile;

    KulkiState m_state;

    int m_deal_count;
    double m_deal_time;

    int m_waitd_src_x, m_waitd_src_y;
    int m_waitd_color;

    std::deque<std::pair<int, int>> m_move_path;
    double m_move_time;
    int m_move_dst_x, m_move_dst_y;
    int m_move_color;

    Kulki(const Kulki&);    // Not copyable ...
    Kulki(Kulki&&);         // ... or moveable.

    // Helpers.
    // --------

    glm::mat3 m_current_transform()
    {
        return scale(FIELD_W) * translate(BOARD_SHIFT_X, BOARD_SHIFT_Y); 
    }

    void m_new_ball(int& x, int& y, int& color)
    {
        std::uniform_int_distribution<int> distr_x(0, m_board.m_width - 1);
        std::uniform_int_distribution<int> distr_y(0, m_board.m_height - 1);
        std::uniform_int_distribution<int> distr_color(0, BALL_COLORS.size() - 1);

        do {
            x = distr_x(m_reng);
            y = distr_y(m_reng);
        } while (m_board(x, y) != EMPTY);

        color = distr_color(m_reng);
    }

    // State transitions.
    // ------------------

    void m_set_state_wait_ball()
    {
        m_state = WAIT_BALL;
    }

    void m_reset_state_wait_ball(int src_x, int src_y, int color)
    {
        m_board(src_x, src_y) = color;
        m_state = WAIT_BALL;
    }

    void m_set_state_wait_dest(int src_x, int src_y)
    {
        m_waitd_src_x = src_x;
        m_waitd_src_y = src_y;
        m_waitd_color = m_board(src_x, src_y);
        m_board(src_x, src_y) = EMPTY;
        m_state = WAIT_DEST;
    }

    void m_set_state_deal(int count)
    {
        if (free_fields(m_board) <= count) {
            m_set_state_gameover();
            return;
        }

        m_deal_count = count;
        m_deal_time = DEAL_PERIOD;
        m_state = DEAL;
    }

    void m_set_state_gameover()
    {
        m_state = GAMEOVER;
    }

    void m_set_state_move(int src_x, int src_y, int dst_x, int dst_y, int color)
    {
        std::deque<std::pair<int, int>> path;

        if (!find_path(m_board, { src_x, src_y }, { dst_x, dst_y }, path)) {
            m_reset_state_wait_ball(src_x, src_y, color);
            return;
        }

        m_move_path = std::move(path);
        m_move_time = MOVE_PERIOD;
        m_move_dst_x = dst_x;
        m_move_dst_y = dst_y;
        m_move_color = color;
        m_state = MOVE;
    }

    // Tick implementations.
    // ---------------------

    void m_tick_deal(double dt)
    {
        if ((m_deal_time -= dt) > 0) return;
        else m_deal_time = DEAL_PERIOD;

        int x, y, color;
        m_new_ball(x, y, color);
        m_board(x, y) = color;

        if ((--m_deal_count) == 0) {
            m_set_state_wait_ball();
        }
    }

    void m_tick_move(double dt)
    {
        if ((m_move_time -= dt) > 0) return;
        else m_move_time = MOVE_PERIOD;

        m_move_path.pop_front();

        if (m_move_path.empty()) {
            m_board(m_move_dst_x, m_move_dst_y) = m_move_color;
            m_set_state_deal(DEAL_COUNT_INGAME);
        }
    }

    // Draw implementations.
    // ---------------------

    void m_draw_wait_dest()
    {
        glm::mat3 transf = m_current_transform();
        int x = m_waitd_src_x;
        int y = m_waitd_src_y;
        draw_ball(x + 0.5, y + 0.5, m_waitd_color, BALL_RADIUS, false, transf);
    }

    void m_draw_move()
    {
        glm::mat3 transf = m_current_transform();
        int x = m_move_path.front().first;
        int y = m_move_path.front().second;
        draw_ball(x + 0.5, y + 0.5, m_move_color, BALL_RADIUS, false, transf);
    }

    void m_draw_gameover()
    {
        al_draw_text(
            m_font, GAMEOVER_COLOR,
            GAMEOVER_SHIFT_X, GAMEOVER_SHIFT_Y,
            ALLEGRO_ALIGN_CENTRE,
            "Game Over");
    }

    // Common implementations.
    // -----------------------

    void m_on_key(int key, bool down)
    {
        if (key == ALLEGRO_KEY_ESCAPE && down)
        {
            m_alive = false;
            return;
        }

        switch (m_state)
        {
        case GAMEOVER:
            m_board.clear();
            m_set_state_deal(DEAL_COUNT_INIT);
            break;
        default:
            break;
        }
    }

    void m_on_button(int button, bool down)
    {
        if (!down) return;

        int tx = m_cursor_tile.first;
        int ty = m_cursor_tile.second;

        switch (m_state) {
        case WAIT_BALL:
            if (m_board.has(tx, ty)) {
                m_set_state_wait_dest(tx, ty);
            }
            break;
        case WAIT_DEST:
            if (tx == m_waitd_src_x && ty == m_waitd_src_y) {
                m_reset_state_wait_ball(m_waitd_src_x, m_waitd_src_y, m_waitd_color);
            } else {
                m_set_state_move(m_waitd_src_x, m_waitd_src_y, tx, ty, m_waitd_color);
            }
            break;
        default:
            break;
        }
    }

    void m_on_cursor(int x, int y)
    {
        m_cursor_screen.first = x;
        m_cursor_screen.second = y;

        glm::mat3 inv = glm::inverse(m_current_transform());
        glm::vec3 screen_pos { x, y, 1 };
        glm::vec3 tile_pos = screen_pos * inv;

        m_cursor_tile.first = tile_pos.x;
        m_cursor_tile.second = tile_pos.y;
    }

    void m_tick(double dt)
    {
        switch (m_state) {
        case DEAL:
            m_tick_deal(dt);
            break;
        case MOVE:
            m_tick_move(dt);
            break;
        default:
            break;
        }
    }

    void m_draw(double)
    {
        glm::mat3 transf = m_current_transform();
        al_clear_to_color(BG_COLOR);
        draw_board(m_board, m_cursor_tile.first, m_cursor_tile.second, transf);
        switch (m_state)
        {
        case WAIT_DEST:
            m_draw_wait_dest();
            break;
        case MOVE:
            m_draw_move();
            break;
        case GAMEOVER:
            m_draw_gameover();
            break;
        default:
            break;
        }
        al_flip_display();
    }

public:
    Kulki() :
        m_alive { true },
        m_board { BOARD_W, BOARD_H },
        m_font { al_load_font("prstartk.ttf", -GAMEOVER_FONT_SIZE, 0) }
    {
        m_set_state_deal(DEAL_COUNT_INIT);
    }

    ~Kulki()
    {
        al_destroy_font(m_font);
    }

    void run(Allegro& al)
    {
        using namespace std::placeholders;
        al.realtime_loop(
            std::bind(&Kulki::m_on_key, this, _1, _2),
            std::bind(&Kulki::m_on_button, this, _1, _2),
            std::bind(&Kulki::m_on_cursor, this, _1, _2),
            std::bind(&Kulki::m_tick, this, _1),
            std::bind(&Kulki::m_draw, this, _1),
            m_alive);
    }
};

int main()
{
    Allegro al;
    (Kulki {}).run(al);
}

