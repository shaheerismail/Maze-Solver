#pragma once
// MazeGraph — graph view of a MazeMatrix. One vertex per cell
// (vertex id = row * cols + col); one (two-way) edge per open interior
// passage. Adjacency lists are the project's own LinkedList<int>.

#include "maze_matrix.h"
#include "linked_list.h"

class MazeGraph {
public:
    MazeGraph();
    MazeGraph(const MazeGraph&) = delete;            // rebuilt rather than copied
    MazeGraph& operator=(const MazeGraph&) = delete;
    ~MazeGraph();

    void buildFromMatrix(const MazeMatrix& maze);    // frees the previous lists first
    int vertexCount() const;
    int rowOf(int vertex) const;                     // vertex == row * cols + col
    int colOf(int vertex) const;
    int rows() const;
    int cols() const;
    const LinkedList<int>& neighbors(int vertex) const;   // ascending ids

private:
    int rows_;
    int cols_;
    int count_;
    LinkedList<int>* adjacency_;   // new LinkedList<int>[count_]; delete[] on rebuild/death

    void freeAdjacency();
};
