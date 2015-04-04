// Copyright (C) 2015 Krzysztof Stachowiak

#include <fstream>
#include <sstream>
#include <algorithm>

#include "high_score.h"

namespace {

    struct EntryComparer {
        bool operator()(const HighScore::Entry& lhs, const HighScore::Entry& rhs) const
        {
            return lhs.score > rhs.score;
        }
    };

}

int HighScore::Entry::str_len() const
{
    int int_len;

    {
        std::stringstream ss;
        ss << score;
        int_len = ss.str().size();
    }

    return name.size() + int_len;
}

HighScore::HighScore(std::string filename) :
        m_filename(filename)
{
    std::ifstream in(m_filename.c_str());
    for (int i = 0; i < HIGH_SCORE_ENTRIES; ++i) {
        Entry e;
        char line[128] = { 0 };
        in.getline(line, 128, '#');
        e.name = line;
        in.getline(line, 128, '\n');
        {
            std::stringstream ss(line);
            ss >> e.score;
        }
        m_entries.push_back(e);
    }

    in.close();

    std::sort(begin(m_entries), end(m_entries), EntryComparer());
}

HighScore::~HighScore()
{
    std::ofstream out(m_filename.c_str(), std::ios_base::trunc);
    for (const auto& e : m_entries) {
        out << e.name << '#' << e.score << std::endl;
    }
    out.close();
}

std::vector<HighScore::Entry> const& HighScore::get_entries() const
{
    return m_entries;
}

void HighScore::add_entry(Entry const& e)
{
    m_entries.back() = e;
    std::sort(begin(m_entries), end(m_entries), EntryComparer());
}

int HighScore::insert_position(int score) const
{
    if (score <= m_entries.back().score) {
        return -1;
    }
    auto new_it = std::find_if(begin(m_entries), end(m_entries), [score](const Entry& x) { return x.score < score; });
    return std::distance(begin(m_entries), new_it);
}
