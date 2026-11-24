#include "test_util.h"
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_graph.h"
#include "bfs_solver.h"


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
        IntLinkedList::Cursor it = graph.neighbors(v).cursor();
        while (it.hasNext()) {
            int u = it.next();
            bool mirrored = false;
            IntLinkedList::Cursor back = graph.neighbors(u).cursor();
            while (back.hasNext())
                if (back.next() == v)
                    mirrored = true;
            CHECK(mirrored);
        }
    }


    int degreeSum = 0;
    for (int v = 0; v < 25; ++v)
        degreeSum += graph.neighbors(v).size();
    CHECK_EQ(degreeSum, 2 * matrixInteriorEdges(m));


    MazeGenerator g2(m, 12);
    g2.generate();
    graph.buildFromMatrix(m);
    CHECK_EQ(graph.vertexCount(), 25);
    degreeSum = 0;
    for (int v = 0; v < 25; ++v)
        degreeSum += graph.neighbors(v).size();
    CHECK_EQ(degreeSum, 2 * matrixInteriorEdges(m));


    MazeMatrix none;
    MazeGraph empty;
    empty.buildFromMatrix(none);
    CHECK_EQ(empty.vertexCount(), 0);
    CHECK_EQ(empty.neighbors(0).size(), 0);
}





static bool routeIsValid(const MazeGraph& graph, const IntLinkedList& route)
{
    if (route.isEmpty())
        return false;
    bool seen[2500];
    for (int i = 0; i < 2500; ++i)
        seen[i] = false;

    IntLinkedList::Cursor it = route.cursor();
    int prev = it.next();
    seen[prev] = true;
    while (it.hasNext()) {
        int v = it.next();
        if (seen[v])
            return false;
        seen[v] = true;
        int dr = graph.rowOf(v) - graph.rowOf(prev);
        int dc = graph.colOf(v) - graph.colOf(prev);
        bool adjacent = (dr == 0 && (dc == 1 || dc == -1)) ||
                        (dc == 0 && (dr == 1 || dr == -1));
        if (!adjacent)
            return false;
        prev = v;
    }
    return true;
}

void testBfs()
{




    MazeMatrix m;
    m.reset(3, 3);
    m.removeWall(0, 0, MazeMatrix::WALL_E);
    m.removeWall(0, 1, MazeMatrix::WALL_E);
    m.removeWall(0, 2, MazeMatrix::WALL_S);
    m.removeWall(1, 2, MazeMatrix::WALL_W);
    m.removeWall(1, 1, MazeMatrix::WALL_W);
    m.removeWall(1, 0, MazeMatrix::WALL_S);
    m.removeWall(2, 0, MazeMatrix::WALL_E);
    m.removeWall(2, 1, MazeMatrix::WALL_E);
    MazeGraph graph;
    graph.buildFromMatrix(m);
    BfsSolver solver;
    IntLinkedList route;
    CHECK(solver.solve(graph, 0, 8, route));
    CHECK_EQ(route.size(), 9);
    int expected[9] = { 0, 1, 2, 5, 4, 3, 6, 7, 8 };
    IntLinkedList::Cursor it = route.cursor();
    for (int i = 0; i < 9; ++i)
        CHECK_EQ(it.next(), expected[i]);


    MazeMatrix gen;
    gen.reset(10, 10);
    MazeGenerator g(gen, 3);
    g.generate();
    g.openExtraPassages(7);
    MazeGraph gg;
    gg.buildFromMatrix(gen);
    BfsSolver s2;
    IntLinkedList route2;
    CHECK(s2.solve(gg, 0, 99, route2));
    IntLinkedList::Cursor itFirst = route2.cursor();
    int first = itFirst.next();
    int last = first;
    while (itFirst.hasNext())
        last = itFirst.next();
    CHECK_EQ(first, 0);
    CHECK_EQ(last, 99);
    CHECK(routeIsValid(gg, route2));


    MazeMatrix blocked;
    blocked.reset(2, 2);
    blocked.openOutside(0, 0, MazeMatrix::WALL_N);
    blocked.openOutside(1, 1, MazeMatrix::WALL_S);
    MazeGraph bg;
    bg.buildFromMatrix(blocked);
    BfsSolver s3;
    IntLinkedList route3;
    CHECK(!s3.solve(bg, 0, 3, route3));
    CHECK(route3.isEmpty());


    MazeMatrix big;
    big.reset(50, 50);
    MazeGenerator gbig(big, 5);
    gbig.generate();
    MazeGraph gb;
    gb.buildFromMatrix(big);
    BfsSolver s4;
    IntLinkedList route4;
    CHECK(s4.solve(gb, 0, 2499, route4));
    CHECK(routeIsValid(gb, route4));
    CHECK_EQ((int)s4.steps().size() <= 4 * 2500, true);


    MazeMatrix one;
    one.reset(1, 1);
    MazeGraph o;
    o.buildFromMatrix(one);
    BfsSolver s5;
    IntLinkedList route5;
    CHECK(s5.solve(o, 0, 0, route5));
    CHECK_EQ(route5.size(), 1);
    IntLinkedList::Cursor only = route5.cursor();
    CHECK_EQ(only.next(), 0);



    MazeMatrix corridor;
    corridor.reset(1, 3);
    corridor.removeWall(0, 0, MazeMatrix::WALL_E);
    corridor.removeWall(0, 1, MazeMatrix::WALL_E);
    MazeGraph cg;
    cg.buildFromMatrix(corridor);
    BfsSolver cs;
    IntLinkedList croute;
    CHECK(cs.solve(cg, 0, 2, croute));
    IntLinkedList mined;
    mined.pushBack(1);
    IntLinkedList broute;
    CHECK(!cs.solve(cg, 0, 2, broute, &mined));
    CHECK(broute.isEmpty());
    mined.clear();
    CHECK(cs.solve(cg, 0, 2, broute, &mined));
}
