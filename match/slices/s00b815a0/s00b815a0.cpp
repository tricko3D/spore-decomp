// @ 0x00b815a0
// SP::cPlanetModel::DirectionToSurfacePosition
//   retail: ?DirectionToSurfacePosition@cPlanetModel@@QBEPAUcSPVector3@@PAU3@PBU3@@Z
//   (the generated cPlanetModel.h still carries the dev-PDB by-value overload, so the
//    retail signature is declared here rather than by editing the shared header)
#include "game/types.h"
#include "cSPVector3.h"

#include <math.h>

extern const float kVectorEpsilon;

namespace SP {

struct Quat {
  float x, y, z, w;
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
Quat *QuaternionFromDirections(Quat * out, const float * from, const float * to);  // 0x00698180
void Matrix3FromQuaternion(float * out, const float * q);                       // 0x0059c190
extern const float kOne;                                                        // 0x01485720 (1.0f)
extern const float kRefDirection[3];                                            // 0x01688278
extern const Quat kDefaultOrientation;                                          // 0x0156b614

class cPlanetModel {
 public:
  char pad0[0x20];
  cTerrainSphere * mpSphere;  // +0x20
  int mpISphere;              // +0x24

  float GetRadiusAt(cSPVector3 const &) const;     // 0x00b7ef70
  float GetRadiusAt(cSPVector3 const * dir) const;  // 0x00b7ef70
  cSPVector3 * DirectionToSurfacePosition(cSPVector3 *, cSPVector3 const *) const;
  int IsPositionBelowSurface(cSPVector3 const * dir) const;
  void FUN_00b81a40(cSPVector3 * out, cSPVector3 const * dir, float a, float b);
};

// Normalise dir, then push it out to the surface radius.  The normalised direction
// is kept in a local three-float array (retail keeps the three products in
// xmm2/xmm0/xmm1 and the radius in xmm3 while the sum is built on the x87 stack).
cSPVector3 * cPlanetModel::DirectionToSurfacePosition(cSPVector3 * out,
                                                      cSPVector3 const * dir) const {
  float radius = GetRadiusAt(*dir);
  float inv = 1.0f / sqrtf(dir->x * dir->x + dir->y * dir->y + dir->z * dir->z +
                            kVectorEpsilon);
  float v[3];
  v[0] = dir->x * inv;
  v[1] = dir->y * inv;
  v[2] = dir->z * inv;
  out->x = v[0] * radius;
  out->y = v[1] * radius;
  out->z = v[2] * radius;
  return out;
}

// @ 0x00b81f90
// SP::cPlanetModel::IsPositionBelowSurface
//   Scale the query direction out to the surface radius and compare the squared
//   lengths: below the surface exactly when that scaled point is farther out.
//   Retail builds the sqrt argument on the x87 stack straight from dir (fld y,
//   fld z, fmul/faddp, x*x spilled from SSE) and keeps x in xmm0 with x*x in a
//   second register for the comparison, so the expression has to read through
//   the pointer rather than through float locals: with locals cl evaluates the
//   sum in SSE and only the 1/sqrtf part goes to x87.  The comparison is
//   written right-hand-side-first because retail evaluates px,py,pz before
//   z*z + y*y + x*x.
//   Remaining byte difference (191 vs 196): cl hoists the `r` reload out of
//   the fsqrt gap into xmm7 and keeps it live across the sqrt, where retail
//   loads it once after `pop esi` into xmm1 and copies px to xmm7.
int cPlanetModel::IsPositionBelowSurface(cSPVector3 const * dir) const {
  float radius = GetRadiusAt(dir);
  float inv = 1.0f / sqrtf((dir->y * dir->y + dir->z * dir->z) + dir->x * dir->x +
                           kVectorEpsilon);
  float v[3];
  v[0] = dir->x * inv;
  v[1] = dir->y * inv;
  v[2] = dir->z * inv;
  float px = v[0] * radius;
  float py = radius * v[1];
  float pz = radius * v[2];
  if ((px * px + pz * pz) + py * py >
      (dir->z * dir->z + dir->y * dir->y) + dir->x * dir->x)
    return 1;
  return 0;
}

// @ 0x00b81a40
// SP::cPlanetModel::FUN_00b81a40
//   A point on the surface offset from `dir`: build the local surface frame
//   (the terrain sphere's BuildSurfaceOrientation applied to the quaternion
//   from the reference direction, or the fixed 0x0156b614 orientation when no
//   sphere is attached), step `radius` along the frame by the cos/sin of the
//   angle, then re-project the sum onto the surface radius.  A direction
//   shorter than one unit is replaced by a random unit direction.
void cPlanetModel::FUN_00b81a40(cSPVector3 * out, cSPVector3 const * dir,
                                float radius, float angle) {
  float p[3];
  Quat q1;
  Quat q2;
  float m[9];
  float r;
  float inv;

  if (kOne >= dir->x * dir->x + dir->y * dir->y + dir->z * dir->z) {
    // Not a usable direction: start from a random unit direction instead.
    RandomDirection3(&q1.x);
    r = GetRadiusAt((cSPVector3 const *)&q1);
  } else {
    // Offset along the surface frame: `radius` units in the direction the
    // frame's basis makes with the cos/sin of `angle`.  p[] holds the offset
    // first and the world position afterwards.
    p[2] = 0.0f;
    p[0] = cosf(angle) * radius;
    p[1] = sinf(angle) * radius;

    Quat * u = QuaternionFromDirections(&q1, kRefDirection, (const float *)dir);
    if (mpISphere != 0) {
      mpSphere->BuildSurfaceOrientation(&q2, *dir, *u);
    } else {
      q2.x = kDefaultOrientation.x;
      q2.y = kDefaultOrientation.y;
      q2.z = kDefaultOrientation.z;
      q2.w = kDefaultOrientation.w;
    }

    Matrix3FromQuaternion(m, &q2.x);
    // The frame basis is applied as three columns; every component reads the
    // whole offset before any of it is overwritten.  The additions are grouped
    // (p[1] term + p[2] term) + p[0] term, matching retail's evaluation order.
    float y = (m[4] * p[1] + m[7] * p[2]) + m[1] * p[0];
    float z = (m[5] * p[1] + m[8] * p[2]) + m[2] * p[0];
    float x = (m[3] * p[1] + m[6] * p[2]) + m[0] * p[0];
    p[0] = dir->x + x;
    p[1] = dir->y + y;
    p[2] = dir->z + z;

    r = GetRadiusAt((cSPVector3 const *)p);
    inv = 1.0f / sqrtf((p[1] * p[1] + p[2] * p[2]) + p[0] * p[0]);
    q1.y = p[1] * inv;
    q1.z = p[2] * inv;
    q1.x = inv * p[0];
  }

  // Both branches funnel through the same scale-by-radius tail; q1 doubles as
  // the working vector (retail keeps it in the same stack slot).  The stores
  // are written z, y, x to match retail's schedule.
  out->z = q1.z * r;
  out->y = q1.y * r;
  out->x = q1.x * r;
}

}  // namespace SP
