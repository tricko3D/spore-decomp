// @ 0x00b81780
// Flags: /O2 /MD /Gy /TP /arch:SSE /GS- /fp:fast
// SP::cPlanetModel::MakeRandomWorldPosition
//   A random point on the surface around `dir`: the same construction as
//   FUN_00b81a40 (0x00b81a40), but the step along the local surface frame is
//   randomised -- a distance drawn from [minDist, maxDist] (or minDist itself
//   when the range is empty) at an angle drawn from [0, kSpiralMaxAngle].
//   maxDist <= 0 means "no offset": the invalid-position sentinel is used as
//   the offset.  A direction shorter than one unit is replaced by a random
//   unit direction, as in FUN_00b81a40.  Both branches leave the surface
//   direction in `p` and the surface radius in `scale`, which one shared tail
//   scales into `out`.
#include "game/types.h"
#include "cSPVector3.h"

#include <math.h>

extern const float kVectorEpsilon;

namespace SP {

struct Quat {
  float x, y, z, w;
};

struct Vec3 {
  float x, y, z;
};

// The terrain sphere carries the local surface frame; 0x0156b614 supplies the
// fixed reference orientation when no sphere is attached to the planet model.
struct cTerrainSphere {
  // retail returns cSPQuaternion by value; the hidden return pointer is spelled
  // out here so the call site keeps that shape.
  void BuildSurfaceOrientation(Quat * out, cSPVector3 const & dir,
                               Quat const & ref) const;  // 0x00f9c960
};

void RandomDirection3(float * out);                                             // 0x00b7e560
// Returns out, which the sphere call below passes straight through.
float * QuaternionFromDirections(float * out, const float * from, const float * to);  // 0x00698180
void Matrix3FromQuaternion(float * out, const float * q);                       // 0x0059c190
float RandomFloatRange(float lo, float hi);                                     // 0x00572a10
extern const float kOne;                                                        // 0x01485720 (1.0f)
extern const float kRefDirection[3];                                            // 0x01688278
extern const Quat kDefaultOrientation;                                          // 0x0156b614
extern const float kSpiralMaxAngle;                                             // 0x01688274
// The sentinel the rest of the planet code returns when a position is invalid
// (DAT_016881f0/f4/f8).
extern const Vec3 kFallback;                                                    // 0x016881f0

class cPlanetModel {
 public:
  char pad0[0x20];
  cTerrainSphere * mpSphere;  // +0x20
  int mpISphere;              // +0x24

  float GetRadiusAt(cSPVector3 const &) const;
  float GetRadiusAt(cSPVector3 const * dir) const;
  cSPVector3 * DirectionToSurfacePosition(cSPVector3 *, cSPVector3 const *) const;
  int IsPositionBelowSurface(cSPVector3 const * dir) const;
  void FUN_00b81a40(cSPVector3 * out, cSPVector3 const * dir, float a, float b);
  void MakeRandomWorldPosition(cSPVector3 * out, cSPVector3 const * dir,
                               float minDist, float maxDist);
};

void cPlanetModel::MakeRandomWorldPosition(cSPVector3 * out,
                                           cSPVector3 const * dir,
                                           float minDist, float maxDist) {
  float p[3];
  Quat q1;
  Quat q2;
  float m[9];
  float scale;

  if (kOne >= dir->x * dir->x + dir->y * dir->y + dir->z * dir->z) {
    // Not a usable direction: start from a random unit direction instead.
    // The two branches never overlap, so retail builds that direction in the
    // quaternion scratch slot q1 (the copies into p are forwarded away).
    RandomDirection3(&q1.x);
    scale = GetRadiusAt((cSPVector3 const *)&q1);
    p[0] = q1.x;
    p[1] = q1.y;
    p[2] = q1.z;
  } else {
    if (maxDist <= 0.0f) {
      // No offset requested: the surface point is the invalid-position sentinel.
      *(Vec3 *)p = kFallback;
    } else {
      // Step length: a random value in the requested range, or the single value
      // the range collapses to.  Retail keeps it in the maxDist argument slot.
      if (maxDist > minDist)
        maxDist = RandomFloatRange(minDist, maxDist);
      else
        maxDist = minDist;
      float angle = RandomFloatRange(0.0f, kSpiralMaxAngle);
      p[0] = maxDist * cosf(angle);
      p[1] = maxDist * sinf(angle);
      p[2] = 0.0f;
    }

    float * q1p = QuaternionFromDirections(&q1.x, kRefDirection, (const float *)dir);
    if (mpISphere != 0) {
      mpSphere->BuildSurfaceOrientation(&q2, *dir, *(Quat const *)q1p);
    } else {
      q2.x = kDefaultOrientation.x;
      q2.y = kDefaultOrientation.y;
      q2.z = kDefaultOrientation.z;
      q2.w = kDefaultOrientation.w;
    }

    Matrix3FromQuaternion(m, &q2.x);
    // The frame basis is applied as three columns; every component reads the
    // whole offset before any of it is overwritten.
    Vec3 v;
    v.x = m[3] * p[1] + m[6] * p[2] + m[0] * p[0];
    v.y = m[4] * p[1] + m[7] * p[2] + m[1] * p[0];
    v.z = m[5] * p[1] + m[8] * p[2] + m[2] * p[0];
    p[0] = dir->x + v.x;
    p[1] = dir->y + v.y;
    p[2] = dir->z + v.z;

    scale = GetRadiusAt((cSPVector3 const *)p);
    float inv = 1.0f / sqrtf(p[1] * p[1] + p[2] * p[2] + p[0] * p[0]);
    p[1] *= inv;
    p[2] *= inv;
    p[0] *= inv;
  }

  out->z = p[2] * scale;
  out->y = p[1] * scale;
  out->x = p[0] * scale;
}

}  // namespace SP
