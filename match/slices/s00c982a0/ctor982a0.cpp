// Slice s00c982a0: SP::cTribe::cTribe (0x00c982a0, 1040 bytes).
//
// Retail layout recovered from the constructor itself and from the sub-object
// constructors it calls:
//   +0x000 cCommunity prefix (0x120): cGameData has two vftables (+0x00/+0x04),
//          the editor-name interface vftable sits at +0x34
//   +0x120 cSpatialObject sub-object (0xd4)
//   +0x1f4 cInteractionTarget sub-object (0x18)
//   +0x20c cSPIdentity sub-object (0x24)
//   +0x230 cColorSet sub-object (0x30)
//   +0x260 members
// The generated SP/cTribe.h re-lays these to 0x118/0x19c/0x1b4/0x1d8, which does
// not agree with retail, so this slice carries its own view of the class.
//
// Toolchain: cl 15.00.30729.01 (VS2008 SP1), /O2 /MD /Gy /TP /arch:SSE /fp:fast.
#include <intrin.h>
#include "types.h"

namespace SP {

// ---------------------------------------------------------------- sub-objects

struct cTimer {
    unsigned char mRaw[0x20];
    void Construct();                                        // 0x00b63890
    void Restart();                                          // 0x00bc3170
};

struct cTimerA {                                             // timer at +0x530 (+0x28)
    unsigned char mRaw[0x20];
    int           m20;                                       // +0x20
    bool          mb24, mb25, mb26, mb27;                    // +0x24..+0x27
    void Construct();                                        // 0x00b63890
    void Restart();                                          // 0x00bc3170
};

struct RefObj {                                              // reference-counted object
    virtual void v00();
    virtual void Release();
};

struct Vec3f { float x, y, z; };

// 0x14-byte eastl vector header: begin / end / capacity + 8-byte allocator slot
// (the second allocator word is never written by a constructor).
struct SpVec {
    void* mpBegin;
    void* mpEnd;
    void* mpCapacity;
    unsigned mAllocator;
    unsigned mAllocatorPad;
};

// 0x10-byte eastl tree header as the constructor initialises it.
struct SpMap {
    unsigned m00;
    unsigned m04;
    unsigned m08;
    unsigned m0c;
};

struct Sub3B4 {                                              // +0x3b4 (0x64)
    unsigned char mRaw[0x64];
    void Construct();                                        // 0x00afba10
};

struct SubGrid {                                             // +0x558 (0x1310)
    unsigned char mRaw[0x1310];
    void Construct();                                        // 0x00cc7e10
};

struct Exchange {                                            // cTribeExchange, 0x18 bytes
    unsigned __int64 mTime;                                  // +0x00
    unsigned int     mInteractionID;                         // +0x08
    unsigned int     mOtherTribe;                            // +0x0c
    bool             mbInitiator;                            // +0x10
    bool             mbSuccess;                              // +0x11
};

struct FixedVec {                                            // mSpeciesKeys (+0x1904)
    void* mpBegin;                                           // +0x00
    void* mpEnd;                                             // +0x04
    void* mpCapacity;                                        // +0x08
    unsigned mAllocator0;                                    // +0x0c
    unsigned mAllocator1;                                    // +0x10
    unsigned m14;                                            // +0x14
    unsigned char mInline[0xa8];                             // +0x18
    void Reserve(int n);                                     // 0x004548d0
};

// ---------------------------------------------------------------- base classes

// Sub-object constructors, called explicitly from the retail constructor in this
// order: cCommunity (+0x00), cSpatialObject (+0x120), cInteractionTarget (+0x1f4),
// cSPIdentity (+0x20c) and cColorSet (+0x230).  These are plain storage views: the
// seven vftables are installed by the explicit stores below, exactly as retail does.
struct cCommunityCtorView { unsigned char m[0x120]; void Construct(); };   // 0x00c011c0
struct cSpatialCtorView   { unsigned char m[0x0d4]; void Construct(); };   // 0x00c89630
struct cTargetCtorView    { unsigned char m[0x018]; void Construct(); };   // 0x00ac03d0
struct cIdentityCtorView  { unsigned char m[0x024]; void Construct(); };   // 0x00cee630
struct cColorSetCtorView  { unsigned char m[0x030]; void Construct(); };   // 0x00b6f280

// ---------------------------------------------------------------- class

#pragma pack(push, 4)
class cTribe {
 public:
    cTribe();

