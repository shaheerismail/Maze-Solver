#pragma once
// MazeGraph — graph view of a MazeMatrix. One vertex per cell
// (vertex id = row * cols + col); one (two-way) edge per open interior
// passage. Adjacency lists are the project's own IntLinkedList.

#include "maze_matrix.h"
#include "int_linked_list.h"

// Graph over the maze grid: one vertex per cell, adjacency list per vertex.
class MazeGraph {
public:
    MazeGraph();                                   // empty graph, no vertices
    MazeGraph(const MazeGraph&) = delete;          // rebuilt rather than copied
    MazeGraph& operator=(const MazeGraph&) = delete;
    ~MazeGraph();                                  // frees the adjacency arrays

    void buildFromMatrix(const MazeMatrix& maze);  // frees the previous lists first
    int vertexCount() const;                       // number of cells (vertices)
    int rowOf(int vertex) const;                   // vertex == row * cols + col
    int colOf(int vertex) const;                   // column half of the id split
    int rows() const;                              // maze height in cells
    int cols() const;                              // maze width in cells
    const IntLinkedList& neighbors(int vertex) const; // ascending neighbour ids

private:
    int rows_;                     // maze height of the built graph
    int cols_;                     // maze width of the built graph
    int count_;                    // rows_ * cols_, the vertex count
    IntLinkedList* adjacency_;     // new IntLinkedList[count_]; delete[] on rebuild/death

    void freeAdjacency();          // helper: release the array and zero the sizes
};
