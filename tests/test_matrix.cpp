#include "test_util.h"
#include "maze_matrix.h"

using Maze = MazeMatrix;

void testMatrix()
{
    // T01: 1x1 maze — smallest valid size, every wall standing
    Maze m;
    CHECK(m.reset(1, 1));
    CHECK_EQ(m.rows(), 1);
    CHECK_EQ(m.cols(), 1);
    CHECK(m.hasWall(0, 0, Maze::WALL_N));
    CHECK(m.hasWall(0, 0, Maze::WALL_E));
    CHECK(m.hasWall(0, 0, Maze::WALL_S));
    CHECK(m.hasWall(0, 0, Maze::WALL_W));
    // golden ASCII art for a fully closed 1x1 maze
    const char* golden1x1 = "+---+\n|   |\n+---+\n";
    CHECK_EQ(m.toAscii(), std::string(golden1x1));

    // T02: invalid sizes rejected; live state untouched
    CHECK(!Maze::isValidSize(0, 5));
    CHECK(!Maze::isValidSize(51, 51));
    CHECK(!Maze::isValidSize(-3, 4));
    CHECK(Maze::isValidSize(1, 1));
    CHECK(Maze::isValidSize(50, 50));
    Maze five;
    CHECK(five.reset(5, 5));
    CHECK(!five.reset(0, 5));
    CHECK(!five.reset(51, 51));
    CHECK(!five.reset(-3, 4));
    CHECK_EQ(five.rows(), 5);
    CHECK_EQ(five.cols(), 5);
    CHECK(five.hasWall(0, 0, Maze::WALL_N));  // still the all-wall grid

    // T03: removeWall clears the bit on BOTH sides of the shared passage
    Maze q;
    q.reset(2, 2);
    q.removeWall(0, 0, Maze::WALL_E);
    CHECK(!q.hasWall(0, 0, Maze::WALL_E));
    CHECK(!q.hasWall(0, 1, Maze::WALL_W));
    CHECK(q.hasWall(0, 0, Maze::WALL_S));  // other walls untouched
    CHECK(q.hasWall(0, 1, Maze::WALL_N));

    // T04: deep copy independence (rule of three)
    Maze copyOf(q);
    copyOf.removeWall(1, 1, Maze::WALL_W);
    CHECK(q.hasWall(1, 1, Maze::WALL_W));        // original unaffected
    CHECK(!copyOf.hasWall(1, 1, Maze::WALL_W));
    Maze assigned;
    assigned = copyOf;
    assigned.reset(3, 3);
    CHECK_EQ(copyOf.rows(), 2);                  // assignment never shares storage
    CHECK_EQ(assigned.rows(), 3);

    // T05: wall-string round trip
    Maze a;
    a.reset(3, 3);
    a.removeWall(0, 0, Maze::WALL_N);   // pretend entrance
    a.removeWall(2, 2, Maze::WALL_S);   // pretend exit
    a.removeWall(1, 1, Maze::WALL_E);
    std::string ws = a.toWallString();
    CHECK_EQ((int)ws.size(), 9);
    Maze b = Maze::fromWallString(3, 3, ws);
    CHECK_EQ(b.toWallString(), ws);
    CHECK_EQ(b.toAscii(), a.toAscii());
}
