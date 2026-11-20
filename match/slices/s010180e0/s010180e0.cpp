// Slice s010180e0 -- 0x010180e0: clips a point against a four-plane wedge built around a direction
// (ConeClip). thiscall, 8 stack arguments (ret 0x20), the 8th is unused by the original.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast.
#include "types.h"
#include <math.h>
#pragma intrinsic(sqrt)

template <class T> inline const T &Max(const T &a, const T &b) { return (a < b) ? b : a; }
template <class T> inline const T &Min(const T &a, const T &b) { return (b < a) ? b : a; }

extern float g_016dd390;   // 0x016dd390
extern float g_016dd38c;   // 0x016dd38c

struct ClipVec3 {
    float x, y, z;
    ClipVec3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
};

struct ConeClip {
    char  pad00[0x8c];
    float cx, cy, cz;       // +0x8c, +0x90, +0x94
    char  pad98[0xbc - 0x98];
    float mRatio;           // +0xbc

    void Clip(ClipVec3 *out, float px, float py, float pz, float dx, float dy, float dz, float unused);
};

// @ 0x010180e0
void ConeClip::Clip(ClipVec3 *out, float px, float py, float pz, float dx, float dy, float dz, float unused)
{
    float t0 = 1.0f - mRatio;
    float r = mRatio;
    float ez = pz - cz;
    float ey = py - cy;
    float ex = px - cx;
    out->x = px;
    out->y = py;
    out->z = pz;
    if (1.0f < (ez * ez + ey * ey) + ex * ex) {
        float s48 = px - cx;
        float s44 = py - cy;
        float s40 = pz - cz;
        float s3c = px - (cx + dx);
        float s38 = py - (cy + dy);
        float s34 = pz - (cz + dz);

        float inv = 1.0f / sqrtf(dx * dx + dz * dz + dy * dy);
        float ux = dx * inv;
        float uy = dy * inv;
        float uz = dz * inv;
        float nuy = -uy;
        float nux = -ux;

        float inv2 = 1.0f / sqrtf(nuy * nuy + ux * ux);
        float a2c = ux * inv2;
        float a28 = inv2 * 0.0f;
        float invn = inv2 * nuy;
        float c1 = -uz * a2c - nuy * a28;
        float c2 = a28 * nux + uz * invn;
        float c3 = nuy * invn - a2c * nux;

        float k = r * g_016dd390;
        float z1 = uz * k + c3;
        float x1 = ux * k + c1;
        float y1 = uy * k + c2;
        float n1 = 1.0f / sqrtf(z1 * z1 + y1 * y1 + x1 * x1);
        float X1 = x1 * n1;
        float Y1 = y1 * n1;
        float Z1 = z1 * n1;

        float z2 = uz * k - c3;
        float x2 = ux * k - c1;
        float y2 = uy * k - c2;
        float n2 = 1.0f / sqrtf(z2 * z2 + y2 * y2 + x2 * x2);
        float X2 = x2 * n2;
        float Y2 = y2 * n2;
        float Z2 = z2 * n2;

        float k2 = r * g_016dd38c;
        float x3 = ux * k2 + invn;
        float z3 = uz * k2 + a28;
        float y3 = uy * k2 + a2c;
        float n3 = 1.0f / sqrtf(x3 * x3 + z3 * z3 + y3 * y3);
        float X3 = x3 * n3;
        float Y3 = y3 * n3;
        float Z3 = z3 * n3;

        float x4 = ux * k2 - invn;
        float z4 = uz * k2 - a28;
        float y4 = uy * k2 - a2c;
        float n4 = 1.0f / sqrtf(x4 * x4 + z4 * z4 + y4 * y4);
        float X4 = x4 * n4;
        float Y4 = y4 * n4;
        float Z4 = z4 * n4;

        float t = (X1 * s3c + Z1 * s34) + Y1 * s38;
        if (0.0f < t)
            t0 = Max(t0, t / ((X1 * s48 + Z1 * s40) + Y1 * s44));
        t = (X2 * s3c + Z2 * s34) + Y2 * s38;
        if (0.0f < t)
            t0 = Max(t0, t / ((X2 * s48 + Z2 * s40) + Y2 * s44));
        t = (X3 * s3c + Z3 * s34) + Y3 * s38;
        if (0.0f < t)
            t0 = Max(t0, t / ((X3 * s48 + Z3 * s40) + Y3 * s44));
        t = (Z4 * s34 + Y4 * s38) + X4 * s3c;
        if (0.0f < t)
            t0 = Max(t0, t / ((Z4 * s40 + Y4 * s44) + X4 * s48));

        float f = Min(t0, 1.0f);
        ClipVec3 res(cx + (px - cx) * f, cy + (py - cy) * f, cz + (pz - cz) * f);
        *out = res;
    }
}

// ---------------------------------------------------------------- planet camera
namespace SP {

struct Vector3 { float x, y, z; };

// Camera rig hung off the player inventory at +0x34.
class cInventoryCamera {
 public:
  virtual void pv00(); virtual void pv01(); virtual void pv02(); virtual void pv03();
  virtual void pv04(); virtual void pv05(); virtual void pv06(); virtual void pv07();
  virtual void pv08(); virtual void pv09(); virtual void pv10();
  virtual Vector3* GetPosition();   // +0x2c
};

class cPlayerInventory {
 public:
  char pad00[0x34];
  cInventoryCamera mCamera;         // +0x34
  char pad38[0x74c - 0x38];
  bool mbCameraOverride;            // +0x74c
  char pad74d[0x750 - 0x74d];
  Vector3 mCameraOverridePos;       // +0x750
};

class cSPSimulatorSpaceGame {
 public:
  cPlayerInventory* GetPlayerInventory();   // 0x00a1ad60
};

cSPSimulatorSpaceGame* GetUFOSimulator();   // 0x00ffbe50

// Game clock; bit 0 of the flags at +0x48 pauses the scripted camera.
class cGameTimeManager {
 public:
  char pad00[0x48];
  uint32_t mnFlags;                 // +0x48
};
cGameTimeManager* GameTimeManager();   // 0x00b3d380

class cSPSpacePlanetCameraController {
 public:
  float GetCurrentZoom();           // 0x010187c0
};

// @ 0x010187c0
// Distance of the planet camera from the inventory's camera anchor (or from the scripted
// override position while the game clock is running).
float cSPSpacePlanetCameraController::GetCurrentZoom()
{
  cPlayerInventory* pInventory = GetUFOSimulator()->GetPlayerInventory();
  const Vector3* pPos;
  if (pInventory->mbCameraOverride && (GameTimeManager()->mnFlags & 1) == 0) {
    pPos = &pInventory->mCameraOverridePos;
  } else {
    pPos = pInventory->mCamera.GetPosition();
  }
  return sqrtf(pPos->x * pPos->x + pPos->y * pPos->y + pPos->z * pPos->z);
}

}  // namespace SP
