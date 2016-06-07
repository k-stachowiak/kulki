// Copyright (C) 2015 Krzysztof Stachowiak

#include "kulki_variables.h"

namespace {
    std::random_device device;
    std::mt19937 engine { device() };
}

KulkiVariables::KulkiVariables(KulkiConstants &constants) :
    m_constants { constants },
    m_board { m_constants.board_w, m_constants.board_h, m_constants.empty_field },
    m_cursor_tile { -1, -1 },
    m_score { 0 },
    m_streak { 0 }
{}

void KulkiVariables::gen_next_deal(int count)
{
    std::uniform_int_distribution<int> distr_color(0, m_constants.ball_count - 1);
    for (int i = 0; i < count; ++i) {
        m_next_deal.push_back(distr_color(engine));
    }
}

