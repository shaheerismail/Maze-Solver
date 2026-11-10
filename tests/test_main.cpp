#include "test_util.h"

// One function per suite; each is added in its roadmap phase.
void testMatrix();
void testLists();
void testGenerator();

int main()
{
    testBegin("matrix");
    testMatrix();
    testBegin("lists");
    testLists();
    testBegin("generator");
    testGenerator();
    return testSummary();
}
