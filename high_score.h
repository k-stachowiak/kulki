// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef HIGH_SCORE_H
#define HIGH_SCORE_H

#include <string>
#include <vector>

static const int HIGH_SCORE_ENTRIES = 10;

class HighScore {
public:
    struct Entry {
        std::string name;
        int score;
        int str_len() const;
        friend bool operator==(const Entry& x, const Entry& y) {
            return x.name == y.name && x.score == y.score;
        }
    };

private:
    std::string m_filename;
    std::vector<Entry> m_entries;

public:
    HighScore(std::string filename);
    ~HighScore();
    std::vector<Entry> const& get_entries() const;
    void add_entry(Entry const& e);
    int insert_position(int score) const;
};

#endif
