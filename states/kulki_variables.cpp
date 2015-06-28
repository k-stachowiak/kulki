#include "kulki_variables.h"

KulkiVariables::KulkiVariables(KulkiConstants &constants) :
    m_board { constants.board_w, constants.board_h, constants.empty_field },
    m_cursor_screen { -1, -1 },
    m_cursor_tile { -1, -1 },
    m_score { 0 },
    m_streak { 0 }
{}
