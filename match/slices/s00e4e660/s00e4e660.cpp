// @ 0x00e4e660
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
// SP::sUpdateParticle -- one step of the SPH force accumulation for particle
// `i`: gather the particles of the neighbouring cells, add their pressure and
// velocity forces into acceleration[i], then scale the result by the particle's
// density.
#include "game/types.h"
#include "SP/cSPFluidParticles.h"
#include <math.h>

// Fluid tuning globals (unnamed in the PDB): the squared-distance window and the
// force scales.  They are globals, not literals: retail loads and multiplies
// them at run time.
extern float g_15a7b8c;   // 0x015a7b8c  smallest squared distance
extern float g_15a7b94;   // 0x015a7b94  largest squared distance
extern float g_16b39d4;   // 0x016b39d4  pressure force scale
extern float g_16b39d8;   // 0x016b39d8  velocity force scale
extern float g_1483ba4;   // 0x01483ba4  velocity force scale
extern float g_1471064;   // 0x01471064  0.5

int SP_sPartitionGetNeighbors(SP::cFluidPartition* part, float* pos,
                              int* out, int* counts);   // 0x00e4e340

namespace SP {

void sUpdateParticle(cSPFluidParticles* p, int i)
{
    float* pos = (float*)&p->position[i];
    float radius = p->pressure[i];

    int nb[9];
    int counts[9];
    int n = SP_sPartitionGetNeighbors(&p->partition, pos, nb, counts);

    float* acc = (float*)&p->acceleration[i];
    float* vel = (float*)&p->velocity[i];

    for (int j = 0; j < n; ++j) {
        int k = counts[j];
        if (k <= 0)
            continue;
        // Retail PosVel is 0x20 bytes: pos at 0x00, vel at 0x0c, index at 0x1c.
        float* e = (float*)nb[j];
        do {
            float dy = pos[1] - e[1];
            float dz = pos[2] - e[2];
            float dx = pos[0] - e[0];
            float d2 = (dz * dz + dy * dy) + dx * dx;
            if (d2 <= g_15a7b94 && d2 >= g_15a7b8c) {
                float dist = sqrtf(d2);
                int idx = ((int*)e)[7];
                float f = (p->pressure[idx] + radius) * (p->density[idx] / dist);
                f *= g_16b39d4;
                float w = 1.0f - dist;
                f *= w;
                f *= w;
                f *= g_1471064;
                acc[0] += (e[0] - pos[0]) * f;
                acc[1] += (e[1] - pos[1]) * f;
                acc[2] += (e[2] - pos[2]) * f;
                float g = g_16b39d8 * g_1483ba4;
                acc[0] += (e[3] - vel[0]) * g;
                acc[1] += (e[4] - vel[1]) * g;
                acc[2] += (e[5] - vel[2]) * g;
            }
            e += 8;   // sizeof(PosVel) in retail
        } while (--k);
    }

    // Retail reloads p->density[i] before each store: the three multiplications are
    // written through the particle, not through a cached density local.
    acc[0] *= p->density[i];
    acc[1] *= p->density[i];
    acc[2] *= p->density[i];
}

}  // namespace SP
