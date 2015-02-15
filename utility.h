#ifndef UTILITY_H
#define UTILITY_H

#include <utility>

namespace std {
    template <> struct hash<std::pair<int, int>> {
        size_t operator()(const std::pair<int, int> &v) const {
            std::hash<int> int_hasher;
            return int_hasher(v.first) ^ int_hasher(v.second);
        }
    };
}

glm::mat3 translate(double x, double y)
{
    return {
        1, 0, x,
        0, 1, y,
        0, 0, 1
    };
}

glm::mat3 scale(double factor)
{
    return {
        factor, 0, 0,
        0, factor, 0,
        0, 0, 1
    };
}

#endif
