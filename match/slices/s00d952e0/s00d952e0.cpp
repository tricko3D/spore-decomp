// @ 0x00d952e0
// SP::CitizenTree::UfoFear_Tick (creature behaviour-tree tick, cdecl: the creature
// is the first argument, the caller's state record the sixth).
//
// Retail asks the behaviour-tree stimuli block for the UFO/fear stimulus
// (Play(0x4000, 0)); with no stimulus it reports failure.  It copies the
// stimulus position (target + 0xc) and payload (target + 0x18) into the caller's
// state, clears the stimulus, then steps the citizen one unit towards the target
// along the planet surface and faces the surface direction while it interrupts
// the current animation.
//
// The retail row at 0x00d952e0 is 718 bytes, which is two functions: this body
// (0x00d952e0..0x00d9550f, 560 bytes) plus an unnamed sibling that starts at
// 0x00d95510 (two pushes in its prologue: `push ebx; push ebp; push esi`, so the
// fingerprint generator's "ret followed by push ebp; mov ebp, esp" cut did not
// fire).  Four of the row's 13 address masks (offsets 640, 646, 671, 694) sit
// inside that sibling.  See keep.md in the investigation directory: the row
// cannot be byte-exact until the fingerprint is regenerated.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "game/types.h"
#include <math.h>

#define PV(n) virtual void pv##n();

struct Vec3 { float x, y, z; };

struct AnimTarget {
    uint8_t pad00[0x0c];
    Vec3    mPos;       // +0x0c
    int     mField18;   // +0x18
};

struct BehaviorStimuli {
    AnimTarget* Play(int flags, int arg);       // 0x00bc96a0
    void        ClearAll(int flags, int arg);   // 0x00bc98f0
};

struct BehaviorTree {
    uint8_t         pad00[0x08];
    BehaviorStimuli mStimA;                     // +0x08
};

struct Locomotion {                             // cLocomotiveObject, creature + 0xc0
    PV(0) PV(1) PV(2) PV(3) PV(4) PV(5) PV(6) PV(7) PV(8) PV(9) PV(10)
    virtual const Vec3* GetPosition();          // +0x2c (slot 11)
};

struct Creature {
    uint8_t       pad000[0xc0];
    Locomotion    mLocomotion;                  // +0x0c0
    uint8_t       pad0d4[0xb4c - 0xc0 - sizeof(Locomotion)];
    BehaviorTree* mpBehaviorTree;               // +0xb4c
    void MoveToPointAndFacingAtSpeed(int mode, const Vec3* p, const Vec3* facing,
                                     float speed, float turn);   // 0x00c1c5c0
    void InterruptAnimation(uint32_t anim, int a, int b);        // 0x00c12310
};

struct UfoFearState {
    Vec3    mPos;       // +0x00
    bool    mValid;     // +0x0c
    uint8_t pad0d[0x03];
    int     mField10;   // +0x10
};

struct PlanetModel {                            // cPlanetModel singleton
    const Vec3* DirectionToSurfacePosition(Vec3* out, const Vec3* in);  // 0x00b815a0
};
PlanetModel* GetPlanetModel();                  // 0x00b3d350

// Length epsilon used by the inlined direction normalisation (retail reads the
// shared 4-byte float 0x013ec4b8); the 2.0f turn rate is the shared float
// 0x01470f1c, while the 1.0f speed is the literal folded into 1.0f / length.
extern const float kFlSafeEps;
extern const float kFlTwo;

bool FUN_00d952e0(Creature* c, int a2, int a3, int a4, int a5,
                  UfoFearState* st, int a7, int a8)
{
    AnimTarget* target = c->mpBehaviorTree->mStimA.Play(0x4000, 0);
    if (!target)
        return false;

    float px = target->mPos.x;
    float py = target->mPos.y;
    float pz = target->mPos.z;
    st->mPos.x = px;
    st->mPos.y = py;
    st->mPos.z = pz;
    st->mField10 = target->mField18;
    c->mpBehaviorTree->mStimA.ClearAll(0x4000, 0);

    Vec3 dir;
    const Vec3* here = c->mLocomotion.GetPosition();
    Vec3 cur;
    cur.x = here->x;
    cur.y = here->y;
    cur.z = here->z;
    Vec3 diff;
    Vec3 surfPos;

    diff.x = st->mPos.x - cur.x;
    diff.y = st->mPos.y - cur.y;
    diff.z = st->mPos.z - cur.z;

    float inv = 1.0f / sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z
                             + kFlSafeEps);
    dir.x = diff.x * inv;
    dir.y = diff.y * inv;
    dir.z = diff.z * inv;

    diff.x = cur.x + dir.x;
    diff.y = cur.y + dir.y;
    diff.z = cur.z + dir.z;

    const Vec3* surf = GetPlanetModel()->DirectionToSurfacePosition(&surfPos, &diff);

    dir = *surf;
    diff.x = dir.x - cur.x;
    diff.y = dir.y - cur.y;
    diff.z = dir.z - cur.z;

    inv = 1.0f / sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z
                       + kFlSafeEps);
    dir.x = diff.x * inv;
    dir.y = diff.y * inv;
    dir.z = diff.z * inv;

    c->MoveToPointAndFacingAtSpeed(2, &cur, &dir, 1.0f, kFlTwo);
    c->InterruptAnimation(0x2481db0, -1, 0);
    st->mValid = true;
    return true;
}
