// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef HIGH_SCORE_H
#define HIGH_SCORE_H

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "config.h"

struct HighScoreEntry {
    int score;
    std::string name;

    int str_len() const
    {
        std::stringstream ss;
        ss << score;
        std::string score_string;
        ss >> score_string;
        return name.size() + score_string.size();
    }
};

class HighScore {
    int m_max_entries;
    std::unordered_map<int, std::vector<HighScoreEntry>> m_entries;

public:
    static HighScore load(const std::string& filename, int max_entries);
    static void store(const std::string& filename, const HighScore& hs);

    bool can_insert(int balls, int score) const;
    std::vector<int> get_ball_counts() const;
    const std::vector<HighScoreEntry>& get_entries_for_balls(int balls) const;

    void add_entry(int balls, const HighScoreEntry& entry);
};

#endif
