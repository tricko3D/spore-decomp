// Slice s00e5e2c0: SP::sCellBucketEffect (SPCellGame.obj).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast   (SPCellGame.obj: SSE floats, no /EHsc)
//
// Walks the bucket pool kept at cCellGame + 0x38.  Every entry whose type is
// 1 and whose level matches `level` gets a visual effect: a zero-weight
// parameter tears the effect down, anything else creates it (if the entry has
// none yet) and drives it with the cell box that the entry's grid coordinates
// describe.
#include "types.h"

struct Vector3 { float x, y, z; };

struct M33 {
  float m[9];
  void Assign(const void* src);   // 0x41cb40
};

// Effect object returned through SP::CreateEffectSafe.  Only the virtual slots
// this function actually calls are named; the filler slots keep the vtable
// offsets (+0x00 ... +0x44) correct.
struct IRefObj {
  virtual int  AddRef();                                            // +0x00
  virtual void Release();                                           // +0x04
  virtual void SetVisible(int enabled);                             // +0x08
  virtual void Stop(int mode);                                      // +0x0c
  virtual void v10();                                               // +0x10
  virtual void v14();                                               // +0x14
  virtual void SetCellXform(const void* pCell);                     // +0x18
  virtual void v1c();                                               // +0x1c
  virtual void v20();                                               // +0x20
  virtual void v24();                                               // +0x24
  virtual void v28();                                               // +0x28
  virtual void v2c();                                               // +0x2c
  virtual void v30();                                               // +0x30
  virtual void v34();                                               // +0x34
  virtual void v38();                                               // +0x38
  virtual void v3c();                                               // +0x3c
  virtual void v40();                                               // +0x40
  virtual void SetParam(int mode, const float* pValue, int flags);  // +0x44
};

// Cell transform handed to the effect: the same prefix cCell uses (flags,
// counter, position, scale, orientation).
struct CellXform {
  unsigned short mFlags;     // +0x00
  unsigned short mCounter;   // +0x02
  float          mPosX;      // +0x04
  float          mPosY;      // +0x08
  float          mPosZ;      // +0x0c
  float          mScale;     // +0x10
  M33            mOrient;    // +0x14
};

// Element of the bucket pool (0x00e31100 / 0x00b72230).
struct CellEntry {
  char      mReserved00[4];
  int       mType;                          // +0x04  1 == placeable cell
  int       mLevel;                         // +0x08
  int       mCellX;                         // +0x0c
  int       mCellY;                         // +0x10
  char      mReserved14[0x300 - 0x14];
  IRefObj*  mpEffect;                       // +0x300
};

struct Pool {
  void* Next(int* cursor);   // 0x00b72230, ret 4
  int   Begin();             // 0x00e31100
};

struct cCellGame {
  char   mReserved00[0x514c];
  float  mLevelScale;        // +0x514c
};

struct cCellGfx {
  char   mReserved00[0x161bc];
  void*  mpEffectsWorld;     // +0x161bc
};

extern cCellGame*   gspCellGame;        // 0x016b3c04
extern cCellGfx*    gspCellGfx;         // 0x016b3c08
extern float        kLevelSize[];       // 0x01483bd0, one cell size per level
extern const float  kBoxScale;          // 0x013f6a20
extern const float  kHalf;              // 0x01471064
extern const float  kOne;               // 0x01485720
extern const float  kCellZ;             // 0x016b3c30
extern const float  kCellM0;            // 0x016b3c28
extern const float  kCellM1;            // 0x016b3c2c
extern char         gMatrix_16b3dac[];  // 0x016b3dac, identity rotation

void CreateEffectSafe(void* world, unsigned int effectId, int flags,
                      IRefObj** out);   // 0x00628450

// 0x016b3c04 + 0x38.  Re-read the global at every use (the pool accessors are
// calls, so the compiler must reload it), as the retail code does.
#define BUCKET_POOL (*(Pool*)((char*)gspCellGame + 0x38))

