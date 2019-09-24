//
// Created by bruno on 14/10/17.
//

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <climits>

#include "common.h"

static constexpr double pi = 3.1415926535897;


namespace problem2 {
const char description[] = R"(
// Problem 2: Constructing a simple polygon
// NOTE: A simple polygon does not contain intersecting line segments.

// Given a set of n points, connect them to form a simple polygon.

// The naive method of joining up the points in any order won't work in general.

// IDEA(I): choose a point p as the pivot of a circular scan. Connect the points in the order they are encountered.
// This still causes problems when connecting the last point to the first to complete the polygon.

// IDEA(II) choose the point p as one of the points from the point set. For example, the one with the greatest x coord.
)";

/**
 * Return magnitude of vector
 */
double vec_abs(Vec2d vec)
{
    return std::hypot(vec.x, vec.y);
}

void simple_polygon_v1(PointSet &ps)
{
    std::vector<double> angles(ps.size());

    // Make ps[0] be the point with the greatest x-coord (or with the smallest y-coord if x-coord are the same).
    std::nth_element(ps.begin(), ps.begin(), ps.end(), [](const Point &p1, const Point &p2)
    {
        return p1.x > p2.x || (p1.x == p2.x && p1.y < p2.y);
    });

    Point p0 = ps[0];

    for (Point &p : range(ps.begin() + 1, ps.end()))
    {
        double num = p0.x - p.x;
        double denom = p.y - p0.y;
        // Order the points by the angle between the pivot and p.
        p._ordering = std::atan2(num, denom);
    }

    std::sort(ps.begin() + 1, ps.end(), [p0](const Point &p1, const Point &p2)
    {
        // Order by the angle. If tie, use the one with smaller distance from the pivot point (p0).
        return p1._ordering < p2._ordering ||
                (p1._ordering == p2._ordering && vec_abs(p0 - p1) < vec_abs(p0 - p2));
    });
}

const char analysis[] = R"(
// Analysis of the algorithm:
// nth_element is a simple linear scan and swap. O(n)
// Loop is O(n)
// sort is O(n log n)

// So overall O(n log n).
)";

// Two aspects can be simplified:
// - no need to use the arctan function. Can simply sort according to gradients.
// - no need to compute the square root to sort on distances (std::hypot does it). Can simply sort on squares.

/**
 * Same as vec_abs, but does not sqrt the result.
 */
double squared_vec_abs(Vec2d vec)
{
    return std::pow(vec.x, 2) + std::pow(vec.y, 2);
};

void simple_polygon_v2(PointSet &ps)
{
    std::vector<double> angles(ps.size());

    // Make ps[0] be the point with the greatest x-coord (or with the smallest y-coord if x-coord are the same).
    std::nth_element(ps.begin(), ps.begin(), ps.end(), [](const Point &p1, const Point &p2)
    {
        return p1.x > p2.x || (p1.x == p2.x && p1.y < p2.y);
    });

    Point p0 = ps[0];

    for (Point &p : range(ps.begin() + 1, ps.end()))
    {
        double denom = p0.x - p.x;
        double num   = p0.y - p.y;

        // Note: no atan. Simple gradient (Dy/Dx).
        p._ordering = num / denom;
    }


    std::sort(ps.begin() + 1, ps.end(), [p0](const Point &p1, const Point &p2)
    {
        // Order by the angle. If tie, use the one with smaller distance from the pivot point (p0).
        return p1._ordering < p2._ordering ||
               (p1._ordering == p2._ordering && squared_vec_abs(p0 - p1) < squared_vec_abs(p0 - p2));
    });
}

// One last problem to address (tutorial 1 exercise 6):
// If the last two (or more) points are coplanar with the pivot (ps[0]),
// then sort them by decreasing distance to the pivot.

void simple_polygon_v3(PointSet &ps)
{
    // The first steps are equivalent to the previous version.
    simple_polygon_v2(ps);

    // Find the first element where the gradient is different from the previous, starting from the end.
    auto mismatch = std::adjacent_find(ps.rbegin(), ps.rend(), [](auto p1, auto p2) {
        return p1._ordering != p2._ordering;
    }).base();

    if (std::distance(mismatch, ps.end()) > 0)
    {
        // If there are more than one consecutive element with the same gradient, reverse the order until the end.
        std::reverse(std::prev(mismatch), ps.end());
    }
}

