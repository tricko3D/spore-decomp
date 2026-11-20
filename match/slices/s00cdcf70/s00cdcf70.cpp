// @ 0x00cdcf70
// SP::cTribeModeStrategy::StartCvgTransitionCinematic (SPGonzago/SPTribeModeStrategy.obj)
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
//
// Reconstruction of the retail instruction stream.  The function:
//   * clears the strategy's transition effect and sends app-mode message 0x1002,
//   * asks the editor for the tribe position and the app mode for its state,
//   * places a transform message on the transition effect object and starts it,
//   * if the message system resolves 0x427d8c28, posts a second transform to the
//     resolved object,
//   * clears the editor selection/attachment state and the city-hall shadow,
//   * formats and sends the "TRG2CVG_ShowCityHall" messages, choosing the tag from
//     the game timer.
//
// Calling conventions were checked against the game image:
//   0x00b3d300/0x0067cab0/0x00b3d4d0/0x00b3d520/0x00cf74c0/0x0067ddd0 are plain
//   getters (`mov eax,[global]; ret`), so the arguments pushed right before them
//   belong to the member call that follows and are popped by that callee
//   (0x00ae09b0 `ret 0xc`, 0x00ae0930 `ret 0x18`, 0x00adf390 `ret 4`).
//   0x00ad7a30 builds a 0x30-byte message parameter (`ret 4`, returns `this`),
//   0x00ad7ad0 is its destructor (releases the owned pointer at +0x2c).

#include "types.h"
#include <math.h>

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

struct Vec3 { float x, y, z; };
struct Mat3 { float m[9]; };

// Transform message: flags/count, translation, uniform scale, rotation.
struct XformMsg {
    u16  mFlags;      // +0x00
    u16  mCount;      // +0x02
    Vec3 mPos;        // +0x04
    float mScale;     // +0x10
    Mat3 mRot;        // +0x14
};

struct TribeModeState {
    u8    pad000[0x398];
    void* mpEffect;   // +0x398
};

// --- getters (no arguments of their own) -----------------------------------
extern "C" void* __stdcall FUN_00b3d300();
extern "C" void* __stdcall FUN_0067cab0();
extern "C" void* __stdcall FUN_00b3d4d0();
extern "C" void* __stdcall FUN_00b3d520();
extern "C" void* __stdcall FUN_00cf74c0();
extern "C" void* __stdcall FUN_0067ddd0();

// plain functions, caller-cleanup
extern "C" float __cdecl FUN_00cdb8b0();
extern "C" void  __cdecl FUN_0059c190(void* owner, void* out);
extern "C" void* __cdecl FUN_004a9b40(void* owner, void* out);

// --- member functions of the objects named at the call sites ----------------
struct C_BFC5F0 { void* Get(); };                              // 0x00bfc5f0  [ecx+0x70]
struct C_F67D90 { void* Get(); };                              // 0x00f67d90  [ecx+0x74]
struct C_801BB0 { void Method(u32 mode); };                    // 0x00801bb0
struct C_41CB40 { void Set(const float* src); };               // 0x0041cb40
struct C_478DB0 { void Add(void* obj); };                      // 0x00478db0
struct C_537DC0 { void Post(void* msg); };                     // 0x00537dc0
struct C_B225D0 { void Set(void* obj); };                      // 0x00b225d0
struct C_C90470 { void Method(); };                            // 0x00c90470
struct C_C8FED0 { void* Get(); };                              // 0x00c8fed0
struct C_C8FD30 { void* Get(); };                              // 0x00c8fd30
struct PtrVec { u32 mBegin; u32 mEnd; };
struct C_AD79D0 { void Method(const void* a, const void* b); }; // 0x00ad79d0
struct C_MsgParam;
struct C_RadiusParam;
struct C_AE09B0 { void Send(u32 id, const C_MsgParam& param, int flag); };   // 0x00ae09b0
struct C_AE0930 { void Send(u32 id, int a, int b, const char* tag, int c, int d); }; // 0x00ae0930
struct C_ADF390 { void Set(int on); };                         // 0x00adf390

// The 0x30-byte message parameter: built by 0x00ad7a30, released by 0x00ad7ad0
// (the release drops the owned pointer at +0x2c through its vtable +0xc0).
struct C_MsgParam { u32 mData[0x0c]; };
struct C_MsgParamInit { void Init(void* owner); };    // 0x00ad7a30, returns this
struct C_MsgParamRel  { void Release(); };            // 0x00ad7ad0
inline C_MsgParam& InitMsgParam(C_MsgParam& p, void* owner)
{
    ((C_MsgParamInit*)&p)->Init(owner);
    return p;
}
struct C_AE09BR { void Send(u32 id, const C_RadiusParam& param, int flag); }; // 0x00ae09b0

