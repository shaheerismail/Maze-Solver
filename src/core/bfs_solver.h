#pragma once





#include "maze_graph.h"
#include "int_linked_queue.h"
#include "int_linked_list.h"
#include "solve_step_list.h"
#include "steps.h"


class BfsSolver {
public:
    BfsSolver() {}






    bool solve(const MazeGraph& graph, int source, int target, IntLinkedList& outRoute,
               const IntLinkedList* blockedVertices = 0);

    const SolveStepList& steps() const;

private:
    SolveStepList steps_;
};
