#pragma once
// MazeMatrix — the project's 2-D maze, a dynamically allocated array of Cells.
// Each Cell stores its four passage walls as a bitmask (bit set = wall present)
// and a visited flag used by the generator. Pure C++: no Qt, no library containers.

#include <string>

struct Cell {
    unsigned char walls;  // bit set = wall present: N=1 E=2 S=4 W=8
    bool visited;
};

class MazeMatrix {
public:
    static const int WALL_N = 1;
    static const int WALL_E = 2;
    static const int WALL_S = 4;
    static const int WALL_W = 8;
    static const int MAX_ROWS = 50;  // cap keeps the recursive generator's depth safe
    static const int MAX_COLS = 50;

    MazeMatrix();
    MazeMatrix(const MazeMatrix& other);             // deep copy
    MazeMatrix& operator=(const MazeMatrix& other);  // deep copy
    ~MazeMatrix();

    // The only size rule in the project: 1..MAX_ROWS by 1..MAX_COLS.
    static bool isValidSize(int rows, int cols);
    // Writes the row/col step for a side: N=(-1,0), E=(0,+1), S=(+1,0), W=(0,-1).
    static void sideDelta(int side, int& drow, int& dcol);

    // Reallocates (or rezeros) a closed all-wall grid. Returns false and
    // leaves the matrix byte-for-byte unchanged for invalid sizes — no exceptions.
    bool reset(int rows, int cols);

    int rows() const;
    int cols() const;
    bool inBounds(int row, int col) const;
    bool hasWall(int row, int col, int side) const;
    void removeWall(int row, int col, int side);   // also clears the neighbour's opposite bit
    void openOutside(int row, int col, int side);  // clears one bit only — entrance/exit
    void setVisited(int row, int col, bool value);
    bool isVisited(int row, int col) const;
    void clearVisited();

    std::string toAscii() const;      // '+' '-' '|' art for the console tests / demo
    std::string toWallString() const; // one hex char per cell, row-major
    static MazeMatrix fromWallString(int rows, int cols, const std::string& hex);
    // precondition (documented): hex already validated by MazeIO —
    // exactly rows*cols hex digits. MazeIO is the single validation point.

private:
    int rows_;
    int cols_;
    Cell** cells_;   // new Cell*[rows_] then new Cell[cols_] per row; sole owner

    void freeCells();
    void copyFrom(const MazeMatrix& other);
    static int hexValue(char c);
};
