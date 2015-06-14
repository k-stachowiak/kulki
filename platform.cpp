// Copyright (C) 2015 Krzysztof Stachowiak

#include <iostream>

#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

#include "config.h"
#include "platform.h"

void Platform::m_process_events(StateNode &state_node)
{
    ALLEGRO_EVENT event;

    while (!al_is_event_queue_empty(m_ev_queue.get())) {
        al_get_next_event(m_ev_queue.get(), &event);
        switch (event.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            state_node.on_kill();
            return;

        case ALLEGRO_EVENT_KEY_DOWN:
            state_node.on_key(event.keyboard.keycode, true);
            break;

        case ALLEGRO_EVENT_KEY_UP:
            state_node.on_key(event.keyboard.keycode, false);
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            state_node.on_button(event.mouse.button, true);
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            state_node.on_button(event.mouse.button, false);
            break;

        case ALLEGRO_EVENT_MOUSE_AXES:
            state_node.on_cursor(event.mouse.x, event.mouse.y);
            break;

        default:
            break;
        }
        if (state_node.is_done()) {
            return;
        }
    }
}

void Platform::m_realtime_loop_step(StateNode &state_node)
{
    const double new_time = al_get_time();

    double frame_time = new_time - m_current_time;

    if (frame_time > config::MAX_FRAME_TIME) {
        frame_time = config::MAX_FRAME_TIME;
    }

    m_current_time = new_time;
    m_accumulator += frame_time;

    while (m_accumulator >= m_spf) {
        state_node.tick(m_spf);
        m_accumulator -= m_spf;
    }

    const double frame_weight = m_accumulator / m_spf;
    state_node.draw(frame_weight);
}

Platform::Platform() :
    m_screen_w { config::SCREEN_W },
    m_screen_h { config::SCREEN_H },
    m_fps { config::FPS },
    m_spf { 1.0 / m_fps }
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

    m_display.reset(al_create_display(m_screen_w, m_screen_h));
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

    /*
    if(!al_install_audio()) {
        std::cerr << "Failed initializing audio." << std::endl;
        exit(1);
    }
    */

    m_ev_queue.reset(al_create_event_queue());
    if (!m_ev_queue) {
        std::cerr << "Failed creating event queue." << std::endl;
        exit(1);
    }

    al_register_event_source(m_ev_queue.get(), al_get_display_event_source(m_display.get()));
    al_register_event_source(m_ev_queue.get(), al_get_keyboard_event_source());
    al_register_event_source(m_ev_queue.get(), al_get_mouse_event_source());
}

void Platform::realtime_loop(StateNode &state_node)
{
    m_current_time = al_get_time();
    m_accumulator = 0;

    while (true) {
        m_process_events(state_node);
        if (state_node.is_done()) {
            break;
        }

        m_realtime_loop_step(state_node);
        if (state_node.is_done()) {
            break;
        }

        al_rest(config::FRAME_REST);
    }
}

