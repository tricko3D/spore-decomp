// @ 0x00b81630
// Flags: /O2 /MD /Gy /TP /arch:SSE /GS- /fp:fast
// SP::cPlanetModel::FUN_00b81630 -- snap a direction out to the surface radius,
// clamped so it never ends up below the local terrain height.
#include "game/types.h"
#include "cSPVector3.h"

#include <math.h>

namespace SP {

// Terrain map handed out by the terrain sphere at vtable slot +0xc.
struct TerrainMap_s00b8_1630 {
  char pad00[0x34];
  float radius;       // +0x34
  float heightScale;  // +0x38
  float height;       // +0x3c
};

class cPlanetModel {
 public:
  char pad00[0x24];
  void* mpISphere;  // +0x24

  float GetRadiusAt(cSPVector3 const * dir) const;                 // 0x00b7ef70
  cSPVector3 * FUN_00b81630(cSPVector3 * out, cSPVector3 const * dir) const;
};

// The terrain sphere's +0xc entry returns the map (raw vtable read: retail loads
// the slot into edx and calls through it).
typedef TerrainMap_s00b8_1630 *(__thiscall *GetMapFn_s00b8_1630)(void *);

cSPVector3 * cPlanetModel::FUN_00b81630(cSPVector3 * out,
                                        cSPVector3 const * dir) const {
  float r = GetRadiusAt(dir);
  float h;
  if (mpISphere) {
    GetMapFn_s00b8_1630 getMap =
        (GetMapFn_s00b8_1630)((void **)mpISphere)[3];
    TerrainMap_s00b8_1630 * m = getMap(mpISphere);
    h = m->height * m->heightScale + m->radius;
  } else {
    h = 0.0f;
  }
  if (r > h) {
  } else {
    r = h;
  }

  float inv = 1.0f / sqrtf(dir->x * dir->x + dir->y * dir->y +
                            dir->z * dir->z + 1e-8f);
  float xi = dir->x * inv;
  float yi = dir->y * inv;
  float zi = dir->z * inv;
  out->x = xi * r;
  out->y = yi * r;
  out->z = zi * r;
  return out;
}

}  // namespace SP
