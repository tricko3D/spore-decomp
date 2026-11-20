// Slice s00e5b0a0 -- SPGonzago cell-game direction/angle interpolation helper.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

extern "C" double __cdecl asin(double);
#pragma intrinsic(asin)

struct cSPVector3 { float x, y, z; };

// ---- globals ---------------------------------------------------------------
extern const float kf14851ec;   // 0x014851ec  1.5258789e-05f
extern const float kf15a7c24;   // 0x015a7c24  pi
extern const float kf15a7c1c;   // 0x015a7c1c  full turn
extern const float kf13ef54c;   // 0x013ef54c  1.1f

// ---- helpers ---------------------------------------------------------------
float FUN_00e50c50(float angle);                        // 0x00e50c50  wrap into [-pi, pi]
float FUN_00e50cb0(float a, float b, float* out);       // 0x00e50cb0  wrapped difference
void  FUN_00e5aff0(cSPVector3* out, float angle);       // 0x00e5aff0  direction from angle

// @ 0x00e5b0a0
cSPVector3* FUN_00e5b0a0(cSPVector3* out, const cSPVector3* a, const cSPVector3* b, float t)
{
    if (a->y * a->y + a->x * a->x + a->z * a->z < kf14851ec) {
        out->x = b->x;
        out->y = b->y;
        out->z = b->z;
        return out;
    }

    float dx = a->x - b->x;
    float ay = a->y;
    float by = b->y;
    float dy = ay - by;
    float dz = a->z - b->z;
    if ((dy * dy + dx * dx) + dz * dz < kf14851ec) {
        out->x = b->x;
        out->y = b->y;
        out->z = b->z;
        return out;
    }

    float tA = (float)asin((double)a->x);
    float angA = (ay > 0.0f) ? tA : (tA < 0.0f ? -kf15a7c24 : kf15a7c24) - tA;

    float tB = (float)asin((double)b->x);
    float angB = (by > 0.0f) ? tB : (tB < 0.0f ? -kf15a7c24 : kf15a7c24) - tB;

    float diff;
    float sign = FUN_00e50cb0(angA, angB, &diff);
    if (diff >= kf15a7c1c * kf13ef54c) {
        out->x = b->x;
        out->y = b->y;
        out->z = b->z;
        return out;
    }

    float ang = FUN_00e50c50(angA + ay * t);

    float sign2 = FUN_00e50cb0(ang, angB, &diff);
    if (sign2 != sign) ang = angB;

    FUN_00e5aff0(out, ang);
    return out;
}
