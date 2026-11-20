// Slice s00e89530 -- SP::cBuildingView::SpawnAndPlaceDamageEffects (0x00e89530,
// 573 bytes, __thiscall, ret 0xc).
//
// Called from cBuildingView::HandleSimulationUpdate (0x00e8a510) whenever the
// building owns a non-empty transform table (+0x308/+0x30c, 0x38-byte records) and
// one of the two damage effects (fire / smoke) has to be shown.  Each of the four
// damage stages holds an index into that table (+0x48..+0x54, -1 = unused).  For
// every stage whose effect is not running yet, the record's position and matrix are
// copied into a 0x38-byte transform message and handed to the new effect through
// its vtable[0x18]; the effect is then started.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

extern "C" void* memcpy(void* dst, const void* src, unsigned int size);

// Small non-POD vector: the by-value assignment (and the copy constructor it needs)
// keeps retail's per-row copies of the identity basis going through one 12-byte
// stack temporary.
struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {}
    Vec3& operator=(const Vec3& o) { x = o.x; y = o.y; z = o.z; return *this; }
};

struct Matrix3 {
    Vec3 mRow[3];

    void SetRow(int i, Vec3 v) { memcpy(&mRow[i], &v, sizeof(Vec3)); }
};

// ---- globals ---------------------------------------------------------------------------
extern Matrix3      g_identity;    // 0x016c4b98  unit basis (3x3)
extern Vec3         g_axisX;       // 0x016c4bbc  reference position vector
extern const float  g_one;         // 0x01485720  1.0f

// ---- the 0x38-byte transform message ---------------------------------------------------
struct cXformMsg {
    u16     mFlags;                // +0x00  |2 = matrix set, |4 = position set
    u16     mCount;                // +0x02  number of components set
    float   mPos[3];               // +0x04
    float   mScale;                // +0x10
    Matrix3 mMatrix;               // +0x14

    void SetMatrix(const Matrix3& m)
    {
        memcpy(&mMatrix, &m, sizeof(mMatrix));
        mFlags |= 2;
        mCount += 1;
    }

    void SetPosition(const Vec3& p)
    {
        memcpy(mPos, &p, sizeof(mPos));
        mFlags |= 4;
        mCount += 1;
    }
};

// ---- the building's transform table ----------------------------------------------------
struct cDamageXform {              // one 0x38-byte table record
    u32     mFlags;                // +0x00
    Vec3    mPos;                  // +0x04
    float   mScale;                // +0x10
    Matrix3 mMatrix;               // +0x14
};

struct cTransformTable {
    u8             mPad_308[0x308];
    cDamageXform*  mpBegin;        // +0x308
    cDamageXform*  mpEnd;          // +0x30c
};

// ---- effect object (vtable[0x18] = transform) ------------------------------------------
struct cEffect {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void SetTransform(cXformMsg* msg);   // +0x18
};

// ---- the world object the view talks to through its +0x0c pointer ----------------------
struct cSpatialObject {
    bool  GetEffect(int id);                        // 0x00c88910
    void* CreateEffect(u32 desc, int hard, int id); // 0x00c8b060
    void  StartEffect(int id, int hard);            // 0x00c88980
};

// ---- the view --------------------------------------------------------------------------
class cBuildingView {
public:
    u8               mPad_00c[0x0c];
    cSpatialObject*  mpObject;      // +0x0c
    u8               mPad_010[0x48 - 0x10];
    int              mXformIndices[4];  // +0x48, +0x4c, +0x50, +0x54

    void SpawnAndPlaceDamageEffects(cTransformTable* table, int effectId, int effectDesc);
};

// @ 0x00e89530
void cBuildingView::SpawnAndPlaceDamageEffects(cTransformTable* table, int effectId,
                                               int effectDesc)
{
    u32 count = (u32)(table->mpEnd - table->mpBegin);

    if (count != 0) mXformIndices[0] = 0;
    if (count > 1)  mXformIndices[1] = 1;
    if (count > 2)  mXformIndices[2] = 2;
    if (count > 3)  mXformIndices[3] = 3;

    cXformMsg msg;
    msg.mFlags = 0;
    msg.mCount = 0;
    msg.mPos[0] = g_axisX.x;
    msg.mPos[1] = g_axisX.y;
    msg.mPos[2] = g_axisX.z;
    msg.mScale  = g_one;

    msg.mMatrix.SetRow(0, g_identity.mRow[0]);
    msg.mMatrix.SetRow(1, g_identity.mRow[1]);
    msg.mMatrix.SetRow(2, g_identity.mRow[2]);

    for (int i = 0; i < 4; ++i) {
        if (mXformIndices[i] == -1)
            continue;
        if (mpObject->GetEffect(effectId + i))
            continue;

        cEffect* fx = (cEffect*)mpObject->CreateEffect((u32)effectDesc, 0, effectId + i);
        if (fx == 0)
            continue;

        const cDamageXform* rec = &table->mpBegin[mXformIndices[i]];
        msg.SetMatrix(rec->mMatrix);
        msg.SetPosition(rec->mPos);

        fx->SetTransform(&msg);
        mpObject->StartEffect(effectId + i, 1);
    }
}
