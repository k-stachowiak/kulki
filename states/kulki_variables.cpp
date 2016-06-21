// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "kulki_variables.h"

namespace {
    std::random_device device;
    std::mt19937 engine { device() };
}

KulkiVariables::KulkiVariables(KulkiConstants &constants, KulkiConfig& config) :
    m_constants { constants },
    m_board { m_constants.board_w, m_constants.board_h, m_constants.empty_field },
    m_cursor_tile { -1, -1 },
    m_score { 0 },
    m_streak { 0 },
    m_ball_count { config.get_integer("DEFAULT_COLOR_COUNT") }
{}

void KulkiVariables::gen_next_deal(int count)
{
    std::uniform_int_distribution<int> distr_color(0, m_ball_count - 1);
    for (int i = 0; i < count; ++i) {
        m_next_deal.push_back(distr_color(engine));
    }
}

