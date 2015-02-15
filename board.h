#ifndef MY_BOARD_H
#define MY_BOARD_H

#include <vector>
#include <limits>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "utility.h"

struct Board {

    int m_width, m_height;
    std::vector<int> m_fields;

    // Semi-regular:
    Board() : m_width { 0 }, m_height { 0 } {};
    Board(const Board& x) : m_width { x.m_width }, m_height { x.m_height }, m_fields { x.m_fields } {}
    Board(Board&& x) : m_width { x.m_width }, m_height { x.m_height }, m_fields { std::move(x.m_fields) } {}
    Board& operator=(const Board& x)
    {
        m_width = x.m_width;
        m_height = x.m_height;
        m_fields = x.m_fields;
        return *this;
    }
    Board& operator=(Board&& x)
    {
        m_width = x.m_width;
        m_height = x.m_height;
        m_fields = std::move(x.m_fields);
        return *this;
    }

    // Custom constructor:
    Board(int w, int h) :
        m_width { w }, m_height { h },
        m_fields(w * h, EMPTY)
    {}

    // API:
    int &operator()(int x, int y)
    {
        return m_fields[y * m_width + x];
    }

    int operator()(int x, int y) const
    {
        return const_cast<Board&>(*this)(x, y);
    }

    void clear()
    {
        std::fill(begin(m_fields), end(m_fields), EMPTY);
    }

    bool has(int x, int y) const
    {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    int free_fields()
    {
        return std::count(begin(m_fields), end(m_fields), EMPTY);
    }

    std::vector<std::pair<int, int>> m_find_streak_part(const std::pair<int, int>& src, int dx, int dy)
    {
        std::vector<std::pair<int, int>> result;

        int color = (*this)(src.first, src.second);
        int x = src.first, y = src.second;

        while (has(x, y) && (*this)(x, y) == color) {
            result.emplace_back(x, y);
            x += dx;
            y += dy;
        }

        dx *= -1;
        dy *= -1;
        x = src.first + dx;
        y = src.second + dy;

        while (has(x, y) && (*this)(x, y) == color) {
            result.emplace_back(x, y);
            x += dx;
            y += dy;
        }

        return result;
    }

    template <typename Out>
    bool find_streak(const std::pair<int, int>& src, Out out)
    {
        int inserts = 0;
        std::vector<std::pair<int, int>> directions {
            { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, -1 }
        };

        for (const auto& dir : directions) {
            auto s = m_find_streak_part(src, dir.first, dir.second);
            if (s.size() >= STREAK_MIN) {
                std::copy(begin(s), end(s), out);
                ++inserts;
            }
        }

        return inserts > 0;
    }

    bool find_path(std::pair<int, int> src, std::pair<int, int> dst, std::deque<std::pair<int, int>>& path)
    {
        std::unordered_set<std::pair<int, int>> open;
        std::unordered_map<std::pair<int, int>, std::pair<int, int>> preds;
        std::unordered_map<std::pair<int, int>, int> dists;

        for (int x = 0; x < m_width; ++x) {
            for (int y = 0; y < m_height; ++y) {
                dists[std::pair<int, int>(x, y)] = std::numeric_limits<int>::max();
            }
        }

        open.insert(src);
        dists[src] = 0;

        while (!open.empty()) {

            auto it = std::min_element(begin(open), end(open),
                [&dists](const std::pair<int, int>& x, const std::pair<int, int>& y) {
                    return dists[x] < dists[y];
                });

            std::pair<int, int> u = *it;
            if (u == dst) {
                break;
            }

            std::vector<std::pair<int, int>> neighbors {
                { u.first - 1, u.second },
                { u.first + 1, u.second },
                { u.first, u.second - 1 },
                { u.first, u.second + 1 },
            };

            int new_dist = dists[u] + 1;
            for (const auto& v : neighbors) {
                if ((*this)(v.first, v.second) != EMPTY) {
                    continue;
                }
                if (new_dist < dists[v]) {
                    dists[v] = new_dist;
                    preds[v] = u;
                    open.insert(v);
                }
            }

            open.erase(u);
        }

        if (dists[dst] == std::numeric_limits<int>::max()) {
            return false;
        } else {
            std::pair<int, int> n = dst;
            while (n != src) {
                path.push_front(n);
                n = preds[n];
            }
            path.push_front(n);
            return true;
        }
    }

};

#endif
