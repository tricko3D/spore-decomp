// Slice s00e5e590: SP::sCellSetPosition (SPCellGame.obj).
// Retail module: /O2 /MD /Gy /TP /arch=SSE /fp=fast, no /EHsc.
#include "types.h"
#include <intrin.h>

struct Vector3 { float x, y, z; };

// 3x3 matrix, 9 floats, row-major.  Copy construction is the out-of-line
// helper at 0x41cb40.
struct Matrix3 {
  float m[9];
  Matrix3() {}
  Matrix3(const Matrix3& src);   // 0x41cb40
};

// The transform embedded at cCell + 0x48.  The modification counter at
// +0x02 is deliberately not a member of this view (retail does not copy it):
// the two bytes after mFlags are the natural alignment hole before mX.
struct Body {
  unsigned short mFlags;    // +0x00 (cell + 0x48)
  float          mX;        // +0x04 (cell + 0x4c)
  float          mY;        // +0x08 (cell + 0x50)
  float          mZ;        // +0x0c (cell + 0x54)
  float          mW;        // +0x10 (cell + 0x58) scale
  Matrix3        mMatrix;   // +0x14 (cell + 0x5c)
};

// +0x4108 holds the base of the per-cell vertex array; each element is
// addressed as base + index * 12 + 4 and carries three floats.  The base is
// reached through a char* so that the field is re-read every iteration.
struct cCellGame {
  char   mReserved00[0x4108];
  char*  mpPoints;                // +0x4108
};

struct cCell {
  char               mReserved00[0x48];
  Body               mBody;         // +0x048
  char               mReserved80[0x248 - 0x80];
  void*              mpVertexData;  // +0x248
  char               mReserved24c[4];
  int                mVertexCount;  // +0x250
  int                mIndices[1];   // +0x254
};

extern cCellGame* gspCellGame;    // 0x016b3c04
extern const float gKOne;         // 0x01485720  1.0f

namespace SP {

// @ 0x00e5e590
void __cdecl sCellSetPosition(cCell* cell, const Vector3* pos) {
  // The transform as it was before this update; the copy is what the vertex
  // remap below is computed against.
  Body body = cell->mBody;

  *(Vector3*)&cell->mBody.mX = *pos;
  cell->mBody.mFlags |= 4;
  ++((unsigned short*)&cell->mBody)[1];   // modification counter at cell+0x4a

  if (cell->mpVertexData == 0 || cell->mVertexCount <= 1)
    return;

  const float          scale = body.mW;
  const unsigned short flags = body.mFlags;
  const float          one = 1.0f;

  for (int i = 0; i < cell->mVertexCount; ++i) {
    // The base pointer is re-read every iteration; retail does not hoist it
    // out of the loop (the stores through v may change it).
    _ReadWriteBarrier();
    float* v = (float*)(gspCellGame->mpPoints + 4 + cell->mIndices[i] * 0xc);

    const float dx = v[0] - body.mX;
    const float dy = v[1] - body.mY;
    const float dz = v[2] - body.mZ;
    v[0] = dx;
    v[1] = dy;
    v[2] = dz;

    if (scale != one) {
      const float inv = one / scale;
      v[0] = dx * inv;
      v[1] = dy * inv;
      v[2] = dz * inv;
    }

    if (flags & 2) {
      const float* m = body.mMatrix.m;
      const float vx = v[0];
      const float vy = v[1];
      const float vz = v[2];
      Vector3 r;
      r.x = vz * m[2] + vy * m[1] + vx * m[0];
      r.y = vz * m[5] + vy * m[4] + vx * m[3];
      r.z = vz * m[8] + vy * m[7] + vx * m[6];
      *(Vector3*)v = r;
    }

    if (cell->mBody.mFlags & 2) {
      const float* m = cell->mBody.mMatrix.m;
      const float vx = v[0];
      const float vy = v[1];
      const float vz = v[2];
      const float rx = vz * m[6] + vy * m[3] + vx * m[0];
      const float ry = vz * m[7] + vy * m[4] + vx * m[1];
      const float rz = vz * m[8] + vy * m[5] + vx * m[2];
      v[0] = rx;
      v[1] = ry;
      v[2] = rz;
    }

    const float sc = cell->mBody.mW;
    v[0] = v[0] * sc + cell->mBody.mX;
    v[1] = v[1] * sc + cell->mBody.mY;
    v[2] = v[2] * sc + cell->mBody.mZ;
  }
}

}  // namespace SP
