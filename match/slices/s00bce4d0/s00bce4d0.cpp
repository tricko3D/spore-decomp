// @ 0x00bce4d0  cSubObject::FacesTarget (this is 0x554 bytes into its owner object, so the owner is
// reached as (char*)this - 0x554 and its vtable is read there).
//
// The owner is asked for a forward vector (vtable slot 0x5c, out-parameter), the target argument is
// resolved to a spatial object (target vtable slot 0x08) whose position (slot 0x2c) is compared with
// the owner's own position (slot 0x2c). The unit vector from the owner to the target is dotted with
// the forward vector and compared against 0.9.
//
// The real class hierarchy behind the +0x554 sub-object is not identified in this slice; the two
// interfaces below only fix the vtable slots the function uses.
//
// Flags: /O2 /arch:SSE /fp:fast (the SSE movss/subss arithmetic plus the x87 fsqrt/1-over-root
// block only appears with /fp:fast; /arch:SSE alone compiles the whole body to x87)

#include "types.h"
#include <math.h>

struct Vector3
{
    float x, y, z;
};

// Interface shared by the owner object (at this-0x554) and the target's spatial object.
class cSpatialThing
{
public:
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28();
    virtual Vector3* GetPosition();            // +0x2c
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58();
    virtual void GetForward(Vector3* out);     // +0x5c
};

class cTarget
{
public:
    virtual void t00(); virtual void t04();
    virtual cSpatialThing* GetSpatial();       // +0x08
};

// The sub-object whose method this is: it sits at +0x554 inside its owner.
class cSubObject
{
public:
    bool FacesTarget(cTarget* target);
};

bool cSubObject::FacesTarget(cTarget* target)
{
    cSpatialThing* self = (cSpatialThing*)((char*)this - 0x554);

    Vector3 forward;
    self->GetForward(&forward);

    cSpatialThing* other = target->GetSpatial();
    Vector3* mine = self->GetPosition();
    Vector3* theirs = other->GetPosition();

    Vector3 d;
    d.x = theirs->x - mine->x;
    d.y = theirs->y - mine->y;
    d.z = theirs->z - mine->z;

    float inv = 1.0f / sqrtf(d.x * d.x + d.y * d.y + d.z * d.z + 1e-8f);

    if (inv * d.x * forward.x + inv * d.y * forward.y + inv * d.z * forward.z > 0.9f)
        return true;
    return false;
}
