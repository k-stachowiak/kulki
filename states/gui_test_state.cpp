#include "gui_test_state.h"

GuiTestState::GuiTestState(KulkiContext *context) :
    m_context { context }
{}

void GuiTestState::on_key(int key, bool down)
{
    if (down) {
        m_context->set_state_menu();
    }
}
