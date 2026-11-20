// Slice s00cdc8b0 -- stamp the transition targets onto the planet model.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
//
//   0x00cdc8b0 FUN_00cdc8b0   reads three float properties from the shared property
//                             provider (0x01581288) and applies them, scaled by each
//                             target's own float slot +0x60, to every noun in the game
//                             data vector produced by 0xb21340.
//
// The property unpack appears three times verbatim in the original (the compiler
// inlined it three times); it is spelled once as a macro so the three bodies stay
// identical.  Field offsets and virtual slots are named at their retail positions.

#include "types.h"

// ---------------------------------------------------------------------------------------------
//  external callees and globals
// ---------------------------------------------------------------------------------------------

void* FUN_00b3d350();                       // 0x00b3d350  the planet model / stamp target owner
void* FUN_00b3d300();                       // 0x00b3d300  the noun manager

extern void*  DAT_01581288;                 // 0x01581288  shared property provider
extern float  DAT_01485720;                 // 0x01485720  1.0f
extern float  DAT_01473c70;                 // 0x01473c70  10.0f


// ---------------------------------------------------------------------------------------------
//  vtable leaders: B<n> carries n+1 virtual slots (offsets 0x00 .. 4*n)
// ---------------------------------------------------------------------------------------------

struct B00 { virtual void a00() {} };
struct B01 : B00 { virtual void a01() {} };
struct B02 : B01 { virtual void a02() {} };
struct B03 : B02 { virtual void a03() {} };
struct B04 : B03 { virtual void a04() {} };
struct B05 : B04 { virtual void a05() {} };
struct B06 : B05 { virtual void a06() {} };
struct B07 : B06 { virtual void a07() {} };
struct B08 : B07 { virtual void a08() {} };
struct B09 : B08 { virtual void a09() {} };
struct B10 : B09 { virtual void a10() {} };
struct B11 : B10 { virtual void a11() {} };
struct B12 : B11 { virtual void a12() {} };
struct B13 : B12 { virtual void a13() {} };
struct B14 : B13 { virtual void a14() {} };
struct B15 : B14 { virtual void a15() {} };
struct B16 : B15 { virtual void a16() {} };
struct B17 : B16 { virtual void a17() {} };
struct B18 : B17 { virtual void a18() {} };
struct B19 : B18 { virtual void a19() {} };
struct B20 : B19 { virtual void a20() {} };
struct B21 : B20 { virtual void a21() {} };
struct B22 : B21 { virtual void a22() {} };
struct B23 : B22 { virtual void a23() {} };
struct B24 : B23 { virtual void a24() {} };
struct B25 : B24 { virtual void a25() {} };
struct B26 : B25 { virtual void a26() {} };
struct B27 : B26 { virtual void a27() {} };
struct B28 : B27 { virtual void a28() {} };
struct B29 : B28 { virtual void a29() {} };
struct B30 : B29 { virtual void a30() {} };
struct B31 : B30 { virtual void a31() {} };
struct B32 : B31 { virtual void a32() {} };
struct B33 : B32 { virtual void a33() {} };
struct B34 : B33 { virtual void a34() {} };
struct B35 : B34 { virtual void a35() {} };
struct B36 : B35 { virtual void a36() {} };
struct B37 : B36 { virtual void a37() {} };
struct B38 : B37 { virtual void a38() {} };
struct B39 : B38 { virtual void a39() {} };
struct B40 : B39 { virtual void a40() {} };
struct B41 : B40 { virtual void a41() {} };
struct B42 : B41 { virtual void a42() {} };

// --- single-slot vtable views ----------------------------------------------------------------

struct cVtV24  : public B08 { virtual uint8_t v24(int, void**) { return 0; }; };  // +0x24
struct cVtV2c  : public B10 { virtual void*   v2c() { return 0; }; };             // +0x2c
struct cVtV60  : public B23 { virtual float   v60() { return 0.0f; }; };           // +0x60
struct cVtV74  : public B28 { virtual float   v74() { return 0.0f; }; };           // +0x74
struct cVtVac  : public B42 { virtual void*   vac() { return 0; }; };              // +0xac

// --- the unpacked property record ------------------------------------------------------------

struct cPropValue
{
    void*    mp00;                          // +0x00  pointer payload / inline float
    uint8_t  pad04[0x10 - 0x04];
    uint8_t  m10;                           // +0x10  property flags
    uint8_t  pad11;
    uint16_t m12;                           // +0x12  property kind (0x0d / 0x10 are float kinds)
};

