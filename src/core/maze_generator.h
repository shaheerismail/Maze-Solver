#pragma once
// MazeGenerator — recursive backtracking maze carving.
// It holds the caller's maze by reference. generate() first closes all walls
// (reset) and then carves a perfect maze, records every carved wall into a
// CarveStepList (for GUI replay), and opens the entrance/exit to the outside.

#include "maze_matrix.h"
#include "rng.h"
#include "carve_step_list.h"
#include "steps.h"

// Depth-first maze carver driven by the seeded RandomLCG.
class MazeGenerator {
public:
    MazeGenerator(MazeMatrix& maze, unsigned int seed); // bind maze + carving seed

    void generate();                     // perfect maze + entrance/exit openings
    void openExtraPassages(int count);   // random interior walls removed ('ExtraOpening' steps);
                                         // same (seed, count) pair -> same result

    const CarveStepList& steps() const;  // recorded wall openings of the last run

private:
    MazeMatrix& maze_;        // the maze being carved (caller-owned)
    RandomLCG rng_;           // seeded random stream for reproducible mazes
    CarveStepList steps_;     // every wall opened, in carving order

    void carve(int row, int col);        // recursive backtracking body
};
