// @ 0x00c6a100
#include "game/types.h"

// Retail-only views: the generated spatial header has different slots and offsets.
struct SurfacePosition_c6a100 { float x, y, z; };
struct SurfaceOrientation_c6a100 { float x, y, z, w; };
namespace SP {
class cPlanetModel {
public:
    SurfacePosition_c6a100* SnapB(SurfacePosition_c6a100* out,
                                 const SurfacePosition_c6a100* position, int flag);
    SurfaceOrientation_c6a100* BuildSurfaceOrientation(
        SurfaceOrientation_c6a100* out, const SurfacePosition_c6a100* position) const;
};
cPlanetModel* PlanetModel();
}
struct SpatialObject_c6a100 {
    void** vtable;
    unsigned char beforeFlags[0x4c];
    unsigned int mFlags;
    unsigned char beforeSupported[0x1d];
    bool mbSupported;
};
struct Egg_c6a100 {
    unsigned char beforeSpatial[0x34];
    SpatialObject_c6a100 spatial;
};

extern "C" void __cdecl FUN_00c6a100(Egg_c6a100* egg)
{
    egg->spatial.mFlags |= 0x4000;
    SpatialObject_c6a100* spatial = &egg->spatial;
    egg->spatial.mbSupported = true;
    SP::cPlanetModel* planet = SP::PlanetModel();
    const SurfacePosition_c6a100* position =
        ((const SurfacePosition_c6a100* (__thiscall*)(SpatialObject_c6a100*))
            spatial->vtable[11])(spatial);
    SurfacePosition_c6a100 snapped;
    SP::PlanetModel()->SnapB(&snapped, position, 0);
    ((void (__thiscall*)(SpatialObject_c6a100*, const SurfacePosition_c6a100*))
        spatial->vtable[14])(spatial, &snapped);
    SurfaceOrientation_c6a100 orientation;
    ((void (__thiscall*)(SpatialObject_c6a100*, const SurfaceOrientation_c6a100*))
        spatial->vtable[15])(spatial,
            planet->BuildSurfaceOrientation(&orientation, &snapped));
}