    void*        mpVtbl;                                     // +0x000
    void*        mpVtbl04;                                   // +0x004
    unsigned char pad08[0x34 - 0x08];
    void*        mpVtbl34;                                   // +0x034
    unsigned char pad38[0x120 - 0x38];
    unsigned char mSub120[0xd4];                             // +0x120 cSpatialObject
    void*        mpVtbl1f4;                                  // +0x1f4
    unsigned char mSub1f8[0x20c - 0x1f8];
    void*        mpVtbl20c;                                  // +0x20c
    unsigned char mSub210[0x230 - 0x210];
    void*        mpVtbl230;                                  // +0x230
    unsigned char pad234[0x260 - 0x234];

    void*        mpFoodMat;                                  // +0x260
    bool         mbMotiveCheatOn;                            // +0x264
    float        mRSquareSize;                               // +0x268
    unsigned int mCurrentVignetteInstanceId;                 // +0x26c
    cTimer       mVignetteTimer;                             // +0x270

    void*        mp290;                                      // +0x290
    SpVec        mChatAreas;                                 // +0x294
    SpVec        mPanicBubbles;                              // +0x2a8

    unsigned int mAttitudeTowardPlayerTribe;                 // +0x2bc
    float        mF2c0;                                      // +0x2c0
    float        mF2c4;                                      // +0x2c4
    float        mF2c8;                                      // +0x2c8
    float        mF2cc;                                      // +0x2cc
    void*        mp2d0;                                      // +0x2d0
    void*        mp2d4;                                      // +0x2d4
    unsigned int mU2d8;                                      // +0x2d8
    unsigned char pad2dc[0x300 - 0x2dc];
    bool         mbCheckedForWater;                          // +0x300
    bool         mbCheckedForForests;                        // +0x301
    unsigned char pad302[2];
    float        mClosestWaterX;                             // +0x304
    float        mClosestWaterY;                             // +0x308
    float        mClosestWaterZ;                             // +0x30c
    float        mClosestWaterDistance;                      // +0x310
    SpVec        mClosestForests;                            // +0x314
    unsigned int mU328;                                      // +0x328
    float        mF32c;                                      // +0x32c
    float        mF330;                                      // +0x330
    bool         mb334;                                      // +0x334
    unsigned char pad335[3];
    int          mI338;                                      // +0x338
    bool         mb33c;                                      // +0x33c
    unsigned char pad33d[3];
    SpVec        mVec340;                                    // +0x340
    SpVec        mVec354;                                    // +0x354
    void*        mpRef368;                                   // +0x368
    SpVec        mVec36c;                                    // +0x36c
    SpVec        mVec380;                                    // +0x380
    SpMap        mMap394;                                    // +0x394
    float        mF3a4;                                      // +0x3a4
    float        mF3a8;                                      // +0x3a8
    unsigned int mU3ac;                                      // +0x3ac
    unsigned char pad3b0[4];
    Sub3B4       mSub3b4;                                    // +0x3b4
    SpVec        mVec418;                                    // +0x418
    unsigned int mU42c;                                      // +0x42c
    unsigned char mInline430[0xb4];                          // +0x430 .. +0x4e4
    SpMap        mMap4e4;                                    // +0x4e4
    float        mF4f4;                                      // +0x4f4
    float        mF4f8;                                      // +0x4f8
    unsigned int mU4fc;                                      // +0x4fc
    unsigned char pad500[0x10];                              // +0x500 .. +0x510
    cTimer       mTimer510;                                  // +0x510
    cTimerA      mTimer530;                                  // +0x530
    SubGrid      mGrid558;                                   // +0x558 .. +0x1868
    Exchange     mTrackedInteractions[4];                    // +0x1868 .. +0x18c8
    void*        mpOwned18c8;                                // +0x18c8
    unsigned char mSpecies18cc[0x38];                        // +0x18cc .. +0x1904
    FixedVec     mSpeciesKeys;                               // +0x1904 .. +0x19c4
    void*        mpRef19c4;                                  // +0x19c4
    void*        mpRef19c8;                                  // +0x19c8
    void*        mpRef19cc;                                  // +0x19cc
    void*        mpRef19d0;                                  // +0x19d0
    void*        mp19d4;                                     // +0x19d4
    void*        mp19d8;                                     // +0x19d8
};
#pragma pack(pop)

}  // namespace SP

// ------------------------------------------------------------------- helpers

extern "C" {
float gZeroVector3[3];                                       // 0x01695374: { 0, 0, 0 }
void* gEmptyTree154df28;                                     // 0x0154df28
void* cdecl_memset(void* dst, int value, unsigned size);     // 0x011e073e memset
void* cdecl_alloc(unsigned size, const char* what, int a, int b, int c, int d);  // 0x00f473a0
}

namespace SP {
struct cOwned18c8 {                                          // 0x18-byte object stored at +0x18c8
    cOwned18c8* Construct();                                 // 0x01062ee0
};
}