// The transition parameter: 0x30 bytes built by 0x00ad79d0 (two positions), it
// carries the two distances at +0x1c/+0x20 and releases the pointer at +0x2c.
struct C_RadiusParam {
    u32   pad000[7];  // +0x00 .. +0x18
    float mDistA;     // +0x1c
    float mDistB;     // +0x20
    u32   pad024[2];  // +0x24, +0x28
    void* mpOwned;    // +0x2c
};

// static data used by the message code
extern float g_fOne;          // 0x01485720
extern float g_fHalf;         // 0x01471064
extern float g_fMax;          // 0x01477430  (FLT_MAX)
extern float g_fThird;        // 0x013f26dc  (1/3)
extern float g_fMinusThird;   // 0x0140d0cc  (-1/3)
extern float g_vecA[3];       // 0x0169b34c
extern float g_rotA[9];       // 0x0169b328
extern float g_rotB[9];       // 0x0169b378
extern const char g_tagMili[];    // 0x01477808
extern const char g_tagEcon[];    // 0x01477800
extern const char g_tagReli[];    // 0x01477810

struct EffectObj {
    void* mpOwner;    // +0x00
    u32   mFlags4;    // +0x04
};

#define VSLOT(fnptr, obj, off) ((fnptr)(((void***)(obj))[0][(off) / 4]))

