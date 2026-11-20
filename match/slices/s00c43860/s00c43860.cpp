// SP::cLocomotiveObject::cLocomotiveObject (retail layout).
//
// The generated SP/cLocomotiveObject.h layout is dev-based and wrong for
// retail (the object at +0xd4 is built by 0xac14d0 and Reset by 0xac5330, the
// two fixed vectors sit at +0x278 and +0x490 with their inline storage at
// +0x290/+0x4a8 and the last dword is at +0x4cc, while the header claims
// +0x84 and 0x3f8), so the class defined here uses the retail offsets observed
// in the constructor.  Member names are offset-based until the header is
// re-laid; base-class members below 0xd4 are addressed through the
// SP::cSpatialObject base image.
#include "game/types.h"
#include <intrin.h>

typedef unsigned int u32;
typedef unsigned char u8;

extern "C" float FUN_009360d0(void*);  // 0x9360d0 EA::Random::RandomLinearCongruential::RandomDoubleUniform
extern char DAT_0167ec50;              // 0x167ec50 the global random generator

extern float DAT_0168e7e4, DAT_0168e7e8, DAT_0168e7ec;  // default direction triple
extern float DAT_01485720;   // 1.0f
extern float DAT_0157424c;   // pi
extern float DAT_01470f1c;   // 2.0f
extern float DAT_013ec4b4;   // 0.9f
extern float DAT_0146cf2c;   // 0.0f
extern float DAT_0146cf24;   // 0.0f
extern float DAT_0140e87c;   // variance scale
extern float DAT_013ec480;   // separation variance range
extern float DAT_01486374;   // separation variance base
extern float DAT_013eb8c8;   // speed variance range
extern float DAT_01485ac0;   // speed variance base

struct cSpatialObjectBase {
    cSpatialObjectBase();  // 0xc89630 SP::cSpatialObject::cSpatialObject
    virtual ~cSpatialObjectBase();
    u8 mPad_004[0xd0];     // retail sizeof(cSpatialObject) == 0xd4
};

struct cAStarClientBase {
    cAStarClientBase();    // 0xac14d0
    virtual void OnPathCompleted(bool);
    u32 mPad_004;          // the sub-object at +0xd4 is 8 bytes wide
};

struct cLocomotiveObject : cSpatialObjectBase, cAStarClientBase {
    cLocomotiveObject();
    virtual ~cLocomotiveObject();

    u32 mField_0dc, mField_0e0, mField_0e4;
    u32 mField_0e8, mField_0ec;
    void* mpField_0f0, *mpField_0f4, *mpField_0f8, *mpField_0fc;
    void* mpField_100;
    u8    pad_104[0x84];             // 0x104..0x188

    float mField_188, mField_18c;     // variance values, computed from six random draws below
    float mField_190, mField_194, mField_198;
    u8    mField_19c;
    float mField_1a0;
    u32   mField_1a4;
    float mField_1a8, mField_1ac, mField_1b0;
    float mField_1b4, mField_1b8, mField_1bc;
    float mField_1c0, mField_1c4;
    float mField_1c8, mField_1cc, mField_1d0;
    float mField_1d4, mField_1d8, mField_1dc;
    float mField_1e0, mField_1e4, mField_1e8, mField_1ec;
    u32   mField_1f0, mField_1f4, mField_1f8, mField_1fc;
    u8    pad_200[0x08];             // 0x200..0x208
    float mField_208, mField_20c, mField_210, mField_214;
    u32   mField_218;
    u8    pad_21c[0x24];             // 0x21c..0x240
    u8    mField_240;
    float mField_244, mField_248, mField_24c;
    u32   mField_250;
    float mField_254, mField_258, mField_25c, mField_260;
    u32   mField_264;
    u8    mField_268;
    float mField_26c;
    u32   mField_270;
    u8    mField_274;
    u8    pad_275[0x03];             // 0x275..0x278

    // sp_fixed_vector<EA::AutoRefCount<SP::cSpatialObject>, 128>
    void* mpField_278, *mpField_27c, *mpField_280;
    u8    pad_284[0x08];             // 0x284..0x28c
    u32   mField_28c;
    u8    pad_290[0x200];            // inline storage, 0x290..0x490

    // sp_fixed_vector<void (__cdecl*)(SP::tSteeringData &), 4>
    void* mpField_490, *mpField_494, *mpField_498;
    u8    pad_49c[0x08];             // 0x49c..0x4a4
    u32   mField_4a4;
    u8    pad_4a8[0x10];             // inline storage, 0x4a8..0x4b8

    u32   mField_4b8, mField_4bc, mField_4c0;
    u8    pad_4c4[0x08];             // 0x4c4..0x4cc
    u32   mField_4cc;
};

