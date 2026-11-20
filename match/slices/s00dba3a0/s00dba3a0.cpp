// @ 0x00dba3a0
// Behaviour action ("heal other"): pick the nearest suitable neighbour and publish
// it in the caller's result block.
//
// The owner's nearby-list/position holder lives at owner+0xc0; its vtable slot 0x2c
// reports a position (three floats).  The action seeds the search with the object
// returned by FUN_00c22f50(owner), ref-counted into result+0xc, then walks the
// game-data vector of noun tag 0x18c6d19 and keeps the closest candidate that
// FUN_00d00a70 classifies as >= 4.  Returns true.

struct Vec3 {
    float x;
    float y;
    float z;
};

struct PointerList {
    void** mpBegin;     // +0
    void** mpEnd;       // +4
    void** mpCapacity;  // +8
};

// The game-data vector wrapper returned by the noun manager: its own pointer range
// starts at +4 (begin at +4, end at +8).
struct GameObjectRef {
    int         mTag;      // +0
    PointerList mObjects;  // +4
};

// Result block the caller hands in.
struct HealTarget {
    int   mState;    // +0
    bool  mFound;    // +4
    float mDistSq;   // +8
    void* mTarget;   // +0xc, ref-counted
};

typedef void  (__thiscall* FnVoidVoid)(void*);
typedef void* (__thiscall* FnPtrVoid)(void*);
typedef void* (__thiscall* FnPtrObjInt)(void*, void*, int);

// Calling a declared symbol through a __thiscall function-pointer type keeps the
// direct `call rel32` shape while putting the first argument in ecx.
#define TCS(ret, params, name) ((ret(__thiscall*)params) name)

// 0x00c26380 - owner notification with a zero payload.
extern "C" void  FUN_00c26380();
// 0x00c22f50 - the owner's current target object.
extern "C" void* FUN_00c22f50();
// 0x00b3d300 - current game noun manager.
extern "C" void* FUN_00b3d300();
// 0x00b21340 - noun manager game-data vector (thiscall, callee pops five args).
extern "C" void* FUN_00b21340(void*, void*, void*, void*, void*, void*);
// 0x00b3d2c0 - the object behind a noun handle (callee pops its single argument).
extern "C" void* __stdcall FUN_00b3d2c0(void*);
// 0x00d00a70 - relationship/kind code of a game object.
extern "C" int   FUN_00d00a70();

static float DistSq(const Vec3* a, const Vec3* b);

// @ 0x00dba3a0
bool FUN_00dba3a0(void* owner, void* unused2, void* unused3, void* unused4,
                  void* unused5, HealTarget* out)
{
    if (out)
        out->mTarget = 0;
    out->mFound = false;
    out->mDistSq = 0.0f;
    out->mState = 0;

    TCS(void, (void*, int), FUN_00c26380)(owner, 0);

    const Vec3* myPos = (const Vec3*)((FnPtrVoid)((void**)*(void***)((char*)owner + 0xc0))[0x2c / 4])(
        (char*)owner + 0xc0);

    void* best = TCS(void*, (void*), FUN_00c22f50)(owner);
    void* old = out->mTarget;
    if (best != old) {
        if (best)
            ((FnVoidVoid)((void**)*(void***)best)[0])(best);
        out->mTarget = best;
        if (old)
            ((FnVoidVoid)((void**)*(void***)old)[1])(old);
    }

    float bestDist = DistSq(myPos,
        (const Vec3*)((FnPtrVoid)((void**)*(void***)((char*)best + 0x120))[0x2c / 4])(
            (char*)best + 0x120));

    void* info = (void*)((FnPtrVoid)((void**)*(void***)best)[0x4c / 4])(best);

    void* manager = FUN_00b3d300();
    GameObjectRef* data = (GameObjectRef*)TCS(void*, (void*, void*, void*, void*, void*, void*),
                                             FUN_00b21340)(
        manager, (void*)0xcd7d10, (void*)0xd3d420, (void*)0xaccbb0,
        (void*)0xb1e500, (void*)0x18c6d19);

    PointerList* objects = &data->mObjects;
    void** it = objects->mpBegin;
    void** end = objects->mpEnd;
    while (it != end) {
        void* obj = *it;
        if (obj != best) {
            // Fetch the (obj slot 0x4c) call through a named pointer: the original keeps
            // the callee in eax for this call, which only the local spelling produces.
            FnPtrObjInt getOther = (FnPtrObjInt)((void**)*(void***)obj)[0x4c / 4];
            void* other = getOther(obj, info, 1);
            int kind = TCS(int, (void*), FUN_00d00a70)(FUN_00b3d2c0(other));
            if (kind >= 4) {
                float dist = DistSq(myPos,
                    (const Vec3*)((FnPtrVoid)((void**)*(void***)((char*)obj + 0x120))[0x2c / 4])(
                        (char*)obj + 0x120));
                if (bestDist > dist) {
                    bestDist = dist;
                    void* prev = out->mTarget;
                    if (obj != prev) {
                        ((FnVoidVoid)((void**)*(void***)obj)[0])(obj);
                        out->mTarget = obj;
                        if (prev) {
                            ((FnVoidVoid)((void**)*(void***)prev)[1])(prev);
                        }
                    }
                }
            }
        }
        ++it;
    }
    return true;
}

static float DistSq(const Vec3* a, const Vec3* b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dz * dz + dy * dy + dx * dx;
}
