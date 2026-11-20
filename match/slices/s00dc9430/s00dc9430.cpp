// 0x00dc9430 -- anonymous-namespace AttemptRepark, part of the SP::cMovingFormation vehicle
// handling block (SPGonzago): repark a formation vehicle.  While the repark cooldown timer is
// still running nothing happens; otherwise a new parking spot is asked for (0x00dc8f30) and the
// vehicle heads for it, or, when the vehicle is flagged as repark-pending, the vehicles crowded
// around its destination are pushed out of the way with a horn.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include "types.h"

#define CAT2(a, b) a##b
#define CAT(a, b) CAT2(a, b)
#define PAD1 virtual void CAT(pad, __COUNTER__)();
#define PAD4 PAD1 PAD1 PAD1 PAD1
#define PAD8 PAD4 PAD4
#define PAD16 PAD8 PAD8

// A user copy ctor/operator= is what makes cl 15 at /arch:SSE /fp:fast copy the position with
// scalar movss loads instead of x87 fld/fstp, which in turn is what makes the three
// pos.component - destination.component subtractions come out as movss/subss pairs.
struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(const Vector3& o) : x(o.x), y(o.y), z(o.z) {}
    Vector3& operator=(const Vector3& o) { x = o.x; y = o.y; z = o.z; return *this; }
};

// The original builds the delta vector from z down to x: the three subss/movss pairs appear in
// that order and the three results land in consecutive slots at +0x00 (x), +0x04 (y), +0x08 (z).
inline Vector3 Sub(const Vector3& a, const Vector3& b)
{
    Vector3 r;
    r.z = a.z - b.z;
    r.y = a.y - b.y;
    r.x = a.x - b.x;
    return r;
}

// cSPTimer, embedded in the vehicle at +0xba0 (see the getter at 0x00c9e9d0).
struct cSPTimer {
    char IsRunning();                                       // 0x00feba90
    unsigned __int64 Elapsed();                             // 0x00bc3190
    void Stop();                                            // 0x00bc3110
    void Restart();                                         // 0x00bc3130
};

struct cLocoGoal {
    char  pad0[0x20];
    float mSpeed;                                           // +0x20
};

struct NearVehicleList;

// Locomotion (spatial) interface embedded at +0x34 of vehicles.
struct cSpatialObject {
    PAD8 PAD1 PAD1 PAD1                                     // slots 0..10
    virtual const Vector3& GetPosition();                   // +0x2c
    PAD16 PAD1                                              // slots 12..28
    virtual float GetRadius();                              // +0x74
    PAD16 PAD8 PAD1 PAD1 PAD1                               // slots 30..56
    virtual void MoveTo(const Vector3& dst, float speed);   // +0xe4

    cLocoGoal* GetGoal();                                   // 0x00c41ec0
    NearVehicleList* GetNearbyVehicles();                   // 0x00c420d0
};

// Ref-counted handle stored in the nearby-vehicle list; the object pointer is first.
struct AutoRefCount {
    void* mpObject;
};

struct NearVehicleList {
    AutoRefCount* mpBegin;
    AutoRefCount* mpEnd;
};

// Base interface of everything found in the nearby list; only Cast matters here.
struct cGameData {
    PAD16 PAD16 PAD8 PAD4 PAD1 PAD1                          // slots 0..45
    virtual void* Cast(uint32_t typeID);                    // +0xb8
};

struct cVehicle {
    char           pad0[0x34];
    cSpatialObject mLoco;                                   // +0x034
    char           pad38[0x2a4 - 0x38];
    uint32_t       mFlags2a4;                               // +0x2a4
    char           pad2a8[0xba0 - 0x2a8];

    cSPTimer* GetTimer();                                   // 0x00c9e9d0
    float* GetDestinationOut(Vector3& out);                 // 0x00c9fdc0
    void PlaySomething();                                   // 0x00c9eea0
};

extern bool __cdecl FUN_00dc8f30(cVehicle* pVehicle, Vector3* pOut);   // 0x00dc8f30

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

// 0x00dc9430 still differs by one frame dword: the original's frame is 0x34 with an unused slot
// at +0x08 (dist at +0x04, itEnd at +0x0c, delta at +0x10..+0x1c), this source gives 0x30 with
// every slot above +0x08 shifted down by 4.  The instruction stream itself (calls, SSE
// subtraction, x87 square root, MoveTo argument order) is identical to the original.

// @ 0x00dc9430
void AttemptRepark(cVehicle* pVehicle)
{
    cSPTimer* pTimer = pVehicle->GetTimer();

    if (pTimer->IsRunning() && pTimer->Elapsed() <= 250)
        return;

    Vector3 parkingSpot;
    if (FUN_00dc8f30(pVehicle, &parkingSpot)) {
        pTimer->Stop();
        // Reading the goal speed into its own local keeps the vtable load for mLoco.MoveTo after
        // the GetGoal() call, as in the original.
        float speed = pVehicle->mLoco.GetGoal()->mSpeed;
        pVehicle->mLoco.MoveTo(parkingSpot, speed);
        return;
    }

    pTimer->Restart();

    if ((pVehicle->mFlags2a4 & 2) == 0)
        return;

    Vector3 destination;
    pVehicle->GetDestinationOut(destination);

    NearVehicleList* pNearby = pVehicle->mLoco.GetNearbyVehicles();
    AutoRefCount* it = pNearby->mpBegin;
    AutoRefCount* itEnd = pNearby->mpEnd;
    for (; it != itEnd; ++it) {
        cGameData* pData = (cGameData*)it->mpObject;
        if (!pData)
            continue;

        cVehicle* pOther = (cVehicle*)pData->Cast(0x137e8e0);
        if (!pOther)
            continue;

        // Written as a value copy of the position: the copy ctor's movss loads make cl compute
        // the deltas in SSE (movss/subss) and spill them as floats, then feed the *float*
        // distance through x87 (fld/fmul/faddp/fsqrt) before the two GetRadius() calls, exactly
        // as the original does.  A `const Vector3&` makes cl keep the whole distance in x87 and
        // sink it past both calls.
        Vector3 pos = pOther->mLoco.GetPosition();
        Vector3 delta = Sub(pos, destination);
        float dist = (float)sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

        // dist on the left of the comparison makes cl materialise it (fstp [dist]) before the
        // GetRadius() calls instead of deferring the square root to the branch.
        if (dist < pVehicle->mLoco.GetRadius() + pOther->mLoco.GetRadius())
            pOther->PlaySomething();
    }
}
