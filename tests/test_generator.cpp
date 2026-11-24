#include "test_util.h"
#include "maze_matrix.h"
#include "maze_generator.h"

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

void testGenerator()
{

    MazeMatrix a;
    a.reset(15, 15);
    MazeMatrix b;
    b.reset(15, 15);
    MazeGenerator ga(a, 42);
    ga.generate();
    MazeGenerator gb(b, 42);
    gb.generate();
    CHECK_EQ(a.toWallString(), b.toWallString());
    CHECK_EQ(a.toAscii(), b.toAscii());


    MazeMatrix c;
    c.reset(15, 15);
    MazeGenerator gc(c, 43);
    gc.generate();
    CHECK(a.toWallString() != c.toWallString());


    MazeMatrix d;
    d.reset(20, 25);
    MazeGenerator gd(d, 7);
    gd.generate();
    bool allVisited = true;
    for (int r = 0; r < 20; ++r)
        for (int col = 0; col < 25; ++col)
            if (!d.isVisited(r, col))
                allVisited = false;
    CHECK(allVisited);
    CHECK_EQ(openInteriorPassages(d), 20 * 25 - 1);
    CHECK(!d.hasWall(0, 0, MazeMatrix::WALL_N));
    CHECK(!d.hasWall(19, 24, MazeMatrix::WALL_S));
    CHECK_EQ(openInteriorPassages(d), (int)gd.steps().size());


    MazeMatrix one;
    one.reset(1, 1);
    MazeGenerator g1(one, 1);
    g1.generate();
    CHECK(!one.hasWall(0, 0, MazeMatrix::WALL_N));
    CHECK(!one.hasWall(0, 0, MazeMatrix::WALL_S));
    CHECK(one.hasWall(0, 0, MazeMatrix::WALL_E));
    CHECK_EQ((int)g1.steps().size(), 0);


    MazeMatrix row;
    row.reset(1, 7);
    MazeGenerator grow(row, 5);
    grow.generate();
    CHECK_EQ(openInteriorPassages(row), 6);
    MazeMatrix colm;
    colm.reset(7, 1);
    MazeGenerator gcol(colm, 5);
    gcol.generate();
    CHECK_EQ(openInteriorPassages(colm), 6);


    MazeMatrix big;
    big.reset(50, 50);
    MazeGenerator gbig(big, 9);
    gbig.generate();
    CHECK_EQ((int)gbig.steps().size(), 50 * 50 - 1);
    CHECK_EQ(openInteriorPassages(big), 50 * 50 - 1);
}
