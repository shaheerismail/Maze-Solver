#pragma once




#include <string>

struct Cell {
    unsigned char walls;
    bool visited;
};

class MazeMatrix {
public:
    static const int WALL_N = 1;
    static const int WALL_E = 2;
    static const int WALL_S = 4;
    static const int WALL_W = 8;
    static const int MAX_ROWS = 50;
    static const int MAX_COLS = 50;

    MazeMatrix();
    MazeMatrix(const MazeMatrix& other);
    MazeMatrix& operator=(const MazeMatrix& other);
    ~MazeMatrix();


    static bool isValidSize(int rows, int cols);

    static void sideDelta(int side, int& drow, int& dcol);



    bool reset(int rows, int cols);

    int rows() const;
    int cols() const;
    bool inBounds(int row, int col) const;
    bool hasWall(int row, int col, int side) const;
    void removeWall(int row, int col, int side);
    void openOutside(int row, int col, int side);
    void setVisited(int row, int col, bool value);
    bool isVisited(int row, int col) const;
    void clearVisited();

    std::string toAscii() const;
    std::string toWallString() const;
    static MazeMatrix fromWallString(int rows, int cols, const std::string& hex);



private:
    int rows_;
    int cols_;
    Cell** cells_;

    void freeCells();
    void copyFrom(const MazeMatrix& other);
    static int hexValue(char c);
};
