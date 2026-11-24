#include "test_util.h"


void testMatrix();
void testLists();
void testGenerator();
void testGraph();
void testBfs();
void testExtra();
void testIo();
void testLibrary();

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
    testBegin("library");
    testLibrary();
    return testSummary();
}
