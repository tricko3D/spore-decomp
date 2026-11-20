// @ 0x00e5e850
// Flags: /vc71 /O2 /MD /Gy /TP /arch:SSE
#include "types.h"

struct Vec3_s00e5e850 { float x, y, z; };

// 3x3 matrix, 9 floats.  Copy construction is the out-of-line helper at 0x41cb40.
struct Matrix3_s00e5e850 {
    float m[9];
    Matrix3_s00e5e850() {}
    Matrix3_s00e5e850(const Matrix3_s00e5e850& src);   // 0x41cb40
};

struct Quat_s00e5e850 { float x, y, z, w; };

// Fills *out and returns it (retail copies the result straight from eax).
extern Matrix3_s00e5e850* Matrix3FromQuaternion(Matrix3_s00e5e850* out, const Quat_s00e5e850* q);  // 0x59c190

struct CellGame_s00e5e850 {
    char   pad_0000[0x4108];       // +0x0000
    char*  mpPoints;               // +0x4108
};

extern CellGame_s00e5e850* gspCellGame;   // 0x016b3c04

// Embedded body copied by value at the top of the function; the copy holds
// the pre-update transform.  mFlags sits at +0, the floats follow, then the
// matrix (copied through 0x41cb40).
struct Body_s00e5e850 {
    unsigned short    mFlags;    // +0x00  (self +0x48)
    float             mX;        // +0x04  (self +0x4c)
    float             mY;        // +0x08  (self +0x50)
    float             mZ;        // +0x0c  (self +0x54)
    float             mW;        // +0x10  (self +0x58)
    Matrix3_s00e5e850 mMatrix;   // +0x14  (self +0x5c)
};

struct Obj_s00e5e850 {
    char           pad_000[0x48];           // +0x000
    Body_s00e5e850 mBody;                   // +0x048
    char           pad_080[0x248 - 0x080];  // +0x080
    void*          mpPositions;             // +0x248
    char           pad_24c[0x250 - 0x24c];  // +0x24c
    int            mCount;                  // +0x250
    int            mIndices[1];             // +0x254
};

// Row-major matrix times column vector: result.x = row0 . v.  Retail sums each
// component starting from the z term (parens keep that rounding order).
__forceinline Vec3_s00e5e850 operator*(const Matrix3_s00e5e850& a, const Vec3_s00e5e850& v)
{
    Vec3_s00e5e850 r;
    r.x = (a.m[2] * v.z + a.m[1] * v.y) + a.m[0] * v.x;
    r.y = (a.m[5] * v.z + a.m[4] * v.y) + a.m[3] * v.x;
    r.z = (a.m[8] * v.z + a.m[7] * v.y) + a.m[6] * v.x;
    return r;
}

// @ 0x00e5e850
void __cdecl FUN_00e5e850(Obj_s00e5e850* self, Quat_s00e5e850* q)
{
    Body_s00e5e850 body = self->mBody;

    Matrix3_s00e5e850 rot;
    self->mBody.mMatrix = *Matrix3FromQuaternion(&rot, q);

    ((unsigned short*)&self->mBody)[1] += 1;    // +0x4a
    self->mBody.mFlags |= 2;

    if (self->mpPositions == 0 || self->mCount <= 1)
        return;

    float scale = body.mW;

    for (int i = 0; i < self->mCount; ++i) {
        float* v = (float*)(gspCellGame->mpPoints + 4 + self->mIndices[i] * 0xc);

        float x = v[0];
        float y = v[1];
        float z = v[2];
        x = x - body.mX;
        y = y - body.mY;
        z = z - body.mZ;
        v[0] = x;
        v[1] = y;
        v[2] = z;

        if (scale != 1.0f) {
            float inv = 1.0f / scale;
            x = x * inv;
            y = y * inv;
            z = z * inv;
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }

        if (body.mFlags & 2)
            *(Vec3_s00e5e850*)v = body.mMatrix * *(Vec3_s00e5e850*)v;

        if (self->mBody.mFlags & 2) {
            // Vector times matrix: each result component is a column of the
            // new orientation dotted with the position vector.  Parens keep
            // retail's per-component rounding order.
            float x = v[0], y = v[1], z = v[2];
            v[0] = (self->mBody.mMatrix.m[6] * z + self->mBody.mMatrix.m[3] * y) + self->mBody.mMatrix.m[0] * x;
            v[1] = (self->mBody.mMatrix.m[1] * x + self->mBody.mMatrix.m[7] * z) + self->mBody.mMatrix.m[4] * y;
            v[2] = (self->mBody.mMatrix.m[2] * x + self->mBody.mMatrix.m[8] * z) + self->mBody.mMatrix.m[5] * y;
        }

        float s = self->mBody.mW;
        v[0] = s * v[0];
        v[1] = s * v[1];
        v[2] = s * v[2];

        v[0] = self->mBody.mX + v[0];
        v[1] = self->mBody.mY + v[1];
        v[2] = self->mBody.mZ + v[2];
    }
}
