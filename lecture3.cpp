//
// Created by bruno on 17/01/18.
//

#include <cassert>
#include <cmath>
#include "common.h"

namespace problem4 {

const char description[] = R"(
// Problem 4: Finding a closest pair of points.

// Problem: given a set P of n points in the plane, find a pair whose distance from
// each other is as small as possible.

// Naive algorithm : Check each pair - O(n^2) complexity
// Divide and conquer: O(n log n) complexity, based on sorting

// Basic idea:
// - Sort P by x-coordinate once at the start
// - divide P into two equal-sized subsets Q,R based on the x-coordinate
// - Solve for each subset recursively
// - Combine: closest pair (x,y) satisfies either:
//   1) (x, y) belong to Q (solved already)
//   2) (x, y) belong to R (solved already)
//   3) x belongs to Q, y belongs to R (with distance apart <= d, where d = min(d1, d2)

// To take care of case 3:
// - Eliminate all the points at distance > d from L (the 'division line' between the two subsets)
// - Sort the remaining points on their y coordinate.
// - Compare each point against the next 5 successors (max 6 points can be in the given region)
)";

struct ClosestPairResult
{
    std::pair<Point, Point> closest_pair;
    double squared_distance; // Use squared distance as it's faster to compute.

    ClosestPairResult(Point p1, Point p2) : closest_pair{p1, p2}
    {
        squared_distance = std::pow((p1.x - p2.x), 2) + std::pow((p1.y - p2.y), 2);
    }

    ClosestPairResult(Point p1) : closest_pair{p1, p1}
    {
        // Consider distance to itself to be infinite.
        squared_distance = std::numeric_limits<double>::max();
    }

    ClosestPairResult() : ClosestPairResult(Point{}) {};

    bool operator <(const ClosestPairResult &rhs) const {
        return squared_distance < rhs.squared_distance;
    }
};

/** returns a PointSet containing points in range with
  * x-coord within dist of orig; preserves relative order
  **/
PointSet filter_x(PointSet &ps, range<PointSet::iterator> rng, double dist, double orig)
{
    PointSet result;
    std::copy_if(rng.begin(), rng.end(), std::back_inserter(result), [=](Point p) {
        return std::abs(p.x - orig) <= dist;
    });

    return result;
}


/*
 * Assumes p[range_start...range_end] sorted on x-coord;
 * Returns the closest pair of points in p[range_start...range_end].
 * ps is also modified so that the points in [range_start...range_end] are now sorted by y-coord.
 */
ClosestPairResult closest_pair_rec_impl(PointSet &ps, range<PointSet::iterator> rng)
{
    assert(rng.size() > 0 && "The range must contain at least 1 element.");

    // Base case
    if (rng.size() == 1) {
        return {rng.first()};
    }

    // Split the range around the middle
    auto [left_range, right_range] = rng.split_at(rng.size() / 2);
    double mid_x = (left_range.last().x + right_range.first().x) / 2.0;

    auto closest_left =  closest_pair_rec_impl(ps, left_range);
    auto closest_right = closest_pair_rec_impl(ps, right_range);

    // Merge the two sides of the pointSet based on the y-coord.
    std::inplace_merge(left_range.begin(), left_range.end(), right_range.end(), [](Point p1, Point p2) {
        return p1.y < p2.y;
    });

    ClosestPairResult closest = std::min(closest_left, closest_right);
    PointSet filtered = filter_x(ps, rng, closest.squared_distance, mid_x);

    if (filtered.empty()) {
        return closest;
    }

    // Iterate over each remaining point after filtering:
    for (size_t j = 0, len_filt = filtered.size(); j < len_filt - 1; ++j) {
        // Compare each of the points with the next 5 elements (or less if len_filt is smaller)
        for (size_t k = j + 1; k < std::min(j + 6, len_filt); ++k) {
            ClosestPairResult tentative{filtered[j], filtered[k]};
            if (tentative < closest) {
                closest = tentative;
            }
        }
    }
    return closest;
}

ClosestPairResult closest_pair(PointSet ps)
{
    // Sort on x coordinates.
    std::sort(ps.begin(), ps.end(), [](Point a, Point b) { return a.x < b.x; });

    return closest_pair_rec_impl(ps, {ps.begin(), ps.end()});
}

const char analysis[] = R"(
// Analysis of the algorithm:

// - Initial sort on x-coordinates is O(n log n)
// - When dealing with a PointSet of length n, merge and filter are both O(n)
// - The nested for loops contribute O(n) (inner loop executed max 5 times)
// - Let f(n) be the worst-case complexity. Then
//   - f(n) <= 2 f(n/2) + cn (n>1) where cn are filter,merge etc
//   - f(1) = d

// Hence, f(n) = O(n log n) as for mergesort.
)";

void run()
{
    PointSet ps = ask_pointset();
    auto res = problem4::closest_pair(ps);
    auto [p1, p2] = res.closest_pair;
    std::cout << "Smallest distance is " << std::sqrt(res.squared_distance)
              << " between points " << p1 << " " << p2 << std::endl;
}

} //end namespace problem4

int main()
{
    std::cout << problem4::description << std::endl;
    problem4::run();
    std::cout << problem4::analysis << std::endl;
}
