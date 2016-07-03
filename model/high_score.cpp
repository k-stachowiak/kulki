// Copyright (C) 2015 Krzysztof Stachowiak

#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "high_score.h"
#include "dick.h"

namespace {

    bool high_score_entry_compare(
            const HighScoreEntry &x,
            const HighScoreEntry &y)
    {
        return x.score < y.score;
    }

}

HighScore HighScore::load(const std::string& filename, int max_entries)
{
    std::ifstream in { filename.c_str() };
    if (!in.is_open()) {
        std::cerr << "Failed opening high score file: \"" << filename << "\"" << std::endl;
        exit(1);
    }

    HighScore result;
    result.m_max_entries = max_entries;
    int balls;
    HighScoreEntry entry;
    while (in >> balls >> entry.score >> entry.name) {
        result.m_entries[balls].push_back(entry);
    }

    return result;
}

void HighScore::store(const std::string& filename, const HighScore& hs)
{
    std::ofstream out { filename.c_str(), std::ios_base::trunc };
    if (!out.is_open()) {
        std::cerr << "Failed opening high score file: \"" << filename << "\"" << std::endl;
        exit(1);
    }

    for (const auto& pr : hs.m_entries) {
        for (const auto& entry : pr.second) {
            out << pr.first << ' ' << entry.score << ' ' << entry.name << std::endl;
        }
    }
}

bool HighScore::can_insert(int balls, int score) const
{
    const std::vector<HighScoreEntry>& entries = m_entries.at(balls);
    if (static_cast<int>(entries.size()) < m_max_entries) {
        return true;
    } else {
        auto min = std::min_element(
            begin(entries),
            end(entries),
            high_score_entry_compare);
        return score > min->score;
    }
}

std::vector<int> HighScore::get_ball_counts() const
{
    std::vector<int> result;
    for (const auto& pr: m_entries) {
        result.push_back(pr.first);
    }
    return result;
}

const std::vector<HighScoreEntry>& HighScore::get_entries_for_balls(int balls) const
{
    return m_entries.at(balls);
}

void HighScore::add_entry(int balls, const HighScoreEntry &entry)
{
    m_entries[balls].push_back(entry);
}

