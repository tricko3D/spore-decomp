// Slice s00e0c490 - SporeApp 0x00e0c490..0x00e0c600
// Direction-pair setter: copies two direction vectors into the mode, replaces
// each with its unit-length version (a small epsilon guards the reciprocal
// square root) and then notifies the interface subobject and the mode itself.
// Flags: /O2 /Ob1 /MD /Gy /EHsc /TP /arch:SSE /GS- /fp:fast
#include "types.h"
#include <math.h>

typedef unsigned char u8;
typedef unsigned int  u32;
typedef float         f32;

// ---------------------------------------------------------------------------------------------
// Support types
// ---------------------------------------------------------------------------------------------
struct Vec3f {
    f32 x;                              // 0x00
    f32 y;                              // 0x04
    f32 z;                              // 0x08

    Vec3f() {}
    Vec3f(f32 ax, f32 ay, f32 az) : x(ax), y(ay), z(az) {}

    Vec3f Normalized() const;
};

// Unit-length copy of the vector; the epsilon keeps the reciprocal square root
// finite for a zero vector.  The components are seeded into the result and then
// scaled in place, so the scale is applied as a component assignment.
inline Vec3f Vec3f::Normalized() const
{
    f32 inv = 1.0f / sqrtf(x * x + y * y + z * z + 1e-8f);
    Vec3f r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.x *= inv;
    r.y *= inv;
    r.z *= inv;
    return r;
}

// Interface subobject embedded at +0x04 of the mode; the setter notifies it
// through vtable slot 36 (offset 0x90).
struct cModeSubObj {
    virtual void s00(); virtual void s01(); virtual void s02(); virtual void s03();
    virtual void s04(); virtual void s05(); virtual void s06(); virtual void s07();
    virtual void s08(); virtual void s09(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36();                // slot 36, offset 0x90
};

// Mode object: the two stored directions sit at 0x304 and 0x310, right before
// the fade half-angle at 0x350 and the dirty flags at 0x35c.
struct cFadeMode {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual void v35();                // slot 35, offset 0x8c

    cModeSubObj mSub;                   // 0x04
    u8  mPad0008[0x2fc];                // 0x08
    Vec3f mDirA;                        // 0x304
    Vec3f mDirB;                        // 0x310
    u8  mPad0318[0x38];                 // 0x318

    bool SetDirections(const Vec3f* a, const Vec3f* b);   // 0x00e0c490
};

// ---------------------------------------------------------------------------------------------
// Definition
// ---------------------------------------------------------------------------------------------
// @ 0x00e0c490
bool cFadeMode::SetDirections(const Vec3f* a, const Vec3f* b)
{
    mDirA = *a;
    mDirA = mDirA.Normalized();

    mDirB = *b;
    mDirB = mDirB.Normalized();

    mSub.s36();
    v35();
    return true;
}
