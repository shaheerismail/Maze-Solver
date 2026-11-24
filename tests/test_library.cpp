#include "test_util.h"
#include "maze_library.h"

static MazeRecord makeRecord(unsigned int seed, const char* walls)
{
    MazeRecord r;
    r.seed = seed;
    r.rows = 2;
    r.cols = 2;
    r.extraPassages = 0;
    r.walls = walls;
    return r;
}

void testLibrary()
{

    MazeLibrary lib;
    lib.insert(makeRecord(50, "AAAA"));
    lib.insert(makeRecord(20, "BBBB"));
    lib.insert(makeRecord(90, "CCCC"));
    CHECK_EQ(lib.size(), 3);
    MazeRecord out;
    CHECK(lib.findBySeed(20, out));
    CHECK_EQ(out.walls, std::string("BBBB"));
    CHECK(lib.findBySeed(90, out));
    CHECK_EQ(out.walls, std::string("CCCC"));
    CHECK(!lib.findBySeed(1, out));


    lib.insert(makeRecord(20, "WWWW"));
    CHECK_EQ(lib.size(), 3);
    CHECK(lib.findBySeed(20, out));
    CHECK_EQ(out.walls, std::string("WWWW"));



    lib.insert(makeRecord(70, "DDDD"));
    CHECK_EQ(lib.size(), 4);
    CHECK(lib.removeBySeed(90));
    CHECK_EQ(lib.size(), 3);
    CHECK(!lib.findBySeed(90, out));
    CHECK(lib.findBySeed(70, out));
    CHECK(lib.removeBySeed(20));
    CHECK_EQ(lib.size(), 2);
    CHECK(!lib.findBySeed(20, out));
    CHECK(lib.findBySeed(50, out));
    CHECK_EQ(lib.size(), 2);


    MazeLibrary lib2;
    lib2.insert(makeRecord(50, "A"));
    lib2.insert(makeRecord(30, "B"));
    lib2.insert(makeRecord(70, "C"));
    lib2.insert(makeRecord(40, "D"));
    lib2.insert(makeRecord(90, "E"));
    CHECK(lib2.removeBySeed(50));
    CHECK_EQ(lib2.size(), 4);
    CHECK(!lib2.findBySeed(50, out));
    unsigned int leftmostSeed = 0;
    unsigned int rightmostSeed = 0;
    MazeRecordList sorted;
    lib2.inOrder(sorted);
    MazeRecordList::Cursor it = sorted.cursor();
    CHECK_EQ(sorted.size(), 4);
    CHECK_EQ(it.next().seed, 30u);
    CHECK_EQ(it.next().seed, 40u);
    leftmostSeed = 40;
    CHECK_EQ(it.next().seed, 70u);
    rightmostSeed = it.next().seed;
    CHECK_EQ(rightmostSeed, 90u);
    (void)leftmostSeed;


    MazeLibrary lib3;
    unsigned int seeds[6] = { 900, 100, 400, 500, 550, 50 };
    for (int i = 0; i < 6; ++i)
        lib3.insert(makeRecord(seeds[i], "ZZ"));
    MazeRecordList ordered;
    lib3.inOrder(ordered);
    CHECK_EQ(ordered.size(), 6);
    MazeRecordList::Cursor walk = ordered.cursor();
    unsigned int last = 0;
    while (walk.hasNext()) {
        MazeRecord rec = walk.next();
        CHECK((unsigned int)last < rec.seed);
        last = rec.seed;
    }
    CHECK_EQ(last, 900u);


    MazeLibrary empty;
    CHECK_EQ(empty.size(), 0);
    MazeRecord nothing;
    CHECK(!empty.findBySeed(7, nothing));
    CHECK(!empty.removeBySeed(7));
    CHECK_EQ(empty.size(), 0);
}
