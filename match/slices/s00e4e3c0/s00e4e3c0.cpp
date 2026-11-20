// Slice s00e4e3c0 -- SP::FluidParticlesSampleVelocity.
// Module flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast (SPCellFluid.obj)
//
// The object was built /GL and linked with /LTCG, so the two helpers below are internal
// functions with register arguments: 0x00e4dd20 takes the output buffer first (retail passes it
// in eax) and the input position second (ecx); 0x00e4e170 keeps its seven arguments on the
// stack.  The stubs that give the link those facts are in ltcg/00e4e3c0.cpp.
#include "game/types.h"

void FUN_00e4dd20(float* out, const float* in);   // 0x00e4dd20
void FUN_00e4e170(float v, int lo, int hi, int* idx, float* w,
                  float* iw, int* dir);           // 0x00e4e170

struct CellVelocity {
    float x, y, z;
};

// @ 0x00e4e3c0
// Bilinear interpolation of the 20 x 15 per-cell velocity grid (at +0x1c210 of
// the fluid object) at the given world position.
void* __cdecl SP_FluidParticlesSampleVelocity(float* out, char* fluid,
                                              const float* in)
{
    float c[2];
    FUN_00e4dd20(c, in);

    int x, y;
    float wx = 0.0f, iwx = 0.0f;
    int dx = 0;
    float wy = 0.0f, iwy = 0.0f;
    int dy = 0;

    FUN_00e4e170(c[0], 0, 20, &x, &wx, &iwx, &dx);
    FUN_00e4e170(c[1], 0, 15, &y, &wy, &iwy, &dy);

    const CellVelocity* v = (const CellVelocity*)(fluid + 0x1c210);

    out[0] = v[y * 20 + x].x * (wy * wx)
           + v[y * 20 + x + dx].x * (wy * iwx)
           + v[(y + dy) * 20 + x].x * (iwy * wx)
           + v[(y + dy) * 20 + x + dx].x * (iwy * iwx);
    out[1] = v[y * 20 + x].y * (wy * wx)
           + v[y * 20 + x + dx].y * (wy * iwx)
           + v[(y + dy) * 20 + x].y * (iwy * wx)
           + v[(y + dy) * 20 + x + dx].y * (iwy * iwx);
    out[2] = v[y * 20 + x].z * (wy * wx)
           + v[y * 20 + x + dx].z * (wy * iwx)
           + v[(y + dy) * 20 + x].z * (iwy * wx)
           + v[(y + dy) * 20 + x + dx].z * (iwy * iwx);
    return out;
}