// @ 0x00c982a0
//
// SP::cTribe::cTribe -- base sub-objects first (the compiler emits those calls and
// the seven vftable stores), then the member initialisation in retail order.
SP::cTribe::cTribe()
{
    // Base sub-objects, in retail order (eax/ecx are the only scratch registers
    // here; the sub-object addresses stay live in edi/ebx/ebp for the vftables).
    ((cCommunityCtorView*)this)->Construct();                // +0x000
    cSpatialCtorView* spatial = (cSpatialCtorView*)(void*)mSub120;
    spatial->Construct();                                    // +0x120
    cTargetCtorView* target = (cTargetCtorView*)(void*)((char*)this + 0x1f4);
    target->Construct();                                     // +0x1f4
    cIdentityCtorView* identity = (cIdentityCtorView*)(void*)((char*)this + 0x20c);
    identity->Construct();                                   // +0x20c
    ((cColorSetCtorView*)(void*)((char*)this + 0x230))->Construct();  // +0x230

    // The seven vftables, in retail order.
    *(void**)target = (void*)0x1473d64;                      // +0x1f4 cInteractionTarget
    *(void**)spatial = (void*)0x1473d80;                     // +0x120 cSpatialObject
    mpVtbl = (void*)0x1473e58;                               // +0x000 cTribe
    mpVtbl04 = (void*)0x1473e44;                             // +0x004 cTribe
    mpVtbl34 = (void*)0x1469cc0;                             // +0x034 name provider
    *(void**)identity = (void*)0x1473d20;                    // +0x20c cSPIdentity
    mpVtbl230 = (void*)0x1473d00;                            // +0x230 cColorSet

    mpFoodMat = 0;                                           // +0x260
    mbMotiveCheatOn = false;                                 // +0x264
    mRSquareSize = 10.0f;                                    // +0x268
    mCurrentVignetteInstanceId = 0xffffffff;                 // +0x26c
    mVignetteTimer.Construct();                              // +0x270

    mp290 = 0;                                               // +0x290
    mChatAreas.mpBegin = 0;                                  // +0x294
    mChatAreas.mpEnd = 0;                                    // +0x298
    mChatAreas.mpCapacity = 0;                               // +0x29c
    mPanicBubbles.mpBegin = 0;                               // +0x2a8
    mPanicBubbles.mpEnd = 0;                                 // +0x2ac
    mPanicBubbles.mpCapacity = 0;                            // +0x2b0

    _ReadWriteBarrier();   // retail starts the zero-float block after the two vectors
    mF2c0 = 0.0f;                                            // +0x2c0
    mF2c4 = 0.0f;                                            // +0x2c4
    mF2c8 = 0.0f;                                            // +0x2c8
    mF2cc = 0.0f;                                            // +0x2cc
    mAttitudeTowardPlayerTribe = 0xffffffff;                 // +0x2bc
    mp2d0 = 0;                                               // +0x2d0
    mp2d4 = 0;                                               // +0x2d4
    mU2d8 = 0;                                               // +0x2d8

    mbCheckedForWater = false;                               // +0x300
    mbCheckedForForests = false;                             // +0x301
    mClosestWaterX = gZeroVector3[0];                        // +0x304
    mClosestWaterY = gZeroVector3[1];                        // +0x308
    mClosestWaterZ = gZeroVector3[2];                        // +0x30c
    _ReadWriteBarrier();
    mClosestWaterDistance = 0.0f;                            // +0x310
    mClosestForests.mpBegin = 0;                             // +0x314
    mClosestForests.mpEnd = 0;                               // +0x318
    mClosestForests.mpCapacity = 0;                          // +0x31c

    mF330 = 30.0f;                                           // +0x330
    mU328 = 2;                                               // +0x328
    mb334 = false;                                           // +0x334
    mI338 = 0;                                               // +0x338
    mb33c = false;                                           // +0x33c
    mVec340.mpBegin = 0;                                     // +0x340
    mVec340.mpEnd = 0;                                       // +0x344
    mVec340.mpCapacity = 0;                                  // +0x348
    mVec354.mpBegin = 0;                                     // +0x354
    mVec354.mpEnd = 0;                                       // +0x358
    mVec354.mpCapacity = 0;                                  // +0x35c
    mpRef368 = 0;                                            // +0x368
    mVec36c.mpBegin = 0;                                     // +0x36c
    mVec36c.mpEnd = 0;                                       // +0x370
    mVec36c.mpCapacity = 0;                                  // +0x374
    mVec380.mpBegin = 0;                                     // +0x380
    mVec380.mpEnd = 0;                                       // +0x384
    mVec380.mpCapacity = 0;                                  // +0x388

    _ReadWriteBarrier();
    mF3a4 = 1.0f;                                            // +0x3a4
    mF3a8 = 2.0f;                                            // +0x3a8
    void* emptyTree = &gEmptyTree154df28;
    mMap394.m04 = (unsigned)emptyTree;                       // +0x398
    mMap394.m08 = 1;                                         // +0x39c
    mMap394.m0c = 0;                                         // +0x3a0
    mU3ac = 0;                                               // +0x3ac

    mSub3b4.Construct();                                     // +0x3b4

    void** inlineStart = (void**)mInline430;
    mVec418.mpBegin = inlineStart;                           // +0x418
    mVec418.mpEnd = inlineStart;                             // +0x41c
    mU42c = 0;                                               // +0x42c
    mVec418.mpCapacity = (void*)((char*)inlineStart + 0xb4); // +0x420

    mF4f4 = 1.0f;                                            // +0x4f4
    mF4f8 = 2.0f;                                            // +0x4f8
    mMap4e4.m04 = (unsigned)emptyTree;                       // +0x4e8
    mMap4e4.m08 = 1;                                         // +0x4ec
    mMap4e4.m0c = 0;                                         // +0x4f0
    mU4fc = 0;                                               // +0x4fc

    mTimer510.Construct();                                   // +0x510
    mTimer530.Construct();                                   // +0x530
    mTimer530.m20 = 0;                                       // +0x550
    mTimer530.mb24 = false;                                  // +0x554
    mTimer530.mb25 = false;                                  // +0x555
    mTimer530.mb26 = false;                                  // +0x556
    mTimer530.mb27 = false;                                  // +0x557

    mGrid558.Construct();                                    // +0x558

    mTrackedInteractions[0].mTime = 0;                       // +0x1868
    mTrackedInteractions[0].mInteractionID = 0;              // +0x1870
    mTrackedInteractions[0].mOtherTribe = 0xffffffff;        // +0x1874
    mTrackedInteractions[0].mbInitiator = false;             // +0x1878
    mTrackedInteractions[0].mbSuccess = false;               // +0x1879
    mTrackedInteractions[1].mTime = 0;                       // +0x1880
    mTrackedInteractions[1].mInteractionID = 0;              // +0x1888
    mTrackedInteractions[1].mOtherTribe = 0xffffffff;        // +0x188c
    mTrackedInteractions[1].mbInitiator = false;             // +0x1890
    mTrackedInteractions[1].mbSuccess = false;               // +0x1891
    mTrackedInteractions[2].mTime = 0;                       // +0x1898
    mTrackedInteractions[2].mInteractionID = 0;              // +0x18a0
    mTrackedInteractions[2].mOtherTribe = 0xffffffff;        // +0x18a4
    mTrackedInteractions[2].mbInitiator = false;             // +0x18a8
    mTrackedInteractions[2].mbSuccess = false;               // +0x18a9
    mTrackedInteractions[3].mTime = 0;                       // +0x18b0
    mTrackedInteractions[3].mInteractionID = 0;              // +0x18b8
    mTrackedInteractions[3].mOtherTribe = 0xffffffff;        // +0x18bc
    mTrackedInteractions[3].mbInitiator = false;             // +0x18c0
    mTrackedInteractions[3].mbSuccess = false;               // +0x18c1
    mpOwned18c8 = 0;                                         // +0x18c8

    FixedVec* speciesKeys = &mSpeciesKeys;
    unsigned char* inlinePtr = speciesKeys->mInline;
    speciesKeys->mpBegin = inlinePtr;                        // +0x1904
    speciesKeys->mpEnd = inlinePtr;                          // +0x1908
    speciesKeys->m14 = 0;                                    // +0x1918
    speciesKeys->mpCapacity = inlinePtr + 0xa8;              // +0x190c

    mpRef19c4 = 0;                                           // +0x19c4
    mpRef19c8 = 0;                                           // +0x19c8
    mpRef19cc = 0;                                           // +0x19cc
    mpRef19d0 = 0;                                           // +0x19d0
    mp19d4 = 0;                                              // +0x19d4
    mp19d8 = 0;                                              // +0x19d8

    void* speciesDst = mSpecies18cc;
    cdecl_memset(speciesDst, 0, 0x38);                       // +0x18cc

    speciesKeys->Reserve(0xe);                               // +0x1904

    if (mpRef368) {
        RefObj* old = (RefObj*)mpRef368;
        mpRef368 = 0;
        old->Release();
    }

    mTimer530.Restart();                                     // +0x530
    mTimer510.Restart();                                     // +0x510

    void* owned = cdecl_alloc(0x18, "Simulator::cTribe", 0, 0, 0, 0);
    mpOwned18c8 = owned ? ((cOwned18c8*)owned)->Construct() : 0;
}
