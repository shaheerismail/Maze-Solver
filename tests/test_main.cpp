#include "test_util.h"

// One function per suite; each is added in its roadmap phase.
void testMatrix();
void testLists();
void testGenerator();
void testGraph();
void testBfs();
void testExtra();

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
    return testSummary();
}
