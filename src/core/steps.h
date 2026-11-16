#pragma once
// Shared plain structs used by the core and passed to the GUI.
// No Qt here — these are plain C++ data.

#include <string>

// One wall opened during maze generation: a carved passage or an extra opening.
struct CarveStep {
    enum Kind { Carve, ExtraOpening } kind;
    int row, col, side;   // wall opened at (row,col) towards 'side'
};

// One step of the BFS animation: a vertex dequeued, or an edge examined.
struct SolveStep {
    enum Kind { Dequeued, EdgeChecked, Found } kind;
    int vertex;      // dequeued vertex, or owner vertex of the checked edge
    int neighbor;    // the far end of the edge, valid when kind == EdgeChecked
    bool discovered; // whether this edge enqueued a new vertex
};

// One saved maze in the BST library (and in the save file).
struct MazeRecord {
    unsigned int seed;
    int rows;
    int cols;
    int extraPassages;
    std::string walls;   // rows*cols hex digits, row-major
};
