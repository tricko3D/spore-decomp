// @ 0x00e4def0
// Averaging pass over a per-slot influence table: for each of 300 slots it sums the
// (x,y,z) parts of the slot's first `count` 32-byte influences, sums 1/weight, then
// scales both by 1/count, stores the length of the resulting vector and rescales the
// weight sum.
//
// Retail convention (whole-program /GL image, custom register arguments): the callee's
// first parameter arrives in edx, the second in ecx, the third in eax and the set on the
// stack, so retail's entry moves (ebx = ecx sums, esi = eax lengths, edi = edx weight
// sums) mean the source lists the weight sums first, the sums second and the lengths
// third; see s00e4def0.h. The weight rescale sits before the length because that is the
// order that gives retail's tail shape and its `lea eax, [ebx + 4]` sum base.
// flags /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
#include <string.h>
#include <math.h>
#include "s00e4def0.h"

__declspec(noinline)
void SmoothSlotAverages(float* pWeight, SmoothVec* pSum, float* pLen, const SmoothSet* set)
{
    memset(pSum, 0, 300 * sizeof(SmoothVec));
    memset(pWeight, 0, 300 * sizeof(float));
    memset(pLen, 0, 300 * sizeof(float));

    for (int i = 0; i < 300; ++i) {
        int n = set->count;
        if (n > 0) {
            for (int j = 0; j < n; ++j) {
                const SmoothInfluence& v = set->rows[i].influence[j];
                float x = pSum[i].x + v.x;
                float y = pSum[i].y + v.y;
                float z = pSum[i].z + v.z;
                pSum[i].x = x;
                pSum[i].y = y;
                pSum[i].z = z;
                pWeight[i] += 1.0f / v.weight;
            }
            float inv = 1.0f / (float)n;
            pSum[i].x *= inv;
            pSum[i].y *= inv;
            pSum[i].z *= inv;
            pWeight[i] *= inv;
            pLen[i] = sqrtf(pSum[i].x * pSum[i].x +
                            pSum[i].y * pSum[i].y +
                            pSum[i].z * pSum[i].z);
        }
    }
}
