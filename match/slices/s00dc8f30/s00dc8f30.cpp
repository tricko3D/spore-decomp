// @ 0x00dc8f30
// Parking-spot search for a formation vehicle: fetch the order's destination and the distance band it
// accepts, ask the simulator for everything around the destination, build a packed action circle there and
// walk its slots in distance order.  A slot is taken when it is far enough from the destination, sits on
// drivable terrain and does not overlap the surface projection of any nearby vehicle's destination.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include "types.h"

#define CAT2(a, b) a##b
#define CAT(a, b) CAT2(a, b)
#define PAD1 virtual void CAT(pad, __COUNTER__)();
#define PAD4 PAD1 PAD1 PAD1 PAD1
#define PAD8 PAD4 PAD4
#define PAD16 PAD8 PAD8

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

struct Vector3 {
    float x, y, z;

    Vector3() {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

    float Length() const;
    Vector3 Normalized() const;
};

Vector3 operator-(const Vector3& a, const Vector3& b)
{
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator*(const Vector3& a, float s)
{
    return Vector3(a.x * s, a.y * s, a.z * s);
}

extern float DAT_013ec4b8;                                         // 0x013ec4b8 (normalisation epsilon)

float Vector3::Length() const
{
    return (float)sqrt((double)((x * x + z * z) + y * y));
}

Vector3 Vector3::Normalized() const
{
    double len = sqrt(((double)x * x + (double)z * z) + (double)y * y + DAT_013ec4b8);
    float inv = (float)(1.0 / len);
    return Vector3(x * inv, y * inv, z * inv);
}

// Locomotion interface embedded at +0x34 of a vehicle.
struct cSpatialObject {
    PAD8 PAD1 PAD1 PAD1                                            // slots 0..10
    virtual const Vector3& GetPosition();                          // +0x2c
    PAD8 PAD1 PAD1 PAD1                                            // slots 12..22
    virtual Vector3 GetFacing();                                   // +0x5c
    PAD4                                                           // slots 24..27
    virtual float GetRadiusB();                                    // +0x70
    virtual float GetRadius();                                     // +0x74
};

struct cVehicle;

// Vector of pointers with the retail eastl vector's three pointers and out-of-line erase.
struct FoundVec {
    cVehicle** mpBegin;
    cVehicle** mpEnd;
    cVehicle** mpCapacity;

    FoundVec() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    void Erase(cVehicle** first, cVehicle** last);                 // 0x00b48e40
    ~FoundVec();
};

struct IntVec {
    int* mpBegin;
    int* mpEnd;
    int* mpCapacity;

    IntVec() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    ~IntVec();
};

// Base interface of the objects the spatial query returns; only Cast matters here.
struct cGameData {
    PAD16 PAD16 PAD8 PAD4 PAD1 PAD1                                // slots 0..45
    virtual void* Cast(uint32_t typeID);                           // +0xb8
};

struct cActionCircle {
    struct tSlot {
        char    pad[0x0c];
        Vector3 mPos;                                              // +0x0c
    };
    struct SlotVector {
        tSlot* mpBegin;
        tSlot* mpEnd;
        tSlot* mpCapacity;
        int    mAllocator[2];
    };

    char       pad0[0x50];
    SlotVector mSlots;                                             // +0x50

    void Construct();                                              // 0x00afba10
    void SetAtCenter(const Vector3* center, float radius,
                     const Vector3* facing, float altitude);       // 0x00af9cf0
    void SetSlotRadius(int index, float radius);                   // 0x00afef10
    void BuildIndexListSortedByDistance(const Vector3& pos, IntVec& out); // 0x00afeff0
    void Destroy();                                                // 0x00afbad0
    void NoOp();                                                   // 0x00c2e4e0
};

struct cVehicle {
    PAD16 PAD8 PAD4                                                // slots 0..27
    virtual float GetRadiusB();                                    // +0x70
    char           pad0[0x30];
    cSpatialObject mLoco;                                          // +0x34

    void   GetC3C(Vector3* dest, float* minDist, float* maxDist);  // 0x00c9e8a0
    bool   CanOccupy(const Vector3* pos);                          // 0x00c9e940
    float  GetModeRadius();                                        // 0x00c9eb80
    void   GetDestinationOut(Vector3* out);                        // 0x00c9fdc0
};

// Simulator returned by 0x00b3d240; the radius query lives at vtable +0x48.
struct cSimulator {
    PAD16 PAD1 PAD1                                                // slots 0..17
    virtual void Query(const Vector3& pos, float radius, FoundVec& out,
                       int a, int b, int c);                       // +0x48
};
extern cSimulator* __cdecl FUN_00b3d240();

struct cPlanetModel {
    Vector3* CalcSurface(Vector3* out, const Vector3* in);         // 0x00b81630
};
extern cPlanetModel* __cdecl FUN_00b3d350();

extern uint8_t DAT_0169fa32;                                       // 0x0169fa32

// @ 0x00dc8f30
bool __cdecl FUN_00dc8f30(cVehicle* pVehicle, Vector3* pOut)
{
    float radius = pVehicle->mLoco.GetRadius();

    Vector3 dest;
    float minDist;
    float maxDist;
    pVehicle->GetC3C(&dest, &minDist, &maxDist);

    static FoundVec candidates;
    FUN_00b3d240()->Query(dest, maxDist, candidates, 0, 0, 0);

    cActionCircle circle;
    circle.Construct();

    float dist = (dest - pVehicle->mLoco.GetPosition()).Length();
    Vector3 facing = dist > 0.1f
        ? (dest - pVehicle->mLoco.GetPosition()).Normalized()
        : pVehicle->mLoco.GetFacing();

    circle.SetAtCenter(&dest, maxDist, &facing, pVehicle->GetModeRadius());
    circle.SetSlotRadius(0, radius);

    static IntVec order;
    circle.BuildIndexListSortedByDistance(pVehicle->mLoco.GetPosition(), order);

    if (DAT_0169fa32)
        circle.NoOp();

    int count = (int)(order.mpEnd - order.mpBegin);
    for (int i = 0; i < count; ++i) {
        int index = order.mpBegin[i];
        Vector3 spot = circle.mSlots.mpBegin[index].mPos;

        if ((spot - dest).Length() < minDist)
            continue;
        if (!pVehicle->CanOccupy(&spot))
            continue;

        cVehicle** it = candidates.mpBegin;
        cVehicle** itEnd = candidates.mpEnd;
        while (it != itEnd) {
            cGameData* pData = (cGameData*)*it;
            if (pData) {
                cVehicle* pOther = (cVehicle*)pData->Cast(0x137e8e0);
                if (pOther) {
                    Vector3 otherDest;
                    pOther->GetDestinationOut(&otherDest);
                    float otherRadius = pOther->GetModeRadius();
                    Vector3 offset = otherDest.Normalized() * otherRadius;

                    Vector3 scratch;
                    otherDest = *FUN_00b3d350()->CalcSurface(&scratch, &otherDest) + offset;

                    if ((otherDest - spot).Length() < radius + pOther->GetRadiusB())
                        break;
                }
            }
            ++it;
        }

        if (it != itEnd)
            goto nextSpot;

        *pOut = spot;
        candidates.Erase(candidates.mpBegin, candidates.mpEnd);
        circle.Destroy();
        return true;

    nextSpot:
        ;
    }

    candidates.Erase(candidates.mpBegin, candidates.mpEnd);
    circle.Destroy();
    return false;
}
