//
// Created by bruno on 14/10/17.
//

#include "common.h"
#include <algorithm>
#include <tuple>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::string_literals;
using namespace std::string_view_literals;

// Problem 1: determining if two line segments intersect

// Do it without division

// Split the problem into 2 subproblems:

// 1: Determine whether points p,q lie on opposite sides of the line through points r and s, and whether points r,s lie
//    on opposite sides of the line through points p and q (onOppositeSides tests)

// 2: Determine whether the smallest rectangles whose sides are parallel to the x and y axes containing each of the p—q
// and r—s intersect (boundingBox test).

// Subproblem 1:

bool on_opposite_sides(Point a, Point b, Line l)
{
    //equation of line l is (l.p2.x - l.p1.x)(y - l.p1.y) - (l.p2.y - l.p1.y)(x - l.p1.x) = 0.

    //a and b lie on opposite sides of l if and only if LHS has opposite signs when substituted with each of a and b.

    double g = (l.p2.x - l.p1.x) * (a.y - l.p1.y) - (l.p2.y - l.p1.y) * (a.x - l.p1.x);
    double h = (l.p2.x - l.p1.x) * (b.y - l.p1.y) - (l.p2.y - l.p1.y) * (b.x - l.p1.x);

    return g * h <= 0.0;
}

// Subproblem 2:
// The bounding box of a line segment t p—q is the smallest rectangle containing p—q whose sides are parallel
// to the x and y axes

bool bounding_box_collision(Line l1, Line l2)
{
    struct BoundingBox
    {
        BoundingBox(Line l)
        {
            std::tie(min_x, max_x) = std::minmax(l.p1.x, l.p2.x);
            std::tie(min_y, max_y) = std::minmax(l.p1.y, l.p2.y);

        }
        double min_x;
        double max_x;
        double min_y;
        double max_y;
    };

    auto bb1 = BoundingBox{l1};
    auto bb2 = BoundingBox{l2};

    return bb1.max_x >= bb2.min_x &&
           bb1.min_x <= bb2.max_x &&
           bb1.max_y >= bb2.min_y &&
           bb1.min_y <= bb2.max_y;
}

// Putting everything together

bool intersect(Line l1, Line l2, bool verbose)
{
    // Intersect if and only if:
    bool check1 = on_opposite_sides(l1.p1, l1.p2, l2);
    bool check2 = on_opposite_sides(l2.p1, l2.p2, l1);
    bool check3 = bounding_box_collision(l1, l2);

    if (verbose)
    {
        std::cout << "on_opposite_sides(l1.p1, l1.p2, l2) returned " << check1 << std::endl;
        std::cout << "on_opposite_sides(l2.p1, l2.p2, l1) returned " << check2 << std::endl;
        std::cout << "bounding_box_collision(l1, l2) returned " << check3 << std::endl;
    }

    return check1 && check2 && check3;
}

// the on_opposite_sides tests on their own are
// sufficient to determine whether two line segments intersect,
// except in one special case – can you find it?

// The special case would be when the two segments are part of the same line.
// In such case both points would satisfy the line equation for either line segment provided.


// Testing:
Line ask_line()
{
    std::cout << "Type points for line segment ( eg (1,2) (3,4) ): ";

    double p1x, p1y, p2x, p2y;
    std::string res;

    std::getline(std::cin, res);
    white_strip(res);
    if (sscanf(res.c_str(), "(%lf,%lf)(%lf,%lf)", &p1x, &p1y, &p2x, &p2y) != 4)
    {
        std::cerr << "error in parsing \"" << res << "\".\n";
        std::abort();
    };

    return {{p1x, p1y}, {p2x, p2y}};
}

int main(int argc, char* argv[])
{
    int verbose = false;

    if (argc > 1) verbose = true;

    std::cout << std::boolalpha << on_opposite_sides({0, 0}, {2, 2}, {{2,1}, {4, 2}});

    Line l1 = ask_line();
    Line l2 = ask_line();

    std::cout << "the two lines" << (intersect(l1, l2, verbose) ? " "sv : " don't "sv) << "intersect.\n";
}



