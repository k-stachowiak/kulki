// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef KULKI_VARIABLES_H
#define KULKI_VARIABLES_H

#include "board.h"
#include "kulki_constants.h"

class KulkiConfig;

struct KulkiVariables {

    KulkiConstants &m_constants;

    Board m_board;
    std::pair<int, int> m_cursor_tile;
    int m_score;
    int m_streak;
    int m_ball_count;
    std::vector<int> m_next_deal;

    KulkiVariables(KulkiConstants &constants, KulkiConfig& config);
    void gen_next_deal(int count);
};

#endif
