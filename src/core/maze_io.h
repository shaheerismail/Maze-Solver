#pragma once
// MazeIO — text save/load for mazes. Format (see docs/PLAN.md §6):
//   MAZE_V1
//   rows R
//   cols C
//   seed S
//   extra E
//   <R grid lines of C single-hex-digit cells, space separated>
// Parsing is hand-written (no strtol/atoi/sscanf/cctype) and fail-atomic:
// the result is a LoadResult temporary that callers swap in only when ok.

#include "maze_matrix.h"
#include <string>

struct LoadResult {
    bool ok = false;
    std::string error;   // human-readable, line numbers where useful
    MazeMatrix maze;     // meaningful only when ok
    unsigned int seed = 0;
    int extraPassages = 0;
};

class MazeIO {
public:
    static bool saveToFile(const std::string& path, const MazeMatrix& maze,
                           unsigned int seed, int extraPassages, std::string& errorOut);
    static std::string saveToString(const MazeMatrix& maze, unsigned int seed, int extraPassages);
    static LoadResult loadFromString(const std::string& text);
    static LoadResult loadFromFile(const std::string& path);

private:
    MazeIO() {}

    static std::string toString(int value);
    static bool parseNonNegativeInt(const std::string& token, int& outValue);  // digit loop, overflow-safe
    static bool isDigitChar(char c);
    static bool isHexCellChar(char c);
    static bool isSpaceChar(char c);
};
