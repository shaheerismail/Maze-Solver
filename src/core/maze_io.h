#pragma once










#include "maze_matrix.h"
#include <string>

struct LoadResult {
    bool ok = false;
    std::string error;
    MazeMatrix maze;
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
    static bool parseNonNegativeInt(const std::string& token, int& outValue);
    static bool isDigitChar(char c);
    static bool isHexCellChar(char c);
    static bool isSpaceChar(char c);
};
