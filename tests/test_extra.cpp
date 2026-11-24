#include "test_util.h"
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_graph.h"
#include "bfs_solver.h"

static int openInteriorPassages(const MazeMatrix& m)
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

void testExtra()
{


    MazeMatrix m;
    m.reset(10, 10);
    MazeGenerator g(m, 21);
    g.generate();
    CHECK_EQ(openInteriorPassages(m), 99);
    g.openExtraPassages(5);
    CHECK_EQ(openInteriorPassages(m), 104);
    MazeGraph graph;
    graph.buildFromMatrix(m);
    BfsSolver s;
    IntLinkedList route;
    CHECK(s.solve(graph, 0, 99, route));
    CHECK(!route.isEmpty());


    MazeMatrix small;
    small.reset(2, 2);
    MazeGenerator gs(small, 4);
    gs.generate();
    gs.openExtraPassages(10);
    CHECK_EQ(openInteriorPassages(small), 4);
    MazeMatrix tiny;
    tiny.reset(1, 1);
    MazeGenerator gt(tiny, 4);
    gt.generate();
    gt.openExtraPassages(3);
    CHECK_EQ(openInteriorPassages(tiny), 0);


    MazeMatrix m1;
    m1.reset(12, 12);
    MazeGenerator a(m1, 77);
    a.generate();
    a.openExtraPassages(5);
    MazeMatrix m2;
    m2.reset(12, 12);
    MazeGenerator b(m2, 77);
    b.generate();
    b.openExtraPassages(5);
    CHECK_EQ(m1.toWallString(), m2.toWallString());
    MazeMatrix m3;
    m3.reset(12, 12);
    MazeGenerator c(m3, 77);
    c.generate();
    c.openExtraPassages(8);
    CHECK(m1.toWallString() != m3.toWallString());


    MazeMatrix beforeM;
    beforeM.reset(15, 15);
    MazeGenerator gb(beforeM, 33);
    gb.generate();
    MazeGraph graphBefore;
    graphBefore.buildFromMatrix(beforeM);
    BfsSolver sb;
    IntLinkedList rb;
    CHECK(sb.solve(graphBefore, 0, 224, rb));
    int lengthBefore = rb.size();

    MazeMatrix afterM;
    afterM.reset(15, 15);
    MazeGenerator ga(afterM, 33);
    ga.generate();
    ga.openExtraPassages(10);
    MazeGraph graphAfter;
    graphAfter.buildFromMatrix(afterM);
    BfsSolver sa;
    IntLinkedList ra;
    CHECK(sa.solve(graphAfter, 0, 224, ra));
    CHECK((int)ra.size() <= lengthBefore);
    CHECK((int)ra.size() >= 0);
}
