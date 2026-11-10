#include "test_util.h"

// One function per suite; each is added in its roadmap phase.
void testMatrix();
void testLists();

int main()
{
    testBegin("matrix");
    testMatrix();
    testBegin("lists");
    testLists();
    return testSummary();
}
