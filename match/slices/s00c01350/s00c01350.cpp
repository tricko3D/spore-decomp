// @ 0x00c01350
// SPGonzago / SPCommunity side: lane-zone helper.  When the lane zone is valid
// and the zone reports an inner radius, it measures the context position
// against the zone position and, if the point is close enough, asks the
// steering/lane query for a point on the way to the context target and appends
// one spawn record to the context's effect vector.
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast

#include "game/types.h"
#include <math.h>

// ---- shared globals (address in the comment) --------------------------------
extern const float kZero_s00c0_1350;   // 0x01485378   0.0f
extern const float kTwo_s00c0_1350;    // 0x01470f1c   2.0f
extern const float kOne_s00c0_1350;    // 0x01485720   1.0f

// ---- layouts (retail offsets; no canonical header covers this helper yet) ---
struct Vec3_s00c0_1350 { float x, y, z; };

// The position lerp is written component by component: the three components are
// copied to the record separately, which is what the original does.

struct cSpawnRec_s00c0_1350 {              // 0x2c-byte element of the vector at ctx+0x84
    uint32_t mKind;                        // +0x00   written as 4
    uint32_t mCount;                       // +0x04   written as 1
    Vec3_s00c0_1350 mPos;                  // +0x08
    float mScale;                          // +0x14   written as 1.0f
    uint32_t mPad18[5];                    // +0x18   rest of the record
};

struct cSpawnVec_s00c0_1350 {              // vector-like container at ctx+0x84
    cSpawnRec_s00c0_1350* mpBegin;         // +0x84
    cSpawnRec_s00c0_1350* mpEnd;           // +0x88
    cSpawnRec_s00c0_1350* mpCap;           // +0x8c
    void AddDefault();                     // 0x00b48e00  appends one default record
};

struct cLaneCtx_s00c0_1350 {               // the context the helper works on
    Vec3_s00c0_1350 mPos;                  // +0x00
    uint8_t mPad0c[0x14];                  // +0x0c
    Vec3_s00c0_1350 mTarget;               // +0x20
    uint8_t mPad2c[0x54];                  // +0x2c
    void* mpSource;                        // +0x80  vtable +0x70 returns a float
    cSpawnVec_s00c0_1350 mSpawns;          // +0x84
};

struct cLaneZone_s00c0_1350 {              // 'this'
    uint8_t mPad00[0x50];                  // +0x00 (vtable pointer included)
    float mInnerRadius;                    // +0x50
    float mOuterRadius;                    // +0x54
    uint8_t mPad58[0x14];                  // +0x58
    uint8_t mZoneValid;                    // +0x6c

    void Update_00c01350(cLaneCtx_s00c0_1350* ctx);
};

// ---- unnamed retail callees -------------------------------------------------
#define VT(obj, off) ((*(void***)(obj))[(off) / 4])
typedef Vec3_s00c0_1350* (__thiscall* FnGetPosition_s00c0_1350)(void*, Vec3_s00c0_1350* out);
typedef float (__thiscall* FnGetRadius_s00c0_1350)(void*);

// 0x00700c80: steering/lane query; reports the interpolation factor it picked
// through the last parameter and returns whether the point was accepted.
bool LaneQuery_00700c80(cLaneCtx_s00c0_1350* ctx, const Vec3_s00c0_1350* target,
                        Vec3_s00c0_1350* position, float radius, float* outT);

void cLaneZone_s00c0_1350::Update_00c01350(cLaneCtx_s00c0_1350* ctx)
{
    if (mZoneValid != 0)
        return;

    // Radius probe on the context's source object; the result is unused here.
    ((FnGetRadius_s00c0_1350)VT(ctx->mpSource, 0x70))(ctx->mpSource);

    if (!(mInnerRadius > kZero_s00c0_1350))
        return;

    Vec3_s00c0_1350 here;
    Vec3_s00c0_1350* pos = ((FnGetPosition_s00c0_1350)VT(this, 0x58))(this, &here);

    float dx = pos->x - ctx->mPos.x;
    float dy = pos->y - ctx->mPos.y;
    float dz = pos->z - ctx->mPos.z;
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    // Rejection test spelled with the negation: the original compares the
    // radius bound against the distance, which keeps the sum in st(0) and lowers
    // to `fcomip st(1); jb` (the unordered case also returns).
    if (!(distance <= mOuterRadius + kTwo_s00c0_1350))
        return;

    // The position query sits in the argument list: its result is the point the
    // steering query tests, and evaluating the later arguments first is what
    // lets the radius stay in the FPU across the call.
    float t;
    if (!LaneQuery_00700c80(ctx, &ctx->mTarget,
                            ((FnGetPosition_s00c0_1350)VT(this, 0x58))(this, &here),
                            mOuterRadius, &t))
        return;

    // Point part way from the context position towards the target.
    here.x = ctx->mPos.x + (ctx->mTarget.x - ctx->mPos.x) * t;
    here.y = ctx->mPos.y + (ctx->mTarget.y - ctx->mPos.y) * t;
    here.z = ctx->mPos.z + (ctx->mTarget.z - ctx->mPos.z) * t;

    ctx->mSpawns.AddDefault();
    cSpawnRec_s00c0_1350* rec = ctx->mSpawns.mpEnd - 1;
    rec->mKind = 4;
    rec->mCount = 1;
    rec->mPos.x = here.x;
    rec->mPos.y = here.y;
    rec->mPos.z = here.z;
    rec->mScale = kOne_s00c0_1350;
}
