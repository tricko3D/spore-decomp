// Slice s00ca3f20 -- managed-object release (partial).
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

struct cInterface {
    void* mpVtable;                                          // +0x00
};

struct cManaged {
    cInterface* mpInterface;                                 // +0x00
    uint32_t    mFlags04;                                    // +0x04
    uint32_t    mPad08[14];                                  // +0x08 .. +0x3f
    int32_t     mRefCount40;                                 // +0x40
};

struct cOwner {
    uint8_t   mPad00[0xd0];                                  // +0x00 .. +0xcf
    cManaged* mpManaged;                                     // +0xd0

    void Clear();
};

// ===========================================================================
// 0x00ca3f20 -- rebuild the animating creature attached to a game-data object
// (managed-object reset).  Entered with ecx = the owner; it reads the two
// companion pointers at +0xd00 (animating creature) and +0xd04 (game data the
// spawn position and framing radius come from).
// ===========================================================================

struct Vec3_3f20 { float x, y, z; };
struct Vec4_3f20 { float x, y, z, w; };
struct Frame6_3f20 { float v[6]; };

// Constants touched by the original (address bytes are not compared).
extern const float g_1473c70_3f20;      // 0x01473c70
extern const float g_157c720_3f20;      // 0x0157c720
extern const float g_1488874_3f20;      // 0x01488874
extern const float g_13ec488_3f20;      // 0x013ec488
extern const float g_13eb1bc_3f20;      // 0x013eb1bc
extern const float g_157c750_3f20;      // 0x0157c750
extern const float g_157c754_3f20;      // 0x0157c754
extern const float g_157c758_3f20;      // 0x0157c758
extern const float g_157c75c_3f20;      // 0x0157c75c
extern const float g_15d9650_3f20[];    // 0x015d9650
extern char g_169c9c8_3f20;             // 0x0169c9c8

// The animating-creature object; only the fields this function touches.
struct cAnimatingCreature_3f20 {
    void __thiscall AddRef();                   // 0x00a02c30
    void __thiscall Release();                  // 0x00a05270
    void __thiscall SetAttachments(void* owner); // 0x00a04af0
    void __thiscall Set6(int value);            // 0x00a04c80
    void __thiscall SetA(int value);            // 0x00a04ca0
    void __thiscall SetB(int value);            // 0x00a04cc0
    void __thiscall SetC(int value);            // 0x00a04ad0

    char  pad00[0x3c];
    float m3c;                                  // +0x3c
    char  pad40[0x74 - 0x40];
    Vec4_3f20 m74;                              // +0x74
    unsigned char m84;                          // +0x84
    char  pad85[0x17c - 0x85];
    void* m17c;                                 // +0x17c
};

// Object behind the creature's +0x17c handle.
struct EffectTarget_3f20 {
    void __thiscall Apply(float value);         // 0x009b3410
};

struct NounManager_3f20 {
    void* __thiscall FindNoun(unsigned int id);  // 0x00b25f40
};
NounManager_3f20* __cdecl SP_NounManager_3f20(); // 0x00b3d300

// 0x00b33f40: the simulation/creature-manager singleton.
void* __cdecl FUN_00b33f40();

// 0x0169c9c8: a global service object used for the framing calculation.
struct FramingService_3f20 {
    void __thiscall Compute(int index, float x, float y, float z,
                            float* outC, float* outB, float* outA);
};

// Owner methods reached with ecx = the game-data object.
struct cGameData_3f20 {
    void __thiscall FUN_00ca03a0(void* arg);    // 0x00ca03a0
};

