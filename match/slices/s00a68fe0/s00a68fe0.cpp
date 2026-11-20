// @ 0x00a68fe0
#include "EA/Random/RandomLinearCongruential.h"
#include <intrin.h>

extern "C" {
extern const float g_RandomScale; // 2^-32
extern const float g_RandomHalf;  // 0.5
}

double EA::Random::RandomLinearCongruential::RandomDoubleUniform()
{
    if (mnSeed == 0)
        mnSeed = (unsigned int)__rdtsc();

    int seed = (int)(mnSeed * 0x278dde6du);
    mnSeed = (unsigned int)seed;
    double value = (double)seed * g_RandomScale + g_RandomHalf;
    if (value >= 1.0)
        value = 0.0;
    return value;
}
