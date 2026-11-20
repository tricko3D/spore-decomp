// Slice s00e0c820 - SporeApp 0x00e0c820..0x00e0cba0
// Builds the spherical sin/cos lookup tables for the sky-ring sampler and then
// walks `rowCount` elevation rows of the ring, sampling a six-face 16-bit cube
// map into an 8-bit strip.
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /GS- /fp:fast
#include "types.h"
#include <math.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef float          f32;

// ---------------------------------------------------------------------------------------------
// Support types
// ---------------------------------------------------------------------------------------------
struct Vec3f {
    f32 x;                              // 0x00
    f32 y;                              // 0x04
    f32 z;                              // 0x08
};

// One mip level of the cubemap image: the faces are packed one after another,
// each `mSize` x `mSize` 16-bit texels.
struct cCubeMip {
    u8          mPad00[8];
    u32         mSize;                  // 0x08 face edge in texels
    u8          mPad0c[4];
    const u16*  mData;                  // 0x10 six faces, row major
};

// Image object reached through the source interface: the mip level lives at +0x08.
struct cCubeImage {
    u8          mPad00[8];
    cCubeMip*   mMip;                   // 0x08
};

// Interface returned by the global image source; the cubemap accessor is slot 3.
struct IImageSource {
    virtual void s00(); virtual void s01(); virtual void s02();
    virtual cCubeImage* s03();          // offset 0x0c
};

// ---------------------------------------------------------------------------------------------
// Globals implemented in other slices
// ---------------------------------------------------------------------------------------------
extern IImageSource* GetImageSource();  // 0x00f48aa0
extern float* g_trigTable;              // 0x015a4cbc sin(elev) | cos(elev) | sin(az) | cos(az)
extern float  g_halfPi;                 // 0x015a4bfc pi/2
// Float constants from the game's read-only data; they are objects, not macros, so the
// 2/256 and 1/128 step scales stay two separate multiplies (they are equal values).
extern const float kTwo;                // 0x01470f1c 2.0f
extern const float kInv256;             // 0x014650a8 1/256
extern const float kInv128;             // 0x0146150c 1/128

// ---------------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------------
// @ 0x00e0c820
// `pOwner` is the leading argument retail passes but never reads (the body
// works only through the globals below).
void BuildSkyRingStrip(void* pOwner, u8* out, int startRow, int rowCount,
                       const Vec3f* dir, f32 fov, bool force)
{
    static int     s_ready;             // 0x016a2094
    static float*  s_cosElev = g_trigTable + 128;    // 0x016a20a0
    static float*  s_sinAzim = s_cosElev + 128;      // 0x016a209c
    static float*  s_cosAzim = s_sinAzim + 256;      // 0x016a2098

    if (!s_ready || force) {
        f32 azStep = fov * 2.0f;
        f32 elStep = fov / 128.0f;
        azStep /= 256.0f;

        s_ready = 1;

        f32 y = dir->y;
        f32 x = dir->x;
        f32 z = dir->z;
        f32 azStart = g_halfPi - atan2f(y, x) - fov;
        f32 ang = atan2f(z, sqrtf(x * x + y * y)) - fov * 0.5f;

        // Elevation table: 128 rows centred on the view elevation.
        for (int i = 0; i < 128; ++i) {
            g_trigTable[i] = sinf(ang);
            s_cosElev[i] = cosf(ang);
            ang += elStep;
        }

        // Azimuth table: 256 columns spanning 2*fov around the view azimuth.
        ang = azStart;
        for (int i = 0; i < 256; ++i) {
            s_sinAzim[i] = sinf(ang);
            s_cosAzim[i] = cosf(ang);
            ang += azStep;
        }
    }

    u8* dst = out;
    cCubeMip* mip = GetImageSource()->s03()->mMip;
    u32 size = mip->mSize;
    const u16* texels = mip->mData;
    f32 fsize = (f32)size;

    int row = 127 - startRow;
    int last = row - rowCount;
    if (row > last) {
        for (; row > last; --row) {
            f32 se = g_trigTable[row];      // sin(elevation)
            f32 ce = s_cosElev[row];        // cos(elevation)
            for (int j = 255; j >= 0; --j) {
                f32 p1 = ce * s_sinAzim[j];
                f32 p2 = ce * s_cosAzim[j];

                f32 ax = fabsf(se);
                f32 ay = fabsf(p1);
                f32 az = fabsf(p2);

                f32 u, v;
                int face;
                if (ax >= ay && ax >= az) {
                    u = (p1 / se + 1.0f) * 0.5f;
                    v = (p2 / ax + 1.0f) * 0.5f;
                    face = (se >= 0.0f) ? 0 : 1;
                } else if (az >= ay) {
                    u = (se / p2 + 1.0f) * 0.5f;
                    v = (p1 / az + 1.0f) * 0.5f;
                    face = (p2 >= 0.0f) ? 4 : 5;
                } else {
                    u = (p2 / p1 + 1.0f) * 0.5f;
                    v = (se / ay + 1.0f) * 0.5f;
                    face = (p1 >= 0.0f) ? 2 : 3;
                }

                int ui = (int)(u * fsize);
                int vi = (int)(v * fsize);
                *dst++ = (u8)(texels[(face * size + vi) * size + ui] >> 8);
            }
        }
    }
}
