#include "test_util.h"

// One function per suite; each is added in its roadmap phase.
void testMatrix();

int main()
{
    testBegin("matrix");
    testMatrix();
    return testSummary();
}
