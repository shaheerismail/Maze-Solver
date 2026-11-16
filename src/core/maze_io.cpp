#include "maze_io.h"
#include "string_list.h"
#include <fstream>

// Decimal text for a non-negative or negative int, built digit by digit.
std::string MazeIO::toString(int value)
{
    if (value == 0)
        return "0";
    std::string out;
    if (value < 0) {
        out += '-';
        value = -value;
    }
    int digits[12];
    int n = 0;
    while (value > 0) {
        digits[n++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (n > 0)
        out += digits[--n];
    return out;
}

bool MazeIO::isDigitChar(char c)
{
    return c >= '0' && c <= '9';
}

bool MazeIO::isHexCellChar(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

bool MazeIO::isSpaceChar(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool MazeIO::parseNonNegativeInt(const std::string& token, int& outValue)
{
    if (token.empty())
        return false;
    int value = 0;
    for (int i = 0; i < (int)token.size(); ++i) {
        if (!isDigitChar(token[i]))
            return false;
        if (value > 100000000)   // overflow guard; sizes/seeds stay far below
            return false;
        value = value * 10 + (token[i] - '0');
    }
    outValue = value;
    return true;
}

std::string MazeIO::saveToString(const MazeMatrix& maze, unsigned int seed, int extraPassages)
{
    std::string out = "MAZE_V1\n";
    out += "rows " + toString(maze.rows()) + "\n";
    out += "cols " + toString(maze.cols()) + "\n";
    out += "seed " + toString((int)seed) + "\n";   // GUI caps seeds at INT_MAX
    out += "extra " + toString(extraPassages) + "\n";
    std::string cells = maze.toWallString();
    int i = 0;
    for (int r = 0; r < maze.rows(); ++r) {
        for (int c = 0; c < maze.cols(); ++c) {
            if (c > 0)
                out += ' ';
            out += cells[i];
            ++i;
        }
        out += '\n';
    }
    return out;
}

bool MazeIO::saveToFile(const std::string& path, const MazeMatrix& maze,
                        unsigned int seed, int extraPassages, std::string& errorOut)
{
    std::ofstream file(path.c_str());
    if (!file.is_open()) {
        errorOut = "cannot open file for writing";
        return false;
    }
    file << saveToString(maze, seed, extraPassages);
    if (!file.good()) {
        errorOut = "error while writing the file";
        return false;
    }
    errorOut.clear();
    return true;
}

LoadResult MazeIO::loadFromFile(const std::string& path)
{
    LoadResult result;   // ok = false by default
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        result.error = "cannot open file for reading";
        return result;
    }
    std::string text;
    std::string line;
    while (getline(file, line))
        text += line + "\n";
    return loadFromString(text);
}

LoadResult MazeIO::loadFromString(const std::string& text)
{
    LoadResult result;
    result.seed = 0;
    result.extraPassages = 0;

// Split into lines, dropping a trailing '\r' from CRLF files
    StringList lines;
    std::string current;
    for (int i = 0; i < (int)text.size(); ++i) {
        char ch = text[i];
        if (ch == '\n') {
            if (!current.empty() && current[current.size() - 1] == '\r')
                current.resize(current.size() - 1);
            lines.pushBack(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    if (!current.empty()) {   // final line without newline
        if (current[current.size() - 1] == '\r')
            current.resize(current.size() - 1);
        lines.pushBack(current);
    }

    int lineNumber = 0;

    // 1) magic
    std::string magic;
    if (!lines.popFront(magic)) {
        result.error = "empty file";
        return result;
    }
    ++lineNumber;
    if (magic != "MAZE_V1") {
        result.error = "bad magic on line 1 (expected MAZE_V1)";
        return result;
    }

    // 2..5) ordered header: rows, cols, seed, extra
    int header[4] = { 0, 0, 0, 0 };
    const char* keys[4] = { "rows", "cols", "seed", "extra" };
    for (int h = 0; h < 4; ++h) {
        std::string line;
        if (!lines.popFront(line)) {
            result.error = std::string("missing '") + keys[h] + "' header line";
            return result;
        }
        ++lineNumber;
        int split = -1;
        for (int i = 0; i < (int)line.size(); ++i) {
            if (line[i] == ' ') {
                split = i;
                break;
            }
        }
        if (split <= 0) {
            result.error = std::string("malformed line ") + toString(lineNumber);
            return result;
        }
        if (line.substr(0, split) != keys[h]) {
            result.error = std::string("expected '") + keys[h] + "' on line " +
                           toString(lineNumber);
            return result;
        }
        if (!parseNonNegativeInt(line.substr(split + 1), header[h])) {
            result.error = std::string("bad ") + keys[h] + " value on line " +
                           toString(lineNumber);
            return result;
        }
    }

    if (!MazeMatrix::isValidSize(header[0], header[1])) {
        result.error = "invalid size (rows and cols must be 1..50)";
        return result;
    }

    // 6) grid: exactly rows lines with cols single-hex-digit tokens each
    std::string cells;
    for (int r = 0; r < header[0]; ++r) {
        std::string row;
        if (!lines.popFront(row)) {
            result.error = "truncated grid";
            return result;
        }
        ++lineNumber;
        int tokenCount = 0;
        int i = 0;
        while (i < (int)row.size()) {
            if (isSpaceChar(row[i])) {   // runs of whitespace are separators
                ++i;
                continue;
            }
            if (!isHexCellChar(row[i])) {
                result.error = "bad hex cell on line " + toString(lineNumber);
                return result;
            }
            if (i + 1 < (int)row.size() && !isSpaceChar(row[i + 1])) {
                result.error = "cell must be a single hex digit on line " +
                               toString(lineNumber);
                return result;
            }
            cells += row[i];
            ++tokenCount;
            ++i;
        }
        if (tokenCount != header[1]) {
            result.error = "expected " + toString(header[1]) + " cells on line " +
                           toString(lineNumber) + ", found " + toString(tokenCount);
            return result;
        }
    }
    if ((int)cells.size() != header[0] * header[1]) {
        result.error = "grid does not match the maze size";
        return result;
    }

    // 7) nothing but whitespace after the grid
    std::string tail;
    while (lines.popFront(tail)) {
        ++lineNumber;
        for (int i = 0; i < (int)tail.size(); ++i) {
            if (!isSpaceChar(tail[i])) {
                result.error = "unexpected content after the grid on line " +
                               toString(lineNumber);
                return result;
            }
        }
    }

    result.maze = MazeMatrix::fromWallString(header[0], header[1], cells);
    result.seed = (unsigned int)header[2];
    result.extraPassages = header[3];
    result.ok = true;
    return result;
}
