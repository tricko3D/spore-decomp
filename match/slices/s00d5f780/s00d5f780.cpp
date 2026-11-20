// match/slices/s00d5f780/s00d5f780.cpp
// SP::cCityVisualizer::AddAnimal (0x00d5f780, 988 bytes, ret 4).
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS- /Isrc/game/include
//   /GS- is required: retail has no __security_cookie; with the default /GS the
//   prologue grows by the cookie and the frame is 0xac instead of 0x90.
//
// The argument is the animal noun: lounge-group pointer at +0xb4c (whose +0x5fc
// flag word is set/tested here), transform sub-object at +0xc0, and the
// 0xc0b780 / 0xc0b7a0 classifier pair.  `this` keeps the retained-noun vector at
// +0xadc, a byte at +0xa50 and the path provider at +0xaf0, whose vtable slot
// +0x6c hands out the 6-point path object (0xbec2d0 / 0xbec1c0).  Sequence:
// retain the noun in the +0xadc list, mark it as a path user (0x01000000 and the
// +0x192 latch of the 0xc04590 result), then either defer through 0xd5f4d0
// (message 0x0cf4034d / 0x4f8e9bb0) while the +0xa50 byte is clear, or scan the
// 6 path points, orient towards the nearest one (0xb3d350 / sqrtf), push a
// 0x3c-byte path entry into the transform's list (0xc41ec0) and hand the
// position and speed to 0xd590b0.

#include "game/types.h"
#include <new>
#include <math.h>

// ---- float / data globals read from absolute addresses ----
extern float g_169ec28;                                  // 0x169ec28
extern float g_169ec2c;                                  // 0x169ec2c
extern float g_169ec30;                                  // 0x169ec30
extern float g_147b9a0;                                  // 0x147b9a0 (initial best distance)
extern float g_1470f1c;                                  // 0x1470f1c
extern float g_13ec4b8;                                  // 0x13ec4b8 (length epsilon)
extern float g_1473c70;                                  // 0x1473c70
extern float g_1485720;                                  // 0x1485720
extern float g_13f6b68;                                  // 0x13f6b68 (speed)
extern char  g_00b4c7c0;                                 // 0xb4c7c0

// ---- helper types / vtable-slot call shapes ----
struct Vec3 { float x, y, z; };

typedef void  (__thiscall **FnVoid)(void*);              // call [vt + 0]
typedef void* (__thiscall **FnPtr)(void*);

struct Sim {
  char raw[0x192];
  char mByte_192;                                        // +0x192
};

struct Group {
  char raw[0x5fc];
  unsigned mFlags_5fc;                                   // +0x5fc
};

struct Scalar9 { int w0, w1, w2, w3, w4, w5, w6, w7, w8; };  // 0x24 bytes, no array

struct PathEntry {
  Vec3    mPos;                                          // +0x00
  float   mValue;                                        // +0x0c
  int     mFlags;                                        // +0x10
  Scalar9 mUnk_014;                                      // +0x14
  char    mByte_038;                                     // +0x38
  char    mPad0, mPad1, mPad2;                           // +0x39

  PathEntry() {}
  PathEntry(PathEntry const&);                           // 0xac1ff0
};

// pointer vector with the inline fast path / DoInsertPtr slow-path split retail
// uses for these handles (element 0x3c bytes: copy-construct 0xac1ff0 /
// insert-slow 0xac45e0).
template <class T> struct ObjVector {
  T* mBegin;
  T* mEnd;
  T* mCap;
  void DoInsertPtr(T* pos, T const& v);                  // 0xac45e0
  void push_back(T const& v) {
    if (mEnd < mCap) {
      T* e = mEnd;
      mEnd = e + 1;
      if (e)
        new (e) T(v);                                    // copy-construct 0xac1ff0
    } else {
      DoInsertPtr(mEnd, v);
    }
  }
};

// AutoRefCount-style retain/release temp used by the +0xadc list.
template <class T> struct AutoRefCount {
  T* p;
  explicit AutoRefCount(T* q) {
    p = q;
    if (q)
      (*(FnVoid)(*(char**)q + 0))(q);                    // AddRef: call [vt + 0]
  }
  AutoRefCount(const AutoRefCount& o) {
    p = o.p;
    if (p)
      (*(FnVoid)(*(char**)p + 0))(p);
  }
  ~AutoRefCount() {
    if (p)
      (*(FnVoid)(*(char**)p + 4))(p);                    // Release: call [vt + 4]
  }
};

// retain list at this+0xadc (begin/end/cap, grow via 0xaea5d0).
template <class T> struct HandleVector {
  T* mBegin;
  T* mEnd;
  T* mCap;
  void DoInsertPtr(T* pos, T const& v);                  // 0xaea5d0
  void push_back(T const& v) {
    if (mEnd < mCap) {
      T* e = mEnd;
      mEnd = e + 1;
      if (e)
        new (e) T(v);
    } else {
      DoInsertPtr(mEnd, v);
    }
  }
};

struct Transform {
  char raw[0x100];
  void  FUN_00c43fe0(void* key);                         // 0xc43fe0
  ObjVector<PathEntry>* FUN_00c41ec0();                  // 0xc41ec0
};

struct PathObject {
  char raw[0x100];
  void  GetPoint(Vec3* out, int index);                  // 0xbec2d0
  Vec3* GetPointB(Vec3* out, int index);                 // 0xbec1c0
};

struct Creature {
  char raw_000[0xc0];
  Transform mTransform_0c0;                              // +0xc0
  char raw_1c0[0xb4c - 0x1c0];
  Group* mGroup_0b4c;                                    // +0xb4c

