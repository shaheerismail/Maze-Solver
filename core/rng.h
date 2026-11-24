#pragma once




class RandomLCG {
public:
    explicit RandomLCG(unsigned int seed);
    void reseed(unsigned int seed);
    unsigned int next();
    int nextInt(int lo, int hiInclusive);
private:
    unsigned int state_;
};