namespace SP {
// 0x00e51ff0, float in st0.  Its body is visible in the original translation unit (SPCellGame.cpp), so cl's
// optimizer knows it neither writes an xmm register nor leaves an x87 value behind; that is why the retail
// code can keep the four cell-box corners live in xmm1..xmm5 across this call.  A plain declaration makes the
// compiler spill them, which changes the frame layout and the float domain of the scale below.  The stand-in
// body below only supplies those clobber facts (the real body lives in slice s00e51820); __declspec(noinline)
// keeps the call that the original makes.
__declspec(noinline) float __cdecl sGetBoxSizeMultiplier2(int level, int unused);
}

namespace SP {

// @ 0x00e5e2c0
void __cdecl sCellBucketEffect(const int* pEffectId, float weight, int level) {
  int cursor = BUCKET_POOL.Begin();
  for (CellEntry* e = (CellEntry*)BUCKET_POOL.Next(&cursor); e != 0;
       e = (CellEntry*)BUCKET_POOL.Next(&cursor)) {
    if (e->mType != 1 || e->mLevel != level)
      continue;

    IRefObj** ppEffect = &e->mpEffect;

    if (weight == 0.0f) {
      IRefObj* pEffect = *ppEffect;
      if (pEffect == 0)
        continue;
      pEffect->Stop(1);
      pEffect = *ppEffect;
      if (pEffect == 0)
        continue;
      *ppEffect = 0;
      pEffect->Release();
      continue;
    }

    // Cell box in world units: one cell of the level grid, flat in Z.
    const float cellSize = kLevelSize[e->mLevel];
    const float zOffset = (float)e->mLevel * 0.0f;
    const float x0 = (float)e->mCellX * cellSize + zOffset;
    const float y0 = (float)e->mCellY * cellSize + zOffset;
    const float x1 = (float)(e->mCellX + 1) * cellSize - zOffset;
    const float y1 = (float)(e->mCellY + 1) * cellSize - zOffset;

    // volatile: the original rounds the product into a stack slot (fstp) before the two
    // reciprocal divisions read it back.
    volatile float boxSize =
        SP::sGetBoxSizeMultiplier2(e->mLevel, e->mType) * kBoxScale;
    const float inv = kOne / (gspCellGame->mLevelScale / boxSize);

    if (*ppEffect == 0) {
      const Vector3 low  = { inv * x0, inv * y0, inv * 0.0f };
      const Vector3 high = { inv * x1, inv * y1, inv * 0.0f };

      // Reset the slot through the smart-pointer idiom, then create.
      IRefObj* pStale = *ppEffect;
      if (pStale != 0) {
        *ppEffect = 0;
        pStale->Release();
      }
      CreateEffectSafe(gspCellGfx->mpEffectsWorld, (unsigned int)*pEffectId,
                       0, ppEffect);

      const float midX = (low.x + high.x) * kHalf;
      const float midY = (low.y + high.y) * kHalf;
      const float midZ = (low.z + high.z) * kHalf;

      CellXform cell;
      cell.mCounter = 0;
      cell.mOrient.m[0] = kCellM0;
      cell.mOrient.m[1] = kCellM1;
      cell.mPosZ = kCellZ;
      cell.mScale = kOne;
      cell.mFlags = 0;
      cell.mOrient.Assign(gMatrix_16b3dac);
      cell.mPosX = midX;
      cell.mPosY = midY;
      cell.mPosZ = midZ;
      cell.mFlags |= 4;
      cell.mCounter += 2;
      cell.mScale = high.x - low.x;

      (*ppEffect)->SetCellXform(&cell);
      (*ppEffect)->SetVisible(0);
    }

    (*ppEffect)->SetParam(4, &weight, 1);
  }
}

}  // namespace SP

// Stand-in for 0x00e51ff0 (the real body is matched in slice s00e51820).  It exists so the compiler can see
// that the callee clobbers no xmm register and returns in st0 -- the fact the retail build relied on, because
// the original SPCellGame.cpp defined it in the same translation unit.  Never called from anywhere else.
namespace SP {
__declspec(noinline) float __cdecl sGetBoxSizeMultiplier2(int level, int unused) {
  if (level < 2)
    return 1.0f;
  if (level >= 5)
    return 0.03125f;
  switch (level) {
    case 2:
      return 0.5f;
    case 3:
      return 0.25f;
    case 4:
      return 0.125f;
  }
  return 1.0f;
}
}  // namespace SP
