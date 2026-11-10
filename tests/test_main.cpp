#include "test_util.h"

// One function per suite; each is added in its roadmap phase.
void testMatrix();
void testLists();
void testGenerator();
void testGraph();
void testBfs();
void testExtra();
void testIo();

int main()
{
    testBegin("matrix");
    testMatrix();
    testBegin("lists");
    testLists();
    testBegin("generator");
    testGenerator();
    testBegin("graph");
    testGraph();
    testBegin("bfs");
    testBfs();
    testBegin("extra");
    testExtra();
    testBegin("io");
    testIo();
    return testSummary();
}