// The _ReadWriteBarrier() calls below are scheduling levers, not logic: without
// them MSVC hoists the first xorps above the two vtable stores and sinks the
// +0x25c/+0x28c/+0x4a4 stores behind the following pointer setup, while retail
// keeps the member stores in declaration order.
// @ 0x00c43860
cLocomotiveObject::cLocomotiveObject()
{
    _ReadWriteBarrier();
    mField_0dc = 0;
    mField_0e0 = 0;
    mField_0e4 = 0;

    void* storage = (u8*)this + 0x108;
    mpField_100 = storage;
    mpField_0f4 = storage;
    mpField_0f0 = storage;
    storage = (u8*)storage + 0x80;
    mpField_0f8 = storage;

    // through a pointer: written as two member stores, MSVC 2008 moves the pair
    // up to just behind the xorps, ahead of the +0xdc block
    float* zp = &mField_188;
    zp[0] = 0.0f;
    zp[1] = 0.0f;

    mField_190 = DAT_0168e7e4;
    mField_194 = DAT_0168e7e8;
    mField_198 = DAT_0168e7ec;
    mField_19c = 0;
    mField_1a0 = 0.0f;
    mField_1a4 = 0;

    mField_1a8 = DAT_0168e7e4;
    mField_1ac = DAT_0168e7e8;
    mField_1b0 = DAT_0168e7ec;

    mField_1b4 = DAT_0168e7e4;
    mField_1b8 = DAT_0168e7e8;
    mField_1bc = DAT_0168e7ec;

    mField_1c0 = 0.0f;
    mField_1c4 = 0.0f;

    mField_1c8 = DAT_0168e7e4;
    mField_1cc = DAT_0168e7e8;
    mField_1d0 = DAT_0168e7ec;

    // the 1.0f and pi constants are held in locals used twice / once far from
    // their loads: retail keeps 1.0f in xmm1 across the +0x208 block and pi in
    // xmm2, which only a named local reproduces
    float pi = DAT_0157424c;
    mField_1d4 = DAT_0168e7e4;
    mField_1d8 = DAT_0168e7e8;
    mField_1dc = DAT_0168e7ec;

    float one = DAT_01485720;   // 1.0f
    mField_1e0 = 0.0f;
    mField_1e4 = one;
    mField_1e8 = pi;
    mField_1ec = 0.0f;

    mField_1f0 = 1;
    mField_1f4 = 0;
    mField_1f8 = 0;
    mField_1fc = 0;

    mField_208 = DAT_0168e7e4;
    mField_20c = DAT_0168e7e8;
    mField_210 = DAT_0168e7ec;
    mField_214 = one;
    mField_218 = 0;

    _ReadWriteBarrier();
    mField_240 = 0;
    mField_244 = 0.0f;
    mField_248 = 0.0f;
    mField_24c = 0.0f;

    mField_254 = DAT_01470f1c;
    mField_258 = DAT_013ec4b4;
    float k1 = DAT_0146cf2c;
    mField_260 = 0.0f;
    float k0 = DAT_0146cf24;
    mField_250 = 0;
    mField_25c = k1;
    mField_264 = 0;
    mField_268 = 1;
    mField_26c = k0;
    mField_270 = 0;
    mField_274 = 0;
    _ReadWriteBarrier();

    // the neighbours vector: begin == end == the inline storage, capacity its end
    void* neighbors = (u8*)this + 0x290;
    mpField_278 = neighbors;
    mpField_27c = neighbors;
    mField_28c = 0;
    _ReadWriteBarrier();
    neighbors = (u8*)neighbors + 0x200;
    mpField_280 = neighbors;
    _ReadWriteBarrier();

    // the steering-callback vector, four slots of 4 bytes
    void* steering = (u8*)this + 0x4a8;
    mpField_490 = steering;
    mpField_494 = steering;
    mField_4a4 = 0;
    _ReadWriteBarrier();
    steering = (u8*)steering + 0x10;
    mpField_498 = steering;

    mField_4b8 = 0;
    mField_4bc = 0;
    mField_4c0 = 0;
    mField_4cc = 0;

    // two 0..1 uniform draws summed and scaled into the separation / speed
    // variance ranges (x87 for the arithmetic, SSE for the stores)
    float r0 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    float r1 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    float r2 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    mField_188 = ((r2 + r1) + r0) * DAT_0140e87c * DAT_013ec480 + DAT_01486374;

    float r3 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    float r4 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    float r5 = ((float (__thiscall*)(void*))FUN_009360d0)(&DAT_0167ec50);
    mField_18c = ((r5 + r4) + r3) * DAT_0140e87c * DAT_013eb8c8 + DAT_01485ac0;

    ((u8*)this)[0xa6] = 0;
    ((u8*)this)[0x71] = 0;
}
