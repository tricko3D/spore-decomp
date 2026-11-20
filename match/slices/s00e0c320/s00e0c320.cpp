// Slice s00e0c320 - SporeApp 0x00e0c320..0x00e0c480
// Angular fade-window update of the same mode object whose SetFade lives at
// 0x00e0c2a0: both angular ranges advance by a global step, the azimuth width
// is smoothed into the fade half-angle (clamped to the configured limits) and,
// unless the width had to be clamped, a direction built from the two range
// centres is handed to the mode's direction sink.
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /GS- /fp:fast
#include "types.h"
#include <math.h>
#include <intrin.h>

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
};

// Interface subobject embedded at +0x04 of the mode; the update notifies it
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

// Mode object: two angular ranges (azimuth at 0x990/0x994, elevation at
// 0x998/0x99c) plus the fade half-angle and the dirty flags.
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
    u8  mPad0008[0x348];
    f32 mFade;                          // 0x350 fade half-angle
    u32 mPad354;                        // 0x354
    u32 mPad358;                        // 0x358
    u32 mFlags;                         // 0x35c
    u8  mPad360[0x630];                 // 0x360
    f32 mMinAzimuth;                    // 0x990
    f32 mMaxAzimuth;                    // 0x994
    f32 mMinElevation;                  // 0x998
    f32 mMaxElevation;                  // 0x99c

    void UpdateAngles(bool notify);     // 0x00e0c320
    void ApplyDirection(const Vec3f* dir);  // 0x00e0c1f0
};

// ---------------------------------------------------------------------------------------------
// Globals implemented in other slices
// ---------------------------------------------------------------------------------------------
extern float g_step;                    // 0x016a206c angular step per update
extern float g_widthLimitMin;           // 0x016a2064
extern float g_widthLimitMax;           // 0x016a2068
extern float g_halfPi;                  // 0x015a4bfc pi/2
extern const float g_two;               // 0x01470f1c 2.0f
extern const float g_half;              // 0x01471064 0.5f

// ---------------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------------
// @ 0x00e0c320
void cFadeMode::UpdateAngles(bool notify)
{
    f32 minAz = mMinAzimuth - g_step;
    f32 minEl = mMinElevation - g_step;
    f32 maxAz = mMaxAzimuth + g_step;
    f32 maxEl = mMaxElevation + g_step;

    mMaxElevation = maxEl;
    mMaxAzimuth = maxAz;

    f32 widthEl = maxEl - minEl;
    f32 widthAz = maxAz - minAz;

    mMinAzimuth = minAz;
    mMinElevation = minEl;

    // Smooth the azimuth width towards twice the elevation width, inside the
    // configured limits.
    f32 width = widthAz;
    if (widthEl * 2.0f > width)
        width = widthEl * 2.0f;
    if (g_widthLimitMin > width)
        width = g_widthLimitMin;

    // Recentre the azimuth range around its old centre with the new width.
    f32 unit = 0.5f;
    f32 centre = minAz + widthAz * unit;
    f32 half = width * unit;
    mMinAzimuth = centre - half;
    mMaxAzimuth = centre + half;

    // A width that had to be clipped keeps a fixed fade instead of the
    // direction built from the two range centres.
    bool inRange = true;
    if (width > g_widthLimitMax) {
        width = g_halfPi * unit;
        inRange = false;
    }

    f32 fade = width * unit;
    mFade = fade;

    // The direction is rebuilt from the stored centre, so read the recentred
    // minimum back from the object rather than a cached copy (retail does not
    // keep it in a stack slot).
    _ReadWriteBarrier();
    if (inRange) {
        f32 elevation = widthEl * unit + minEl;
        f32 azimuth = mMinAzimuth + fade;

        Vec3f dir;
        f32 cosEl = cosf(elevation);
        dir.x = sinf(azimuth) * cosEl;
        dir.y = cosf(azimuth) * cosEl;
        dir.z = sinf(elevation);
        ApplyDirection(&dir);
    }

    if (notify) {
        mSub.s36();
        v35();
        mFlags |= 3;
    }
}