// @ 0x00cdcf70
void __fastcall StartCvgTransitionCinematic(TribeModeState* self)
{
    void* tribe = ((C_BFC5F0*)FUN_00b3d300())->Get();
    ((C_801BB0*)FUN_0067cab0())->Method(0x1002);

    // Stop and release the previous transition effect.
    if (self->mpEffect != 0) {
        VSLOT(void (__thiscall*)(void*, int), self->mpEffect, 0xc)(self->mpEffect, 1);
        if (self->mpEffect != 0) {
            void* effect = self->mpEffect;
            self->mpEffect = 0;
            VSLOT(void (__thiscall*)(void*), effect, 4)(effect);
        }
    }

    u8*   tribeSub = (u8*)tribe + 0x120;
    Vec3* pos = (Vec3*)VSLOT(void* (__thiscall*)(void*), tribeSub, 0x2c)(tribeSub);

    void* transitionMgr = FUN_00b3d520();
    void* simulator     = ((C_F67D90*)FUN_00b3d300())->Get();
    Vec3* simPos        = (Vec3*)((u8*)simulator + 0x1150);

    // Create the transition effect between the tribe and the city-view target.
    void* effect = VSLOT(void* (__thiscall*)(void*, u32, u32, int), transitionMgr, 0xc)
                       (transitionMgr, *(u32*)&simPos->x, *(u32*)&simPos->z, 0);
    void* world = FUN_00cf74c0();
    ((C_478DB0*)((u8*)world + 0x104))->Add(effect);

    // First transform message: tribe position plus the editor's facing matrix.
    XformMsg msg;
    Mat3     matrix;

    msg.mFlags = 0;
    msg.mCount = 0;
    msg.mScale = g_fOne;
    ((C_41CB40*)&msg.mRot)->Set(g_rotB);

    Vec3* editPos = (Vec3*)VSLOT(void* (__thiscall*)(void*), tribeSub, 0x30)(tribeSub);

    msg.mFlags |= 4;
    ++msg.mCount;
    msg.mPos = *pos;

    FUN_0059c190(editPos, &matrix);
    msg.mRot = matrix;
    msg.mFlags |= 2;
    ++msg.mCount;

    ((C_537DC0*)((u8*)effect + 8))->Post(&msg);

    EffectObj* eff = (EffectObj*)effect;
    VSLOT(void (__thiscall*)(void*, void*, int), eff->mpOwner, 0x16c)(eff->mpOwner, effect, 1);
    eff->mFlags4 |= 1;

    VSLOT(void (__thiscall*)(void*, void*, int), transitionMgr, 0xb4)(transitionMgr, effect, 0);
    VSLOT(void (__thiscall*)(void*, void*, float), transitionMgr, 0xb8)(transitionMgr, effect, g_fMax);

    // Second transform, posted to the resolved city-hall object only.
    void* ui = FUN_0067ddd0();
    void* resolved = 0;
    if (VSLOT(unsigned char (__thiscall*)(void*, u32, int, void**), ui, 0x2c)
            (ui, 0x427d8c28u, 0, &resolved)) {
        msg.mFlags = 0;
        msg.mCount = 0;
        msg.mPos.x = g_vecA[0];
        msg.mPos.y = g_vecA[1];
        msg.mPos.z = g_vecA[2];
        msg.mScale = g_fOne;
        ((C_41CB40*)&msg.mRot)->Set(g_rotA);

        msg.mFlags |= 4;
        ++msg.mCount;
        msg.mPos = *pos;

        FUN_004a9b40(editPos, &matrix);
        msg.mRot = matrix;
        msg.mFlags |= 2;
        ++msg.mCount;

        VSLOT(void (__thiscall*)(void*, void*), resolved, 0x18)(resolved, &msg);
        VSLOT(void (__thiscall*)(void*, int), resolved, 8)(resolved, 0);
    }

    // Hide the editor previews and drop the city-hall shadow.
    void* preview = VSLOT(void* (__thiscall*)(void*), tribe, 0xac)(tribe);
    void* previewSub = (u8*)preview + 0x34;
    u32*  previewFlags = (u32*)VSLOT(void* (__thiscall*)(void*), previewSub, 0xac)(previewSub);
    previewFlags[1] &= ~1u;

    u8* begin = (u8*)tribe + 0x210;
    void** it  = *(void***)begin;
    void** end = *(void***)((u8*)tribe + 0x214);
    for (; it != end; ++it) {
        void* obj = *it;
        void* sub = (u8*)obj + 0x34;
        if (VSLOT(void* (__thiscall*)(void*), sub, 0xac)(sub) != 0) {
            u32* flags = (u32*)VSLOT(void* (__thiscall*)(void*), sub, 0xac)(sub);
            flags[1] &= ~1u;
        }
    }

    PtrVec* list = (PtrVec*)VSLOT(void* (__thiscall*)(void*), tribe, 0xbc)(tribe);
    for (;;) {
        if (((int)(list->mEnd - list->mBegin) >> 2) <= 0)
            break;
        VSLOT(void (__thiscall*)(void*, void*), tribe, 0xb8)(tribe, *(void**)list->mBegin);
    }

    void** shadowSlot = (void**)((u8*)tribe + 0x260);
    void*  shadow = *shadowSlot;
    if (shadow != 0)
        VSLOT(void (__thiscall*)(void*), shadow, 0)(shadow);
    ((C_B225D0*)FUN_00b3d300())->Set(shadow);
    if (*shadowSlot != 0) {
        void* old = *shadowSlot;
        *shadowSlot = 0;
        VSLOT(void (__thiscall*)(void*), old, 4)(old);
    }

    ((C_C90470*)tribe)->Method();
    void* controller = ((C_C8FED0*)tribe)->Get();
    VSLOT(void (__thiscall*)(void*, int), controller, 0x54)(controller, 0);

    // Two endpoints of the transition; the parameter also carries the radius.
    C_RadiusParam param;
    ((C_AD79D0*)&param)->Method(editPos, tribe);

    float rx = (*(float*)((u8*)effect + 0x7c) + *(float*)((u8*)effect + 0x70)) * g_fHalf;
    float rz = (*(float*)((u8*)effect + 0x80) + *(float*)((u8*)effect + 0x74)) * g_fHalf;
    float d0x = *(float*)((u8*)effect + 0x7c) - rx;
    float d0z = *(float*)((u8*)effect + 0x80) - rz;
    float d1z = *(float*)((u8*)effect + 0x74) - rz;
    float d1x = *(float*)((u8*)effect + 0x70) - rx;
    float dist0 = (float)sqrt((double)(d0z * d0z + d0x * d0x));
    float dist1 = (float)sqrt((double)(d1z * d1z + d1x * d1x));

    float dist = dist1 > dist0 ? dist1 : dist0;
    param.mDistA = dist;
    param.mDistB = dist0 > dist1 ? dist0 : dist1;
    ((C_AE09BR*)FUN_00b3d4d0())->Send(0x66c82c23u, param, 0);

    void* leader = ((C_C8FD30*)tribeSub)->Get();
    {
        C_MsgParam p;
        ((C_AE09B0*)FUN_00b3d4d0())->Send(0x3d7f90au, InitMsgParam(p, tribeSub), 0);
        ((C_MsgParamRel*)&p)->Release();
    }
    {
        C_MsgParam p;
        ((C_AE09B0*)FUN_00b3d4d0())->Send(0xcd5f0ca4u,
            InitMsgParam(p, leader != 0 ? (void*)((u8*)leader + 0xc0) : 0), 0);
        ((C_MsgParamRel*)&p)->Release();
    }

    ((C_ADF390*)FUN_00b3d4d0())->Set(1);

    float timer = FUN_00cdb8b0();
    const char* tag;
    if (timer >= g_fThird)
        tag = g_tagMili;
    else if (timer >= g_fMinusThird)
        tag = g_tagEcon;
    else
        tag = g_tagReli;

    ((C_AE0930*)FUN_00b3d4d0())->Send(0x1477274u, 1, 0, tag, 0, 0);

    ((C_MsgParamRel*)&param)->Release();

    if (*shadowSlot != 0)
        VSLOT(void (__thiscall*)(void*), *shadowSlot, 4)(*shadowSlot);
    if (resolved != 0)
        VSLOT(void (__thiscall*)(void*), resolved, 4)(resolved);
}
