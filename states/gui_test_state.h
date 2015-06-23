#ifndef GUI_TEST_STATE_H
#define GUI_TEST_STATE_H

#include "state_node.h"

struct KulkiContext;

class GuiTestState : public StateNode {
    KulkiContext *m_context;
public:
    GuiTestState(KulkiContext *context);
    void on_key(int key, bool down);
};

#endif
