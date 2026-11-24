#pragma once



#include <string>


struct CarveStep {
    enum Kind { Carve, ExtraOpening } kind;
    int row, col, side;
};


struct SolveStep {
    enum Kind { Dequeued, EdgeChecked, Found } kind;
    int vertex;
    int neighbor;
    bool discovered;
};


struct MazeRecord {
    unsigned int seed;
    int rows;
    int cols;
    int extraPassages;
    std::string walls;
};
