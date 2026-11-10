#include "test_util.h"
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_graph.h"

// graph side of the BFS suite; the bfs part is added in the next phase
static int matrixInteriorEdges(const MazeMatrix& m)
{
    int count = 0;
    for (int r = 0; r < m.rows(); ++r) {
        for (int c = 0; c < m.cols(); ++c) {
            if (r > 0 && !m.hasWall(r, c, MazeMatrix::WALL_N))
                ++count;
            if (c > 0 && !m.hasWall(r, c, MazeMatrix::WALL_W))
                ++count;
        }
    }
    return count;
}

void testGraph()
{
    // T16: one vertex per cell; every edge mirrored in both adjacency lists
    MazeMatrix m;
    m.reset(5, 5);
    MazeGenerator g(m, 11);
    g.generate();
    MazeGraph graph;
    graph.buildFromMatrix(m);
    CHECK_EQ(graph.vertexCount(), 25);
    CHECK_EQ(graph.rowOf(7), 1);
    CHECK_EQ(graph.colOf(7), 2);
    for (int v = 0; v < 25; ++v) {
        LinkedList<int>::Iterator it = graph.neighbors(v).iterator();
        while (it.hasNext()) {
            int u = it.next();
            bool mirrored = false;
            LinkedList<int>::Iterator back = graph.neighbors(u).iterator();
            while (back.hasNext())
                if (back.next() == v)
                    mirrored = true;
            CHECK(mirrored);
        }
    }

    // T17: sum of degrees == 2 x interior edge count of the matrix
    int degreeSum = 0;
    for (int v = 0; v < 25; ++v)
        degreeSum += graph.neighbors(v).size();
    CHECK_EQ(degreeSum, 2 * matrixInteriorEdges(m));

    // T18: rebuild picks up the new walls, no stale lists
    MazeGenerator g2(m, 12);
    g2.generate();
    graph.buildFromMatrix(m);
    CHECK_EQ(graph.vertexCount(), 25);
    degreeSum = 0;
    for (int v = 0; v < 25; ++v)
        degreeSum += graph.neighbors(v).size();
    CHECK_EQ(degreeSum, 2 * matrixInteriorEdges(m));

    // empty matrix -> empty graph
    MazeMatrix none;
    MazeGraph empty;
    empty.buildFromMatrix(none);
    CHECK_EQ(empty.vertexCount(), 0);
    CHECK_EQ(empty.neighbors(0).size(), 0);
}
