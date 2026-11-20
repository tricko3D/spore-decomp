// 0x00d956e0: creature locomotion tick that steers toward / wanders around its
// tribe's anchor point (position at CTribe +0x138).
//
// Two cases:
//   * the creature is closer than 16 units (squared 256) to the anchor: sprint and
//     take one frame's step (speed from GetSpeed(3,0)) toward it;
//   * otherwise, when the locomotive sub-object reports it is near its goal, pick a
//     random world position 4..8 units around the current position, fill a move
//     parameters block, hand it to the locomotion vtable slot at +0xdc, and
//     interrupt the animation with one of three consecutive ids.
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast.
#include "types.h"
#include <math.h>
#include <float.h>

namespace SP {

struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {}
};

// Parameters block passed to the locomotion sub-object (vtable +0xdc).  The first
// member is a ref-counted buffer that the callee may replace; the caller releases
// it afterwards.
struct MoveParams {
    void* mpBuffer;                 // +0x00
    int   m04;                      // +0x04
    int   m08;                      // +0x08
    int   m0c;                      // +0x0c
    int   m10;                      // +0x10
    Vec3  mPoint;                   // +0x14
    float m20;                      // +0x20
    int   m24;                      // +0x24
    int   m28;                      // +0x28
    int   m2c;                      // +0x2c
    int   m30;                      // +0x30
    int   m34;                      // +0x34
    int   m38;                      // +0x38
    int   m3c;                      // +0x3c
    int   m40;                      // +0x40
    int   m44;                      // +0x44
    int   m48;                      // +0x48
    bool  m4c;                      // +0x4c
    char  m4d;                      // +0x4d
    char  m4e;                      // +0x4e
    char  m4f;                      // +0x4f
    float m50;                      // +0x50
    float m54;                      // +0x54
    float m58;                      // +0x58
    int   m5c;                      // +0x5c
    float m60;                      // +0x60
    float m64;                      // +0x64
    float m68;                      // +0x68
    float m6c;                      // +0x6c
    int   m70;                      // +0x70
};

class cPlanetModel {
public:
    Vec3 MakeRandomWorldPosition(const Vec3& center, float minRadius, float maxRadius);  // 0xb81780
};

cPlanetModel* __cdecl PlanetModel();                    // 0xb3d350

// Locomotion sub-object of a creature (creature +0xc0), own vtable.
struct Loco {
    virtual void s00(); virtual void s01(); virtual void s02(); virtual void s03();
    virtual void s04(); virtual void s05(); virtual void s06(); virtual void s07();
    virtual void s08(); virtual void s09(); virtual void s10();
    virtual const Vec3* GetPosition();                  // +0x2c
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21(); virtual void s22(); virtual void s23();
    virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual void s28(); virtual void s29(); virtual void s30(); virtual void s31();
    virtual void s32(); virtual void s33(); virtual void s34(); virtual void s35();
    virtual void s36(); virtual void s37(); virtual void s38(); virtual void s39();
    virtual void s40(); virtual void s41(); virtual void s42(); virtual void s43();
    virtual void s44(); virtual void s45(); virtual void s46(); virtual void s47();
    virtual void s48(); virtual void s49(); virtual void s50(); virtual void s51();
    virtual void s52(); virtual void s53(); virtual void s54();
    virtual void SetMoveParams(MoveParams* params);     // +0xdc
    bool IsNearGoal();                                  // 0xc42e20
};

// The creature's tribe / anchor object; its world position sits at +0x138.
class CTribe {
public:
    char pad00[0x138];
    Vec3 mPos;                                          // +0x138
};

struct Creature {
    char pad00[0xc0];
    Loco loco;                                          // +0xc0
    char pad04[0xfb8 - 0xc0 - sizeof(Loco)];
    CTribe* mpTribe;                                    // +0xfb8

    void  SetSprinting(int sprint);                                  // 0xc1ad10
    float FUN_00c0cfa0(int a, int b);                                // 0xc0cfa0
    void  FUN_00c0cff0(int mode);                                    // 0xc0cff0
    void  MoveToPointAtSpeed(int mode, const Vec3* p, float speed, float f);  // 0xc1c1d0
    void  InterruptAnimation(unsigned id, int variant, int flags);   // 0xc12310
};

struct Rng { unsigned RandomUint32Uniform(unsigned n); };   // 0xa68fb0
extern Rng g_Rng;                                           // 0x1601760

void FUN_00f47380(void* p);                                 // 0xf47380

// @ 0x00d956e0
bool __cdecl FUN_00d956e0(Creature* self)
{
    Vec3 pos = *self->loco.GetPosition();
    Vec3 anchor = self->mpTribe->mPos;
    Vec3 dir;
    float speed;
    float inv;
    float distSq = (pos.z - anchor.z) * (pos.z - anchor.z)
                 + (pos.y - anchor.y) * (pos.y - anchor.y)
                 + (pos.x - anchor.x) * (pos.x - anchor.x);

    if (distSq < 256.0f) {
        // dest belongs to this block: the original frame gives it the first local slot
        // ([esp+8], the slot the walk target buffer reuses in the other branch).
        Vec3 dest;
        self->SetSprinting(1);
        speed = self->FUN_00c0cfa0(3, 0);

        dir.x = pos.x - anchor.x;
        dir.y = pos.y - anchor.y;
        dir.z = pos.z - anchor.z;
        float dist = sqrtf(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x + 1e-8f);
        inv = 1.0f / dist;

        dest.x = pos.x + dir.x * inv * speed;
        dest.y = pos.y + dir.y * inv * speed;
        dest.z = pos.z + dir.z * inv * speed;

        self->MoveToPointAtSpeed(2, &dest, 1.0f, 2.0f);
    } else if (self->loco.IsNearGoal()) {
        Vec3 wander = PlanetModel()->MakeRandomWorldPosition(pos, 4.0f, 8.0f);

        MoveParams params;
        params.mpBuffer = 0;
        params.m04 = 0;
        params.m08 = 0;
        params.mPoint.x = wander.x;
        params.mPoint.y = wander.y;
        params.mPoint.z = wander.z;
        params.m20 = 1.0f;
        params.m24 = 0;
        params.m4c = false;
        params.m50 = 0.0f;
        params.m54 = 0.0f;
        params.m58 = 0.0f;
        params.m5c = 1;
        params.m60 = 2.0f;
        params.m64 = 0.9f;
        params.m68 = FLT_MAX;
        params.m6c = 0.0f;
        params.m70 = 0;

        self->loco.SetMoveParams(&params);
        self->FUN_00c0cff0(2);
        self->InterruptAnimation(g_Rng.RandomUint32Uniform(3) + 0x2481dc8u, -1, 0);

        if (params.mpBuffer != 0 && ((int*)params.mpBuffer)[-1] != 0)
            FUN_00f47380(params.mpBuffer);
    }
    return true;
}

} // namespace SP
