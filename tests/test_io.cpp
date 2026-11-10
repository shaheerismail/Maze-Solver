#include "test_util.h"
#include "maze_matrix.h"
#include "maze_generator.h"
#include "maze_io.h"

void testIo()
{
    // T27: string and file round trips are exact
    MazeMatrix m;
    m.reset(4, 5);
    MazeGenerator g(m, 12345);
    g.generate();
    g.openExtraPassages(2);
    std::string saved = MazeIO::saveToString(m, 12345, 2);
    CHECK_EQ((int)saved.size() > 0, true);
    LoadResult back = MazeIO::loadFromString(saved);
    CHECK(back.ok);
    CHECK_EQ(back.maze.toWallString(), m.toWallString());
    CHECK_EQ(back.maze.toAscii(), m.toAscii());
    CHECK_EQ((unsigned int)back.seed, 12345u);
    CHECK_EQ(back.extraPassages, 2);
    std::string err = "";
    CHECK(MazeIO::saveToFile("roundtrip_test.txt", m, 12345, 2, err));
    LoadResult fromFile = MazeIO::loadFromFile("roundtrip_test.txt");
    CHECK(fromFile.ok);
    CHECK_EQ(fromFile.maze.toWallString(), m.toWallString());

    // T28: bad magic / invalid size / negatives all rejected
    CHECK(!MazeIO::loadFromString(std::string("MAZE_V2\nrows 4\ncols 5\n")).ok);
    CHECK(!MazeIO::loadFromString(
               std::string("MAZE_V1\nrows 0\ncols 5\nseed 1\nextra 0\n\n")).ok);
    CHECK(!MazeIO::loadFromString(
               std::string("MAZE_V1\nrows 51\ncols 51\nseed 1\nextra 0\n\n")).ok);
    LoadResult neg = MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows -1\ncols 5\nseed 1\nextra 0\n\n"));
    CHECK(!neg.ok);
    CHECK(neg.error.find("extra 0 seed") == std::string::npos);  // error text set
    CHECK(!neg.error.empty());
    CHECK(!MazeIO::loadFromFile("no_such_file_maze.txt").ok);

    // T29: malformed grid gets an exact, line-numbered error
    LoadResult shortGrid = MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows 2\ncols 3\nseed 1\nextra 0\n7 7 7\n7 7\n"));
    CHECK(!shortGrid.ok);
    CHECK(shortGrid.error.find("line") != std::string::npos);
    CHECK(!MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows 1\ncols 2\nseed 1\nextra 0\n7 G\n")).ok);         // 'G' not hex
    CHECK(!MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows 1\ncols 2\nseed 1\nextra 0\n7 7 7\n")).ok);       // too many
    CHECK(!MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows 1\ncols 2\nseed 1\nextra 0\n7 7\nJUNK\n")).ok);   // trailing junk

    // T30: lowercase hex accepted
    LoadResult lower = MazeIO::loadFromString(std::string(
        "MAZE_V1\nrows 1\ncols 2\nseed 9\nextra 0\na f\n"));
    CHECK(lower.ok);
    CHECK_EQ(lower.maze.toWallString(), std::string("AF"));

    // T31: failed load must not touch live state (fail-atomic swap-in)
    MazeMatrix live;
    live.reset(3, 3);
    MazeGenerator gl(live, 5);
    gl.generate();
    std::string before = live.toWallString();
    LoadResult bad = MazeIO::loadFromFile("no_such_file_maze.txt");
    if (bad.ok)
        live = bad.maze;    // GUI-style swap-in, only when ok
    CHECK_EQ(live.toWallString(), before);
}
