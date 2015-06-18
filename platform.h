// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef PLATFORM_H
#define PLATFORM_H

#include <memory>

#include <allegro5/allegro.h>

#include "state_node.h"

class Platform {

    struct AlDisplayDeleter { void operator()(ALLEGRO_DISPLAY *display) { al_destroy_display(display); } };
    struct AlEvQueueDeleter { void operator()(ALLEGRO_EVENT_QUEUE *queue) { al_destroy_event_queue(queue); } };

    const int m_screen_w, m_screen_h;
    const double m_fps, m_spf;
    const double m_max_frame_time;
    const double m_frame_rest;
    double m_current_time, m_accumulator;

    std::unique_ptr<ALLEGRO_DISPLAY, AlDisplayDeleter> m_display;
    std::unique_ptr<ALLEGRO_EVENT_QUEUE, AlEvQueueDeleter> m_ev_queue;

    void m_process_events(StateNode &state_node);
    void m_realtime_loop_step(StateNode &state_node);

public:
    Platform(int screen_w, int screen_h, double fps, double max_frame_time, double frame_rest);
    void realtime_loop(StateNode &state_node);
};

#endif
