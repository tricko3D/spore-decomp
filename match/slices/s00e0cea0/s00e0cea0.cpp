// Slice s00e0cea0 - SporeApp 0x00e0cea0..?
// Minimap helper: expands a 2D minimap coordinate into a 3D direction on the
// planet sphere. The fast path rotates the reference direction by two
// axis/angle rotations; any other game mode falls back to the generic
// spherical projection.
// Flags: /O2 /Ob1 /MD /Gy /EHsc /TP /arch:SSE /GS- /fp:fast
#include "types.h"

typedef unsigned int u32;

struct Vec2 {
    float x;                            // 0x00
    float y;                            // 0x04
};

struct Vec3 {
    float x;                            // 0x00
    float y;                            // 0x04
    float z;                            // 0x08
};

// 3x3 matrix, stored as three consecutive rows. The out-of-line copy
// assignment at 0x0041cb40 is what the two rotations are stored through.
struct Matrix3 {
    float m[9];                         // 0x00

    Matrix3& operator=(const Matrix3& o);   // 0x0041cb40
};

// ---------------------------------------------------------------------------------------------
// External helpers implemented in other slices.
// ---------------------------------------------------------------------------------------------
u32 __cdecl SP_GetCurrentGameMode();                                        // 0x00b5b800
void __cdecl FUN_00e0b2c0(float* out, void* unused, const float* uv,
                          const Vec3* dir, float w);                        // 0x00e0b2c0
Matrix3* __cdecl MatrixFromAxisAngle(Matrix3* out, const Vec3* axis,
                                     float angle);                          // 0x00453b20

// ---------------------------------------------------------------------------------------------
// @ 0x00e0cea0
// ---------------------------------------------------------------------------------------------
extern "C" Vec3* __cdecl ExpandCoord(Vec3* pOut, void* unused, const Vec2* pUV,
                                     const Vec3* pDir, const Vec3* pUp, float w)
{
    u32 mode = SP_GetCurrentGameMode();
    if (mode != 0x1654c05u && mode != 0x1654c04u) {
        FUN_00e0b2c0((float*)pOut, unused, (const float*)pUV, pDir, w);
        return pOut;
    }

    // Longitude of the minimap column. The original evaluates this one
    // expression in x87 extended precision (2*(u*w) - w).
    float lon = (float)((long double)pUV->x * w * 2.0L - w);

    Vec3 axis;
    axis.x = pDir->y * pUp->z - pDir->z * pUp->y;
    axis.y = pUp->x * pDir->z - pDir->x * pUp->z;
    axis.z = pDir->x * pUp->y - pDir->y * pUp->x;

    // Tilt the reference direction by the minimap row, then spin it by the
    // longitude. The scale is distributed over the row term, as the original
    // does: -(row*w - half*w).
    Matrix3 t;
    Matrix3 m;
    m = *MatrixFromAxisAngle(&t, &axis, -(pUV->y * w - 0.5f * w));

    Vec3 v;
    v.x = m.m[0] * pDir->x + m.m[3] * pDir->y + m.m[6] * pDir->z;
    v.y = m.m[1] * pDir->x + m.m[4] * pDir->y + m.m[7] * pDir->z;
    v.z = m.m[2] * pDir->x + m.m[5] * pDir->y + m.m[8] * pDir->z;

    m = *MatrixFromAxisAngle(&t, pUp, lon);

    pOut->x = m.m[0] * v.x + m.m[3] * v.y + m.m[6] * v.z;
    pOut->y = m.m[1] * v.x + m.m[4] * v.y + m.m[7] * v.z;
    pOut->z = m.m[2] * v.x + m.m[5] * v.y + m.m[8] * v.z;
    return pOut;
}
