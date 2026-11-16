// MazeGraph implementation. Vertex ids are row * cols + col; adjacency lists
// are rebuilt from scratch on every buildFromMatrix call.

#include "maze_graph.h"

// An empty graph has zero vertices and no adjacency array yet.
MazeGraph::MazeGraph() : rows_(0), cols_(0), count_(0), adjacency_(0) {}

// Release the adjacency array and reset the graph to empty.
void MazeGraph::freeAdjacency()
{
    if (adjacency_ != 0) {
        delete[] adjacency_;
        adjacency_ = 0;
    }
    rows_ = 0;
    cols_ = 0;
    count_ = 0;
}

// Destructor: the array destructor also frees every IntLinkedList inside.
MazeGraph::~MazeGraph()
{
    freeAdjacency();
}

// Build one adjacency list per cell by scanning only E and S wall facets, so
// every interior passage is visited exactly once and stored in BOTH lists.
void MazeGraph::buildFromMatrix(const MazeMatrix& maze)
{
    freeAdjacency();
    rows_ = maze.rows();
    cols_ = maze.cols();
    count_ = rows_ * cols_;
    if (count_ == 0)
        return;
    adjacency_ = new IntLinkedList[count_];
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            int v = r * cols_ + c;
            if (!maze.hasWall(r, c, MazeMatrix::WALL_E) && maze.inBounds(r, c + 1)) {
                adjacency_[v].pushBack(v + 1);
                adjacency_[v + 1].pushBack(v);
            }
            if (!maze.hasWall(r, c, MazeMatrix::WALL_S) && maze.inBounds(r + 1, c)) {
                adjacency_[v].pushBack(v + cols_);
                adjacency_[v + cols_].pushBack(v);
            }
        }
    }
}

// Number of vertices (one per cell).
int MazeGraph::vertexCount() const { return count_; }

// Row of a vertex id (integer division by the maze width).
int MazeGraph::rowOf(int vertex) const { return vertex / cols_; }

// Column of a vertex id (remainder of the division by the maze width).
int MazeGraph::colOf(int vertex) const { return vertex % cols_; }

// Maze height in cells.
int MazeGraph::rows() const { return rows_; }

// Maze width in cells.
int MazeGraph::cols() const { return cols_; }

// Adjacency list of one vertex; a shared empty list is returned for bad ids.
const IntLinkedList& MazeGraph::neighbors(int vertex) const
{
    static IntLinkedList empty;   // shared read-only fallback for bad ids
    if (vertex < 0 || vertex >= count_)
        return empty;
    return adjacency_[vertex];
}