// Unpack one float property into `dst`.  The out-pointer is deliberately left
// uninitialised: 0x24 writes it before succeeding, and the original keeps no
// zero store in the frame.
#define READ_TRANSITION_FLOAT(dst, key, fallback)                                   \
    {                                                                               \
        cPropValue* q_;                                                             \
        if (DAT_01581288 && ((cVtV24*)DAT_01581288)->v24((key), (void**)&q_))       \
        {                                                                           \
            uint16_t k_ = *(uint16_t*)((char*)q_ + 0x12);                           \
            const float* kFallback_ = (const float*)0x15d9c6c;                       \
            if (k_ != 0x0d && k_ != 0x10)                                           \
                (dst) = *kFallback_;                                                \
            else if (q_->m10 & 0x30)                                                \
                (dst) = *(float*)q_->mp00;                                          \
            else                                                                    \
                (dst) = *(float*)((k_ != 0 ? -1 : 0) & (int)q_);                    \
        }                                                                           \
        else                                                                        \
            (dst) = (fallback);                                                     \
    }

// --- views on foreign objects ----------------------------------------------------------------

// one entry of the game-data vector: vtable up to +0xac, a second polymorphic
// sub-object at +0x120 and a pointer at +0x260 whose +0x34 member is a float source
struct cStampTarget
{
    uint8_t  pad0[0x120];
    cVtV2c   m120;                          // +0x120  destination transform
    uint8_t  pad1[0x260 - 0x124];
    void*    mp260;                         // +0x260  optional scale source
};

struct cObj34f  { uint8_t pad0[0x34]; cVtV74 m34; };   // +0x34  float source
struct cObj34p  { uint8_t pad0[0x34]; cVtV2c m34; };   // +0x34  pointer source

// row set returned by 0xb21340: a begin/end pair bound as an lvalue at +0x04 (the
// original computes &rows once, so the pair is read as rows->pBegin / rows->pEnd)
struct cVec { void** pBegin; void** pEnd; };
struct cRowSet { uint32_t m00; cVec rows; };

// the noun manager (0xb3d300)
struct cNounManager
{
    cRowSet* GetGameDataVector(void*, void*, void*, void*, void*);   // 0xb21340
};

// the stamp target owner (0xb3d350)
struct cPlanetModel
{
    void FUN_00b83c40(void* pTarget, float fValue, int nFlags, int nArg);   // 0x00b83c40
    void FUN_00b81020(void* pTarget, float fValue, int nFlags);             // 0x00b81020
};

// ---------------------------------------------------------------------------------------------

// @ 0x00cdc8b0
void FUN_00cdc8b0()
{
    float v0;
    float v1;
    float v2;

    READ_TRANSITION_FLOAT(v0, 0x080e984f, DAT_01485720);
    READ_TRANSITION_FLOAT(v1, 0x22748972, 0.0f);
    READ_TRANSITION_FLOAT(v2, 0x0af5e325, DAT_01473c70);

    cPlanetModel* model = (cPlanetModel*)FUN_00b3d350();
    cRowSet*      set   = ((cNounManager*)FUN_00b3d300())->GetGameDataVector(
                              (void*)0xcd7d10, (void*)0xd3d420, (void*)0xaccbb0,
                              (void*)0xb1e500, (void*)0x18c6d19);

    cVec*  rows = &set->rows;
    void** it   = rows->pBegin;
    void** end  = rows->pEnd;

    for (; it != end; ++it)
    {
        cStampTarget* target = (cStampTarget*)*it;
        void*         dest   = target->m120.v2c();
        float         scale  = ((cVtV60*)target)->v60();

        model->FUN_00b83c40(dest, scale * v0, 0x4000000, 0);
        model->FUN_00b81020(dest, scale * v1, 0x4000000);

        // the optional scale source is loaded after the first two stamps (the
        // original reuses the tested pointer for the virtual call below)
        void* p260 = target->mp260;
        if (p260)
            model->FUN_00b81020(dest, ((cObj34f*)p260)->m34.v74(), 0x4000000);

        model->FUN_00b83c40(((cObj34p*)((cVtVac*)target)->vac())->m34.v2c(),
                            v2, 0x10000000, 0);
    }
}
