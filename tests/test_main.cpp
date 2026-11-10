#include "test_util.h"

// Suites are added here, one per roadmap phase.
int main()
{
    printf("%d checks passed, %d failed\n", testPassed(), testFailed());
    return testFailed() > 0 ? 1 : 0;
}
