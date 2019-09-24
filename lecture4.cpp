//
// Created by bruno on 18/01/18.
//

namespace problem5 {

// Problem 5: Finding line segment intersections
// Problem: Given a set of h horizontal and v vertical line segments in the plane,
// find all intersections.

// This is a common type of problem - finding intersections between geometric objects

// Brute force algorithm: examine all pairs
// - Each pair can be checked in O(1) time
// - But this is never better than O(hv)

// No algorithm can be better than O(hv) in the worst case, as there could be hv intersections.

// Can we get something better when the number of intersections is small?

// This solution illustrates a powerful general technique: line-sweep
// The algorithm will have O(n log n + p) complexity in the worst case, where n = h + v

// The line-sweep technique:
// - Imagine an infinite vertical line 'sweeping' left to right across the plane.
// - The sweep hits each vertical line once, but hits each horizontal line at its left end point,
//   and leaves it at its right end point:

// - Set up a list of endpoints:
//   - each vertical line is represented once
//   - each horizontal line is represented twice, by its left and right end points.
// - Sort the list by x-coordinate
// - Assume that no two horizontal lines intersect, and that no two vertical lines intersect.
// - Simulate the line-sweep by 'processing' the list of endpoints
// - Maintain throughout a set of candidate horizontal lines (those whose left end point has been processed
//   but whose right end point has not)
// - When a vertical line is encountered during the sweep, consider only the candidate lines for intersections
// - This will result in a real speed-up in many cases, but could still have as many as hv comparisons

// TODO actually create the thing


} // end namespace problem5

