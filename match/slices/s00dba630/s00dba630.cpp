// Tribe behaviour action: steer a member towards the point of the tribe region it
// belongs to while it is still outside the region's reach, then reserve a tool slot
// for the region and walk the reserved placement.
//
// The owner's navigation/position holder lives at owner+0xc0; its vtable slot 0x2c
// reports a position (three floats) and slot 0x74 a radius (float).  The owner's
// current target object is FUN_00c22f50(owner) (a cTribe), so the region table and
// the tool reservation use cTribe members.  Only the layout this function touches is
// spelled out; the called helpers are reached through their retail addresses.
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast

struct Vec3 {
    float x;
    float y;
    float z;
};

typedef void* (__thiscall* FnPtrVoid)(void*);
typedef float (__thiscall* FnFloatVoid)(void*);
typedef void (__cdecl* FnNormalize)(Vec3* out, const Vec3* v);

// Calling a declared symbol through a __thiscall function-pointer type keeps the
// direct `call rel32` shape while putting the first argument in ecx.
#define TCS(ret, params, name) ((ret(__thiscall*)params) name)

// 0x00c41ec0 - field of the owner's position holder (a ref-counted object at +0x0).
extern "C" void* FUN_00c41ec0();
// 0x00c22f50 - the owner's current target (the tribe).
extern "C" void* FUN_00c22f50();
// 0x00c97c40 - cTribe::FindRegionIndex(uint32_t key, int).
extern "C" int   FUN_00c97c40();
// 0x00c979e0 - cTribe::RandomRegionPoint(Vec3* out, uint32_t key, int).
extern "C" void* FUN_00c979e0();
// 0x00c97ca0 - cTribe::InsertRegionValue(uint32_t key, void* object).
extern "C" void  FUN_00c97ca0();
// 0x00c97b10 - cTribe::RandomRegionPointAt(Vec3* out, uint32_t key, int).
extern "C" void* FUN_00c97b10();
// 0x00c423c0 - position of a locomotive object.
extern "C" void* FUN_00c423c0();
// 0x00c42e20 - CLocoObject::IsNearGoal().
extern "C" char  FUN_00c42e20();
// 0x00c986d0 - cTribe::ReserveToolSlot(int weight, int* outIndex, gameData*).
extern "C" void* FUN_00c986d0();
// 0x00c97d40 - cTribe::UpdateRegionSlot(uint32_t key, uint32_t* extra).
extern "C" void  FUN_00c97d40();
// 0x00c27d20 - region/selection table lookup, returns the entry record.
extern "C" void* FUN_00c27d20();
// 0x00c1c1d0 - cSPCreatureBase::MoveToPointAtSpeed(target, position, f0, f1).
extern "C" void  FUN_00c1c1d0();
// 0x00c1c5c0 - creature walk/follow command used by the tool placement path.
extern "C" int   FUN_00c1c5c0();
// 0x00449c20 - SP::normalized_safe(Vec3* out, const Vec3* v).
extern "C" Vec3* FUN_00449c20();

static __forceinline float DistSq(const Vec3& a, const Vec3& b)
{
    float dz = a.z - b.z;
    float dy = a.y - b.y;
    float dx = a.x - b.x;
    return dz * dz + dy * dy + dx * dx;
}

static __forceinline const Vec3* PositionOf(void* body)
{
    return (const Vec3*)((FnPtrVoid)((void**)*(void***)body)[0x2c / 4])(body);
}

static __forceinline float RadiusOf(void* body)
{
    return ((FnFloatVoid)((void**)*(void***)body)[0x74 / 4])(body);
}

// @ 0x00dba630
int FUN_00dba630(void* owner, int key)
{
    if (((unsigned char*)owner)[0x330] & 4)
        return -1;

    char* body = (char*)owner + 0xc0;
    int slot = -1;

    int region;
    void* goal = TCS(void*, (void*), FUN_00c41ec0)(body);
    void* tribe = TCS(void*, (void*), FUN_00c22f50)(owner);

    region = TCS(int, (void*, int, void*), FUN_00c97c40)(tribe, key, owner);

    Vec3 target;
    const Vec3* p = PositionOf(body);
    target.x = p->x;
    target.y = p->y;
    target.z = p->z;

    float reach = *(const float*)0x1572070;
    float radius = RadiusOf(body);

    if (region == -1) {
        Vec3 point;
        target = *(const Vec3*)TCS(Vec3*, (void*, Vec3*, int, void*), FUN_00c979e0)(tribe, &point, key, owner);

        float r2 = RadiusOf(body);
        if (DistSq(target, *PositionOf(body)) <
            (r2 * *(const float*)0x13eb95c) * (r2 * *(const float*)0x13eb95c)) {
            // Inside the creature's radius: join the region, take its region point.
            TCS(void, (void*, int, void*), FUN_00c97ca0)(tribe, key, owner);
            target = *(const Vec3*)TCS(Vec3*, (void*, Vec3*, int, void*), FUN_00c97b10)(tribe, &point, key, owner);
        } else {
            reach = r2 * *(const float*)0x13eb95c;
            radius = r2 * *(const float*)0x14853e0;
        }
    } else {
        Vec3 point;
        target = *(const Vec3*)TCS(Vec3*, (void*, Vec3*, int, void*), FUN_00c97b10)(tribe, &point, key, owner);
    }

    if (*(int*)((char*)goal + 0x5c) != 0) {
        if (DistSq(*PositionOf(body), target) > *(const float*)0x1485720)
            TCS(void, (void*, void*, Vec3*, float, float), FUN_00c1c1d0)(
                owner, (void*)2, &target, reach, radius);
    } else {
        if (*(const float*)0x14774d0 > DistSq(*PositionOf(body), target)) {
            const Vec3* goalPos = (const Vec3*)TCS(void*, (void*), FUN_00c423c0)(goal);
            if (DistSq(target, *goalPos) < *(const float*)0x1485720)
                TCS(void, (void*, void*, Vec3*, float, float), FUN_00c1c1d0)(
                    owner, (void*)2, &target, reach, radius);
        }
    }

    if (region != 0)
        return slot;

    if (!TCS(char, (void*), FUN_00c42e20)(body))
        return slot;

    target = *(const Vec3*)TCS(void*, (void*, int, int*, void*), FUN_00c986d0)(tribe, key, &slot, owner);
    if (slot == -1)
        return slot;

    TCS(void, (void*, int, void*), FUN_00c97d40)(tribe, key, owner);

    region = key;
    void* entry = TCS(void*, (void*, int*), FUN_00c27d20)((char*)tribe + 0x394, &region);

    float walk = *(const float*)((char*)entry + 0xc) + RadiusOf(body) + *(const float*)0x1488874;

    Vec3 tmp;
    tmp.x = *(const float*)((char*)entry + 0x14);
    tmp.y = *(const float*)((char*)entry + 0x18);
    tmp.z = *(const float*)((char*)entry + 0x1c);

    float delta[3];
    delta[0] = tmp.x - target.x;
    delta[1] = tmp.y - target.y;
    delta[2] = tmp.z - target.z;

    float normal[3];
    ((FnNormalize)FUN_00449c20)((Vec3*)normal, (const Vec3*)delta);

    target.x = tmp.x - normal[0] * walk;
    target.y = tmp.y - normal[1] * walk;
    target.z = tmp.z - normal[2] * walk;

    TCS(int, (void*, void*, Vec3*, float*, float, float), FUN_00c1c5c0)(
        owner, (void*)2, &target, normal, *(const float*)0x1572070, *(const float*)0x157206c);
    return slot;
}
