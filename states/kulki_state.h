// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef STATE_H
#define STATE_H

#include <memory>

#include <allegro.h>

#include <glm/glm.hpp>

#include "board.h"
#include "high_score.h"

struct KulkiContext;

struct KulkiState {

    enum class Enum {
        DEAL,
        WAIT_BALL,
        WAIT_DEST,
        MOVE,
        GAMEOVER,
        SCORE
    };

    virtual void on_key(int key, bool down) {}
    virtual void on_button(int button, bool down) {}
    virtual void on_cursor(int x, int y) {}
    virtual void tick(double dt) {}
    virtual void draw(const glm::mat3& transf) {}
};

void draw_ball(double x, double y, int color, double r, const glm::mat3& transf);

#endif

