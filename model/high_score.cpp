// Copyright (C) 2015 Krzysztof Stachowiak

#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "high_score.h"

namespace {

    bool high_score_entry_compare(
            const HighScoreEntry &x,
            const HighScoreEntry &y)
    {
        if (x.balls < y.balls) {
            return true;
        } else if (x.balls > y.balls) {
            return false;
        } else {
            return x.score < y.score;
        }
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
    HighScoreEntry entry;
    while (in >> entry.balls >> entry.score >> entry.name) {
        result.m_entries.push_back(entry);
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

    for (const auto &entry : hs.m_entries) {
        out << entry.balls << ' ' << entry.score << ' ' << entry.name << std::endl;
    }
}

bool HighScore::can_insert(int balls, int score) const
{
    std::vector<HighScoreEntry> entries = get_entries_for_balls(balls);
    if (static_cast<int>(entries.size()) < m_max_entries) {
        return true;
    } else {
        return score > entries.back().score;
    }
}

std::vector<int> HighScore::get_ball_counts() const
{
    std::set<int> result;
    for (const auto &entry: m_entries) {
        result.insert(entry.balls);
    }
    return { begin(result), end(result) };
}

std::vector<HighScoreEntry> HighScore::get_entries_for_balls(int balls) const
{
    std::vector<HighScoreEntry> result;
    std::copy_if(
        begin(m_entries), end(m_entries),
        std::back_inserter(result),
        [balls](const HighScoreEntry &entry) {
            return entry.balls == balls;
        });
    std::sort(begin(result), end(result), high_score_entry_compare);
    return result;
}

void HighScore::add_entry(const HighScoreEntry &entry)
{
    m_entries.push_back(entry);
}

