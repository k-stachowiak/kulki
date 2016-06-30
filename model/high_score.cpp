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
    LOG_TRACE(
        "Loading from file: %s, max entries: %d",
        filename.c_str(), max_entries);

    std::ifstream in { filename.c_str() };
    if (!in.is_open()) {
        std::cerr << "Failed opening high score file: \"" << filename << "\"" << std::endl;
        exit(1);
    }

    HighScore result;
    result.m_max_entries = max_entries;
    HighScoreEntry entry;
    while (in >> entry.balls >> entry.score >> entry.name) {
        LOG_TRACE("\tentry(%d): %s -> %d", entry.balls, entry.name.c_str(), entry.score);
        result.m_entries.push_back(entry);
    }

    return result;
}

void HighScore::store(const std::string& filename, const HighScore& hs)
{
    LOG_TRACE("Storing to file: %s", filename.c_str());

    std::ofstream out { filename.c_str(), std::ios_base::trunc };
    if (!out.is_open()) {
        std::cerr << "Failed opening high score file: \"" << filename << "\"" << std::endl;
        exit(1);
    }

    for (const auto &entry : hs.m_entries) {
        LOG_TRACE("\tentry(%d): %s -> %d", entry.balls, entry.name.c_str(), entry.score);
        out << entry.balls << ' ' << entry.score << ' ' << entry.name << std::endl;
    }
}

bool HighScore::can_insert(int balls, int score) const
{
    LOG_TRACE("Test if can insert score %d for %d balls", score, balls);
    std::vector<HighScoreEntry> entries = get_entries_for_balls(balls);
    if (static_cast<int>(entries.size()) < m_max_entries) {
        LOG_TRACE("\tMax entries not reached => can insert");
        return true;
    } else {
        LOG_TRACE(
            "\tMax entries reached, bottom score: %d, inserted score: %d",
            entries.front().score, score);
        return score > entries.back().score;
    }
}

std::vector<int> HighScore::get_ball_counts() const
{
    std::vector<int> result;
    for (const auto &entry: m_entries) {
        result.push_back(entry.balls);
    }
    std::sort(begin(result), end(result));
    return { begin(result), std::unique(begin(result), end(result)) };
}

std::vector<HighScoreEntry> HighScore::get_entries_for_balls(int balls) const
{
    LOG_TRACE("Returning entries for %d balls", balls);
    std::vector<HighScoreEntry> result;
    std::copy_if(
        begin(m_entries), end(m_entries),
        std::back_inserter(result),
        [balls](const HighScoreEntry &entry) {
            return entry.balls == balls;
        });
    std::sort(begin(result), end(result), high_score_entry_compare);
    for (const HighScoreEntry& e : result) {
        LOG_TRACE("\tEntry: %s, %d", e.name.c_str(), e.score);
    }
    return result;
}

void HighScore::add_entry(const HighScoreEntry &entry)
{
    LOG_TRACE("Adding entry: %s, %d", entry.name.c_str(), entry.score);
    m_entries.push_back(entry);
}

