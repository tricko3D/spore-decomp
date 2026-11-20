// @ 0x00a69020
#include "game/types.h"
#include <intrin.h>

namespace EA {
namespace Random {
// Local declaration includes the float overload missing from the generated header.
class RandomLinearCongruential {
public:
    float RandomFloatUniform();
    unsigned int mnSeed;
};
}
}

extern "C" {
extern const float g_RandomScale; // 2^-32
extern const float g_RandomHalf;  // 0.5
extern const float g_RandomOne;   // 1.0
}

float EA::Random::RandomLinearCongruential::RandomFloatUniform()
{
    if (mnSeed == 0)
        mnSeed = (unsigned int)__rdtsc();

    int seed = (int)(mnSeed * 0x278dde6du);
    mnSeed = (unsigned int)seed;
    float value = (float)seed * g_RandomScale + g_RandomHalf;
    if (value >= g_RandomOne)
        value = 0.0f;
    return value;
}
