#pragma once





#include "maze_matrix.h"
#include "rng.h"
#include "carve_step_list.h"
#include "steps.h"


class MazeGenerator {
public:
    MazeGenerator(MazeMatrix& maze, unsigned int seed);

    void generate();
    void openExtraPassages(int count);


    const CarveStepList& steps() const;

private:
    MazeMatrix& maze_;
    RandomLCG rng_;
    CarveStepList steps_;

    void carve(int row, int col);
};