  Sim*  FUN_00c04590();                                  // 0xc04590
  bool  FUN_00c0b780();                                  // 0xc0b780
  bool  FUN_00c0b7a0();                                  // 0xc0b7a0
  void  FUN_00c1c1d0(int mode, Vec3* pos, float s0, float s1);  // 0xc1c1d0
  void  FUN_00c12310(void* key, int a, int b);           // 0xc12310
};

// 0xb3d350 returns the global planet model; its 0xb815a0 member projects a
// point onto the surface in place and hands the same vector back.
struct cPlanetModel {
  Vec3* DirectionToSurfacePosition(Vec3* out, Vec3 const* in);   // 0xb815a0
};
cPlanetModel* PlanetModel();                             // 0xb3d350

namespace SP {

struct cCityVisualizer {
  char raw_000[0xa50];
  char mByte_a50;                                        // +0xa50
  char raw_a51[0xadc - 0xa51];
  HandleVector<AutoRefCount<Creature> > mAnimals_adc;    // +0xadc
  char raw_ae8[0xaf0 - 0xae8];
  void* mPathProvider_af0;                               // +0xaf0

  void FUN_00d5f4d0(unsigned message, Creature* c);      // 0xd5f4d0
  void FUN_00d590b0(int mode, Vec3* pos, float speed);   // 0xd590b0
  void AddAnimal(Creature* pAnimal);
};

} // namespace SP

// @ 0x00d5f780
void SP::cCityVisualizer::AddAnimal(Creature* pAnimal)
{
  {
    AutoRefCount<Creature> ref(pAnimal);
    mAnimals_adc.push_back(ref);
  }

  pAnimal->mGroup_0b4c->mFlags_5fc |= 0x01000000u;

  Transform* xf = &pAnimal->mTransform_0c0;
  xf->FUN_00c43fe0(&g_00b4c7c0);

  if (pAnimal->mGroup_0b4c->mFlags_5fc & 0x00800000u) {
    if (pAnimal->FUN_00c04590())                 // retail calls it twice
      pAnimal->FUN_00c04590()->mByte_192 = 1;
  }

  if (!pAnimal->FUN_00c0b7a0())
    goto tail;

  if (mByte_a50 == 0) {
    if (pAnimal->FUN_00c0b780())
      FUN_00d5f4d0(0x0cf4034du, pAnimal);
    else
      FUN_00d5f4d0(0x4f8e9bb0u, pAnimal);
    goto tail;
  }

  {
    PathObject* path = (PathObject*)(*(FnPtr)(*(char**)mPathProvider_af0 + 0x6c))(mPathProvider_af0);
    Vec3* from     = (Vec3*)(*(FnPtr)(*(char**)xf + 0x2c))(xf);
    Vec3  pos;                                                      // +0x14
    Vec3  bestNear;                                                 // +0x24
    Vec3  bestFar;                                                  // +0x30
    Vec3  point;                                                    // +0x3c
    Vec3  rotOut;                                                   // +0x58
    float bestDist;                                                 // +0x10
    pos.x = from->x;
    pos.y = from->y;
    pos.z = from->z;
    bestFar.x = g_169ec28;
    bestFar.y = g_169ec2c;
    bestFar.z = g_169ec30;
    bestNear.x = g_169ec28;
    bestNear.y = g_169ec2c;
    bestNear.z = g_169ec30;
    bestDist = g_147b9a0;

    for (int i = 0; i < 6; ++i) {
      path->GetPoint(&point, i);
      float dx = pos.x - point.x;
      float dy = pos.y - point.y;
      float dz = pos.z - point.z;
      float dist = sqrtf(dx * dx + dy * dy + dz * dz);
      if (dist < bestDist) {
        bestDist = dist;
        bestFar  = point;
        bestNear = *path->GetPointB(&rotOut, i);
      }
    }

    pAnimal->FUN_00c1c1d0(3, PlanetModel()->DirectionToSurfacePosition(&rotOut, &bestNear),
                          1.0f, g_1470f1c);

    // the heading is built in place: the delta replaces the position and is then
    // scaled outward from the far point
    pos.x = bestFar.x - bestNear.x;
    pos.y = bestFar.y - bestNear.y;
    pos.z = bestFar.z - bestNear.z;
    float inv = 1.0f / sqrtf(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z + g_13ec4b8);
    float k = g_1473c70;
    float sx = pos.x * inv;
    float sy = pos.y * inv;
    float sz = pos.z * inv;
    pos.x = bestFar.x + sx * k;
    pos.y = bestFar.y + sy * k;
    pos.z = bestFar.z + sz * k;

    Vec3 posOut;                                                    // +0x4c
    PlanetModel()->DirectionToSurfacePosition(&posOut, &pos);

    PathEntry entry;                                                // +0x64
    entry.mPos.x    = posOut.x;
    entry.mPos.y    = posOut.y;
    entry.mPos.z    = posOut.z;
    entry.mValue    = g_1485720;
    entry.mFlags    = 0;
    entry.mByte_038 = 0;

    xf->FUN_00c41ec0()->push_back(entry);

    pAnimal->FUN_00c12310((void*)0x02481db0u, -1, 0);
  }

tail:
  int mode = 0;
  if (pAnimal->FUN_00c0b780() && !pAnimal->FUN_00c0b7a0())
    mode = 2;

  FUN_00d590b0(mode, (Vec3*)(*(FnPtr)(*(char**)xf + 0x2c))(xf), g_13f6b68);
}
