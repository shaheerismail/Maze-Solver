#include "rng.h"

RandomLCG::RandomLCG(unsigned int seed) : state_(1)
{
    reseed(seed);
}

void RandomLCG::reseed(unsigned int seed)
{
    state_ = (seed == 0) ? 1u : seed;
}

unsigned int RandomLCG::next()
{
    // Classic C-style LCG: wraps in unsigned int arithmetic, fully deterministic.
    state_ = state_ * 1103515245u + 12345u;
    return state_ >> 16;   // high 16 bits change fastest
}

int RandomLCG::nextInt(int lo, int hiInclusive)
{
    if (hiInclusive <= lo)
        return lo;
    unsigned int range = (unsigned int)(hiInclusive - lo + 1);
    return lo + (int)(next() % range);
}
