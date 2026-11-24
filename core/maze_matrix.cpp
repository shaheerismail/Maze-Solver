



#include "maze_matrix.h"

MazeMatrix::MazeMatrix() : rows_(0), cols_(0), cells_(0) {}

void MazeMatrix::freeCells()
{
    if (cells_ != 0) {
        for (int r = 0; r < rows_; ++r)
            delete[] cells_[r];
        delete[] cells_;
        cells_ = 0;
    }
    rows_ = 0;
    cols_ = 0;
}

void MazeMatrix::copyFrom(const MazeMatrix& other)
{
    freeCells();
    rows_ = other.rows_;
    cols_ = other.cols_;
    if (rows_ == 0) {
        cells_ = 0;
        return;
    }
    cells_ = new Cell*[rows_];
    for (int r = 0; r < rows_; ++r) {
        cells_[r] = new Cell[cols_];
        for (int c = 0; c < cols_; ++c)
            cells_[r][c] = other.cells_[r][c];
    }
}

MazeMatrix::MazeMatrix(const MazeMatrix& other) : rows_(0), cols_(0), cells_(0)
{
    copyFrom(other);
}

MazeMatrix& MazeMatrix::operator=(const MazeMatrix& other)
{
    if (this != &other)
        copyFrom(other);
    return *this;
}

MazeMatrix::~MazeMatrix()
{
    freeCells();
}

bool MazeMatrix::isValidSize(int rows, int cols)
{
    return rows >= 1 && rows <= MAX_ROWS && cols >= 1 && cols <= MAX_COLS;
}

void MazeMatrix::sideDelta(int side, int& drow, int& dcol)
{
    drow = 0;
    dcol = 0;
    if (side == WALL_N) {
        drow = -1;
    } else if (side == WALL_E) {
        dcol = 1;
    } else if (side == WALL_S) {
        drow = 1;
    } else if (side == WALL_W) {
        dcol = -1;
    }
}

bool MazeMatrix::reset(int rows, int cols)
{
    if (!isValidSize(rows, cols))
        return false;
    if (rows != rows_ || cols != cols_) {
        freeCells();
        rows_ = rows;
        cols_ = cols;
        cells_ = new Cell*[rows_];
        for (int r = 0; r < rows_; ++r)
            cells_[r] = new Cell[cols_];
    }
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            cells_[r][c].walls = WALL_N | WALL_E | WALL_S | WALL_W;
            cells_[r][c].visited = false;
        }
    }
    return true;
}

int MazeMatrix::rows() const { return rows_; }
int MazeMatrix::cols() const { return cols_; }

bool MazeMatrix::inBounds(int row, int col) const
{
    return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

bool MazeMatrix::hasWall(int row, int col, int side) const
{
    if (!inBounds(row, col))
        return false;
    return (cells_[row][col].walls & side) != 0;
}

void MazeMatrix::removeWall(int row, int col, int side)
{
    if (!inBounds(row, col))
        return;
    cells_[row][col].walls &= (unsigned char)~side;
    int drow;
    int dcol;
    sideDelta(side, drow, dcol);
    int nr = row + drow;
    int nc = col + dcol;
    if (inBounds(nr, nc)) {

        int opposite = (side == WALL_N) ? WALL_S
                     : (side == WALL_S) ? WALL_N
                     : (side == WALL_E) ? WALL_W
                                        : WALL_E;
        cells_[nr][nc].walls &= (unsigned char)~opposite;
    }
}

void MazeMatrix::openOutside(int row, int col, int side)
{
    if (!inBounds(row, col))
        return;
    cells_[row][col].walls &= (unsigned char)~side;
}

void MazeMatrix::setVisited(int row, int col, bool value)
{
    if (inBounds(row, col))
        cells_[row][col].visited = value;
}

bool MazeMatrix::isVisited(int row, int col) const
{
    if (!inBounds(row, col))
        return false;
    return cells_[row][col].visited;
}

void MazeMatrix::clearVisited()
{
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            cells_[r][c].visited = false;
}

std::string MazeMatrix::toAscii() const
{
    std::string out;
    if (rows_ == 0)
        return out;
    for (int c = 0; c < cols_; ++c) {
        out += '+';
        out += hasWall(0, c, WALL_N) ? "---" : "   ";
    }
    out += "+\n";
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            out += hasWall(r, c, WALL_W) ? "|" : " ";
            out += "   ";
        }
        out += hasWall(r, cols_ - 1, WALL_E) ? "|" : " ";
        out += '\n';
        for (int c = 0; c < cols_; ++c) {
            out += '+';
            out += hasWall(r, c, WALL_S) ? "---" : "   ";
        }
        out += "+\n";
    }
    return out;
}

std::string MazeMatrix::toWallString() const
{
    std::string out;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            unsigned char w = cells_[r][c].walls;
            out += w < 10 ? (char)('0' + w) : (char)('A' + (w - 10));
        }
    }
    return out;
}

MazeMatrix MazeMatrix::fromWallString(int rows, int cols, const std::string& hex)
{
    MazeMatrix m;
    m.reset(rows, cols);
    int i = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            m.cells_[r][c].walls = (unsigned char)hexValue(hex[i]);
            ++i;
        }
    }
    return m;
}

int MazeMatrix::hexValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    return 10 + c - 'a';
}
