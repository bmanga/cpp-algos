//
// Created by bruno on 10/10/17.
//

#ifndef UNTITLED_COMMON_H
#define UNTITLED_COMMON_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>

struct Vec2d
{
    double x;
    double y;
};

struct Point
{
    double x;
    double y;

    // Extra data.
    union {
        double _ordering = 0; // Used for sorting by external values.
    };

    friend Vec2d operator -(Point lhs, Point rhs)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }
};

struct Line
{
    Point p1;
    Point p2;
};

using PointSet = std::vector<Point>;

inline void white_strip(std::string & res)
{
    res.erase(std::remove_if(res.begin(), res.end(), isspace), res.end());
};

inline PointSet ask_pointset()
{
    PointSet ps;
    std::string str;

    std::cout << "Type a series of lines of points (eg (1,2) ) .\n";

    while (std::getline(std::cin, str))
    {
        white_strip(str);
        double x,y;
        if (sscanf(str.c_str(), "(%lf,%lf)", &x, &y) != 2) {
            break;
        }
        ps.push_back({x, y});
    }
    return ps;
}

/*
struct PointSet
{
    std::vector<Point> points;
};
*/


inline std::ostream &operator <<(std::ostream &out, Point p)
{
    return out << "(" << p.x << "," << p.y << ")";
}

inline std::ostream &operator <<(std::ostream &out, const PointSet &ps)
{
    for (Point p : ps)
    {
        out << p << " ";
    }

    return out;
}

template <class It>
class range
{
public:
    range(It beg, It end) : m_beg(beg), m_end(end) {}

    It begin() { return m_beg; }
    It end()   { return m_end; }

    auto first() { return *m_beg; }
    auto last()  { return *(m_end - 1); }

    std::size_t size() const { return std::distance(m_beg, m_end); }

    std::pair<range<It>, range<It>> split_at(std::size_t pos) const
    {
        It middle = m_beg + pos;
        return {{m_beg, middle}, {middle, m_end}};
    };

public:
    It m_beg;
    It m_end;
};

template <class Fn, class... Args>
std::chrono::microseconds time_us(Fn &&fn, Args &&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    fn(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

#endif //UNTITLED_COMMON_H
