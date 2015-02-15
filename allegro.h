#ifndef MY_ALLEGRO_H
#define MY_ALLEGRO_H

#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

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

#endif
