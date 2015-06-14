// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef STATE_NODE_H
#define STATE_NODE_H

struct StateNode {
    virtual bool is_done() const { return false; }
    virtual void on_kill() {}
    virtual void on_key(int key, bool down) {}
    virtual void on_button(int button, bool down) {}
    virtual void on_cursor(int x, int y) {}
    virtual void tick(double dt) {}
    virtual void draw(double weight) {}
};

#endif

