#include "maze_graph.h"

MazeGraph::MazeGraph() : rows_(0), cols_(0), count_(0), adjacency_(0) {}

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

MazeGraph::~MazeGraph()
{
    freeAdjacency();
}

void MazeGraph::buildFromMatrix(const MazeMatrix& maze)
{
    freeAdjacency();
    rows_ = maze.rows();
    cols_ = maze.cols();
    count_ = rows_ * cols_;
    if (count_ == 0)
        return;
    adjacency_ = new LinkedList<int>[count_];
    // Scan only the E and S facets of every cell: each interior passage is
    // visited exactly once and inserted into BOTH adjacency lists.
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

int MazeGraph::vertexCount() const { return count_; }
int MazeGraph::rowOf(int vertex) const { return vertex / cols_; }
int MazeGraph::colOf(int vertex) const { return vertex % cols_; }
int MazeGraph::rows() const { return rows_; }
int MazeGraph::cols() const { return cols_; }

const LinkedList<int>& MazeGraph::neighbors(int vertex) const
{
    static LinkedList<int> empty;   // shared read-only fallback for bad ids
    if (vertex < 0 || vertex >= count_)
        return empty;
    return adjacency_[vertex];
}
