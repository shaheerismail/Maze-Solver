#pragma once




#include "maze_matrix.h"
#include "int_linked_list.h"


class MazeGraph {
public:
    MazeGraph();
    MazeGraph(const MazeGraph&) = delete;
    MazeGraph& operator=(const MazeGraph&) = delete;
    ~MazeGraph();

    void buildFromMatrix(const MazeMatrix& maze);
    int vertexCount() const;
    int rowOf(int vertex) const;
    int colOf(int vertex) const;
    int rows() const;
    int cols() const;
    const IntLinkedList& neighbors(int vertex) const;

private:
    int rows_;
    int cols_;
    int count_;
    IntLinkedList* adjacency_;

    void freeAdjacency();
};
