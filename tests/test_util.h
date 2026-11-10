#pragma once
// Hand-rolled test harness: CHECK / CHECK_EQ macros plus global counters.
// No test framework dependency — everything is plain C++.

#include <cstdio>

inline int& testPassed() { static int n = 0; return n; }
inline int& testFailed() { static int n = 0; return n; }

inline void testBegin(const char* suiteName) { printf("[suite] %s\n", suiteName); }

inline int testSummary()
{
    printf("\n%d checks passed, %d failed\n", testPassed(), testFailed());
    return testFailed() > 0 ? 1 : 0;
}

#define CHECK(cond) \
    do { if (cond) { ++testPassed(); } \
         else { ++testFailed(); printf("  FAIL %s:%d  CHECK(%s)\n", __FILE__, __LINE__, #cond); } } while (0)

#define CHECK_EQ(a, b) \
    do { if ((a) == (b)) { ++testPassed(); } \
         else { ++testFailed(); printf("  FAIL %s:%d  CHECK_EQ(%s, %s)\n", __FILE__, __LINE__, #a, #b); } } while (0)