// @ 0x00ca3f20
void __fastcall ResetCreature_00ca3f20(char* self)
{
    if (*(char**)(self + 0xd04) == 0)
        return;

    // The spawn position cached by the game data.
    Vec3_3f20 pos = *(Vec3_3f20*)(*(char**)(self + 0xd04) + 0x504);
    Frame6_3f20 frame;

    char* spatial = self + 0x34;
    float* basis = ((float* (__thiscall*)(void*))
                    (*(void***)spatial)[0x30 / 4])(spatial);
    // The original copies the four floats out of the returned basis, then
    // rewrites the block in place with the y/-x/w/-z swizzle (the compiler
    // keeps a temporary for the aggregate assignment).
    Vec4_3f20 local;
    local.x = basis[0];
    local.y = basis[1];
    local.z = basis[2];
    // One six-float scratch buffer: the original builds the y/-x/w/-z swizzle
    // in it, copies four floats into the vector handed to the spawn call, then
    // reuses the same buffer for the bounding frame further down.
    frame.v[0] = local.y;
    frame.v[1] = -local.x;
    frame.v[2] = basis[3];
    frame.v[3] = -local.z;
    local = *(Vec4_3f20*)frame.v;

    if (FUN_00b33f40() != 0) {
        void* spawner = FUN_00b33f40();
        // The spawner's virtual slot is fetched before the spatial call.
        void* spawnSlot = (char*)*(void**)spawner + 0x30;
        void* spawned = ((void* (__thiscall*)(void*, float*, int))
                         (*(void***)spatial)[0x2c / 4])(spatial,
                                                        (float*)&local, 1);
        cAnimatingCreature_3f20* made = (cAnimatingCreature_3f20*)
            ((void* (__thiscall*)(void*, Vec3_3f20*, int, void*))
             (*(void**)spawnSlot))(spawner, &pos, 0, spawned);

        cAnimatingCreature_3f20* old =
            *(cAnimatingCreature_3f20**)(self + 0xd00);
        if (made != old) {
            if (made != 0)
                made->AddRef();
            *(cAnimatingCreature_3f20**)(self + 0xd00) = made;
            if (old != 0)
                old->Release();
        }
    }

    if (*(cAnimatingCreature_3f20**)(self + 0xd00) == 0)
        return;

    // Give the spatial object the creature's nominal radius and read the scale
    // the fresh creature is built with.
    ((void (__thiscall*)(void*, float))
     (*(void***)spatial)[0x40 / 4])(spatial, g_1473c70_3f20);

    {
        cAnimatingCreature_3f20* creature =
            *(cAnimatingCreature_3f20**)(self + 0xd00);
        creature->m3c =
            ((float (__thiscall*)(void*))(*(void***)spatial)[0x34 / 4])(
                spatial);
    }

    void* sub = self + 0x508;
    unsigned int id =
        ((unsigned int (__thiscall*)(void*))(*(void***)sub)[0x10 / 4])(sub);
    void* noun = SP_NounManager_3f20()->FindNoun(id);

    float tintX = g_157c750_3f20;
    float tintY = g_157c754_3f20;
    float tintZ = g_157c758_3f20;
    float tintW = g_157c75c_3f20;
    if (noun != 0) {
        // The noun's colour travels through the same scratch buffer.
        Vec3_3f20* color = (Vec3_3f20*)frame.v;
        *color = *(Vec3_3f20*)((char*)noun + 0xc4);
        tintX = color->x;
        tintY = color->y;
        tintZ = color->z;
    }
    {
        Vec4_3f20* tint = (Vec4_3f20*)(*(char**)(self + 0xd00) + 0x74);
        tint->x = tintX;
        tint->y = tintY;
        tint->z = tintZ;
        tint->w = tintW;
    }
    *(unsigned char*)(*(char**)(self + 0xd00) + 0x84) = 1;

    (*(cAnimatingCreature_3f20**)(self + 0xd00))->SetAttachments(self);

    void* holder = *(void**)(*(char**)(self + 0xd00) + 0x17c);
    if (holder != 0) {
        int index = *(int*)(*(char**)(self + 0xd04) + 0x544);
        float value = g_15d9650_3f20[index];
        ((EffectTarget_3f20*)*(void**)holder)->Apply(value);
    }

    (*(cAnimatingCreature_3f20**)(self + 0xd00))->Set6(6);
    (*(cAnimatingCreature_3f20**)(self + 0xd00))->SetA(1);
    (*(cAnimatingCreature_3f20**)(self + 0xd00))->SetB(1);
    (*(cAnimatingCreature_3f20**)(self + 0xd00))->SetC(2);

    ((cGameData_3f20*)self)->FUN_00ca03a0((void*)0x2481db0);

    // Framing: feed the current target point through the global service and
    // keep the three returned components.
    float px = *(float*)(self + 0xc50);
    float py = *(float*)(self + 0xc54);
    float pz = *(float*)(self + 0xc58);
    int index = *(int*)(self + 0xb1c);
    float outC, outB, outA;
    ((FramingService_3f20*)&g_169c9c8_3f20)->Compute(
        index, px, py, pz, &outC, &outB, &outA);
    *(float*)(self + 0xc5c) = outC;
    *(float*)(self + 0x218) = outA;
    *(float*)(self + 0x534) = outB;

    // A zeroed (or unordered) radius is filled in from the spatial sub-object.
    if (!(*(float*)(self + 0x540) != g_13eb1bc_3f20)) {
        *(float*)(self + 0x540) =
            ((float (__thiscall*)(void*))(*(void***)sub)[0x58 / 4])(sub);
    }

    *(float*)(self + 0xd2c) = g_1488874_3f20 * *(float*)(self + 0x218);
    *(float*)(self + 0x21c) = g_157c720_3f20;

    // Rebuild the bounding frame from the game data's frame radius.
    char* frameData = *(char**)(self + 0xd04);
    float* frameRadius = (float*)(frameData + 0x554);
    float halfX = frameRadius[0] * g_13ec488_3f20;
    float halfY = frameRadius[1] * g_13ec488_3f20;
    float depth = frameRadius[2];

    frame.v[0] = halfX;
    frame.v[1] = halfY;
    frame.v[2] = 0.0f;
    frame.v[3] = -halfX;
    frame.v[4] = -halfY;
    frame.v[5] = depth;

    void** spatialVt = *(void***)spatial;
    float scale =
        ((float (__thiscall*)(void*))spatialVt[0x34 / 4])(spatial);
    ((void (__thiscall*)(void*, Frame6_3f20*, float))
     spatialVt[0x64 / 4])(spatial, &frame, scale);
}

// @ 0x00ca42c0
void cOwner::Clear()
{
    if (mpManaged) {
        cInterface* iface = mpManaged->mpInterface;
        ((void (__thiscall*)(cInterface*, cManaged*, int))
            ((void**)iface->mpVtable)[0x16c / 4])(iface, mpManaged, 0);
    }
    if (mpManaged) {
        cManaged* p = mpManaged;
        mpManaged = 0;
        if (p->mRefCount40 > 1) {
            --p->mRefCount40;
        } else {
            cInterface* iface = p->mpInterface;
            unsigned char flag = (unsigned char)((p->mFlags04 >> 31) & 1);
            ((void (__thiscall*)(cInterface*, cManaged*, unsigned char))
                ((void**)iface->mpVtable)[0x170 / 4])(iface, p, flag);
        }
    }
}
