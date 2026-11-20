// @ 0x00b82110
// SP::cPlanetModel::FUN_00b82110 -- projected-position pick against the water sphere.
//   Probe the terrain with FUN_00b82060, take the water-sphere radius from the
//   mpISphere map set, gate on the radial guard, project onto the water sphere
//   with IntersectRaySphere and keep whichever of the two positions is nearer.
// Flags: /O2 /MD /Gy /TP /arch:SSE /GS- /fp:fast
#include "game/types.h"

#pragma warning(disable: 4100)

namespace slice_b82110 {

struct Vec3 { float x, y, z; };

struct cTerrainMapSet {
    char  pad0[0x34];
    float mRadius;            // +0x34
    float mMaxHeight;         // +0x38
    float mWaterHeight;       // +0x3c
};

struct cITerrainSphere {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual cTerrainMapSet* GetMapSet();                      // +0xc
};

extern Vec3 kFallback;                                        // 0x016881f0 (fallback/invalid point)

bool __cdecl IntersectRaySphere(const Vec3& origin, const Vec3& direction, const Vec3& center,
                                float radius, float* pDistance);          // 0x005a9c40

struct cPlanetModel {
    char  pad00[0x24];
    cITerrainSphere* mpISphere;                               // +0x24

    Vec3* FUN_00b82060(Vec3* out, const Vec3* from, const Vec3* dir);     // 0x00b82060
    Vec3* FUN_00b82110(Vec3* out, const Vec3* from, const Vec3* dir);     // 0x00b82110
};

Vec3* cPlanetModel::FUN_00b82110(Vec3* out, const Vec3* from, const Vec3* dir)
{
    Vec3 n;
    Vec3 p;
    FUN_00b82060(&p, from, dir);

    float r;
    if (mpISphere != 0) {
        cTerrainMapSet* m = mpISphere->GetMapSet();
        r = m->mWaterHeight * m->mMaxHeight + m->mRadius;
    } else {
        r = 0.0f;
    }

    float fx = from->x;
    if (fx * fx + from->y * from->y + from->z * from->z >= r * r &&
        IntersectRaySphere(*from, *dir, kFallback, r, &r)) {
        n.x = fx + dir->x * r;
        n.y = from->y + dir->y * r;
        n.z = from->z + dir->z * r;
        if ((p.x == kFallback.x && p.y == kFallback.y && p.z == kFallback.z) ||
            ((n.z - from->z) * (n.z - from->z) + (n.y - from->y) * (n.y - from->y)) +
                    (n.x - fx) * (n.x - fx) <
                ((p.z - from->z) * (p.z - from->z) + (p.y - from->y) * (p.y - from->y)) +
                    (p.x - fx) * (p.x - fx)) {
            out->x = n.x;
            out->y = n.y;
            out->z = n.z;
            return out;
        }
    }

    out->x = p.x;
    out->y = p.y;
    out->z = p.z;
    return out;
}

}  // namespace slice_b82110
