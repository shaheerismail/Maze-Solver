#pragma once
// BfsSolver — breadth-first search on a MazeGraph. Every dequeue and edge
// check is recorded as a SolveStep so the GUI can replay the search without
// running any algorithm per frame. Uses the standalone IntLinkedQueue as the
// BFS frontier and IntLinkedList for routes and blocked-vertex sets.

#include "maze_graph.h"
#include "int_linked_queue.h"
#include "int_linked_list.h"
#include "solve_step_list.h"
#include "steps.h"

// Breadth-first shortest-route search over the maze graph.
class BfsSolver {
public:
    BfsSolver() {}   // no state beyond the step list, nothing to initialise

    // Returns true iff a route exists. On success outRoute holds the vertex
    // ids from source (entrance) to target (exit). When no route exists the
    // queue drains to empty and outRoute stays empty.
    // blockedVertices (optional): BFS treats these ids as impassable cells —
    // used by the game layer for mine placement and mine-aware solving.
    bool solve(const MazeGraph& graph, int source, int target, IntLinkedList& outRoute,
               const IntLinkedList* blockedVertices = 0);

    const SolveStepList& steps() const;  // recorded dequeue/edge steps of the last solve

private:
    SolveStepList steps_;  // animation record of the last solve call
};