void run()
{
    auto ps = ask_pointset();
    auto psv2 = ps;
    auto psv3 = ps;
    std::cout << "pointset provided:                                " << ps << std::endl;
    auto us_v1 = time_us(simple_polygon_v1, ps);
    std::cout << "pointset transformed (original):                  " << ps << " (" << us_v1.count() << " us)" << std::endl;
    auto us_v2 = time_us(simple_polygon_v2, psv2);
    std::cout << "pointset transformed (no tan, no sqrt):           " << psv2 << " (" << us_v2.count() << " us)" << std::endl;
    auto us_v3 = time_us(simple_polygon_v3, psv3);
    std::cout << "pointset transformed (sort last coplanar points): " << psv3 << " (" << us_v3.count() << " us)" << std::endl;
}

} // end namespace problem2

namespace problem3
{
const char description[] = R"(
// Problem 3: Finding the convex hull.
// The convex hull of a set of points is the smallest convex polygon that contains all points.
// Here, smallest refers to the total length of the edges (or, equivalently, the area).

// Properties of the convex hull:
// - Every point of the convex hull of P is itself a member of P
// - The points of P with the smallest/largest x/y coordinates are part of the convex hull of P.
// - The furthest pair of points in P are points of the convex hull of P.

// Graham Scan algorithm: O(n log n) time.
// Basic idea of the Graham Scan algorithm:
// - Choose a point known to be on the hull as a pivot(eg. point with the greatest x coord and smallest y coord if tie)
// - Scan the remaining points in order of angle to the pivot
// - For each point, include it in temporary hull, possibly excluding one or more of its predecessors.
// - As we trace out the potential hull, we expect to 'turn left' (or right) from one point to another
// - If we turn right we must exclude one or more points.
)";

bool angle_gteq_pi(Line a, Line b)
{
    // Use the cross product between two vectors.
    // Given that u x v = |u| * |v| * sin(angle) * n
    // and that sin(angle) <= 0 if pi <= angle <= 2pi,
    // we can say that u x v should be negative.
    // The cross product then only boils down to:
    Vec2d da = a.p2 - a.p1;
    Vec2d db = b.p2 - b.p1;

    return (da.x * db.y - da.y * db.x) <= 0;

}
PointSet graham_scan(PointSet &ps, bool verbose)
{
    PointSet ch{ps.size(), Point{}}; // Points belonging to the convex hull.
    problem2::simple_polygon_v3(ps);

    if (verbose) std::cout << "Simple polygon    : "<< ps << std::endl;

    // Given the way simple_polygon works, the first 3 points always form a right turn.
    ch[0] = ps[0];
    ch[1] = ps[1];
    ch[2] = ps[2];

    std::size_t m = 2; // m points other than the pivot in current hull
    for (std::size_t k = 3, len = ps.size(); k < len; ++k)
    {
        while (angle_gteq_pi(Line{ch[m-1], ch[m]}, Line{ch[m], ps[k]}))
        {
            --m; // exclude ch[m]
        }
        ch[++m] = ps[k];
        if (verbose) std::cout << "iteration: " << ch << std::endl;
    }

    ch.resize(m + 1);
    return ch;
}

// TODO Furthest pair of points via rotating calipers method.

void run(bool verbose)
{
    auto ps = ask_pointset();
    std::cout << "pointset provided : " << ps << std::endl;
    auto hull = graham_scan(ps, verbose);
    std::cout << "graham scan result: " << hull << std::endl;
}

const char analysis[] = R"(
// Analysis of Graham Scan:

// Simple Polygon is O(n log n)
// Angle function has O(1) complexity
// Main loop has O(n) complexity (a point is eliminated at most once)

// Hence overall the algorithm is O(n log n) (because of sorting!)
)";

} // end namespace problem3


int main(int argc, char *argv[])
{
    std::string choice;
    std::cout << "Problem to run? 'sp' for simple polygon (problem 2), 'ch' for convex hull (problem 3): ";
    std::getline(std::cin, choice);

    if (choice == "sp") {
        std::cout << problem2::description << std::endl;
        problem2::run();
        std::cout << problem2::analysis << std::endl;
    } else if (choice == "ch") {
        std::cout << problem3::description << std::endl;
        problem3::run(argc > 1);
        std::cout << problem3::analysis << std::endl;
    }
}