#pragma once
// BfsSolver — breadth-first search on a MazeGraph. Every dequeue and edge
// check is recorded as a SolveStep so the GUI can replay the search without
// running any algorithm per frame.

#include "maze_graph.h"
#include "queue.h"
#include "linked_list.h"
#include "steps.h"

class BfsSolver {
public:
    BfsSolver() {}

    // Returns true iff a route exists. On success outRoute holds the vertex
    // ids from source (entrance) to target (exit). When no route exists the
    // queue drains to empty and outRoute stays empty.
    bool solve(const MazeGraph& graph, int source, int target, LinkedList<int>& outRoute);

    const LinkedList<SolveStep>& steps() const;

private:
    LinkedList<SolveStep> steps_;
};
