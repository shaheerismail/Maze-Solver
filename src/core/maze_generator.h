#pragma once
// MazeGenerator — recursive backtracking maze carving.
// It holds the caller's maze by reference. generate() first closes all walls
// (reset) and then carves a perfect maze, records every carved wall into a
// step list (for GUI replay), and opens the entrance/exit to the outside.

#include "maze_matrix.h"
#include "rng.h"
#include "linked_list.h"
#include "steps.h"

class MazeGenerator {
public:
    MazeGenerator(MazeMatrix& maze, unsigned int seed);

    void generate();                     // perfect maze + entrance/exit openings
    void openExtraPassages(int count);   // random interior walls removed ('Extra' steps);
                                         // same (seed, count) pair -> same result

    const LinkedList<GenStep>& steps() const;

private:
    MazeMatrix& maze_;
    RandomLCG rng_;
    LinkedList<GenStep> steps_;

    void carve(int row, int col);        // recursive backtracking body
};
