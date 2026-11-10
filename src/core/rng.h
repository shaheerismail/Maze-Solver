#pragma once
// RandomLCG — hand-rolled linear congruential random number generator.
// Why not <random>? It is outside this project's allowed facilities, and a
// fixed recurrence is easier to explain and reproducible on any machine.

class RandomLCG {
public:
    explicit RandomLCG(unsigned int seed);
    void reseed(unsigned int seed);
    unsigned int next();                     // pseudo-random 0..65535
    int nextInt(int lo, int hiInclusive);    // uniform-ish in [lo, hi]
private:
    unsigned int state_;                     // never zero (0 seeded -> 1)
};
