// SP::MOVE_Tick -- retail 2500 bytes, __cdecl, 8 stack args.
// arg1 = creature, arg5 = flags (bit2 and bit3 are read), arg6 = movement state
// (4 floats at +0x00..+0x0c, a bool at +0x11, an object at +0x18), arg8 = dt.
// The movement group lives in a 0x250-byte pool slot at 0x0159d548; every field
// access below is the retail offset read from the disassembly.
#include "types.h"
#include <math.h>

namespace SP {

struct Vec3 {
    float mX;
    float mY;
    float mZ;
};

class cGroupMovement;
class cSPCreatureBase;
class cLocomotiveObject;

// --- pooled objects ---------------------------------------------------------

struct cGroupSlot {                                                // 0x250-byte pool slot
    unsigned mFlags;                                               // +0x000 (bit 31 = occupied)
    unsigned char mObject[0x250 - 4];                              // +0x004 (the group)
};

class cGroupMovementPool {
public:
    cGroupSlot* mpBegin;                                           // +0x00 (0x0159d548)
    cGroupSlot* mpEnd;                                             // +0x04 (0x0159d54c)

    // the inline form of the lookup the tick uses before it calls GetObject
    __forceinline cGroupMovement* Lookup(unsigned slotRef)
    {
        unsigned index = slotRef & 0xbfffffff;
        int count = (int)((char*)mpEnd - (char*)mpBegin) / 0x250;
        if (index >= (unsigned)count)
            return 0;
        cGroupSlot* slot = mpBegin + index;
        if ((slot->mFlags >> 31) & 1)
            return (cGroupMovement*)slot->mObject;
        return 0;
    }

    cGroupMovement* GetObject(unsigned slotRef);                   // 0x00db3c40 (ret 4)
};

extern cGroupMovementPool gGroupMovementPool;                      // 0x0159d548

// --- virtual interfaces ------------------------------------------------------

class IGameObject {
public:
    virtual void v000();
    virtual void v004();
    virtual void v008();
    virtual void* Cast(unsigned typeId);                           // +0x0c
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual unsigned GetClassId();                                 // +0x20
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void v048();
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void v05c();
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084(int, int, int);                              // +0x84
    virtual void v088();
    virtual void v08c();
    virtual void v090();
    virtual void v094();
    virtual void v098();
    virtual void v09c();
    virtual void v0a0();
    virtual void v0a4();
    virtual void v0a8();
    virtual void v0ac();
    virtual void v0b0();
    virtual void v0b4();
    virtual void v0b8();
    virtual void v0bc();
    virtual void v0c0();
    virtual void v0c4();
    virtual void v0c8();
    virtual void v0cc();
    virtual void v0d0();
    virtual void v0d4();
    virtual void v0d8();
    virtual void v0dc();
    virtual void v0e0();
    virtual void v0e4();
    virtual void v0e8();
    virtual void v0ec();
    virtual void v0f0(int);                                        // +0xf0
};

class cLocomotiveObject {                                          // the creature's +0xc0 sub-object
public:
    virtual void l000();
    virtual void l004();
    virtual void l008();
    virtual void l00c();
    virtual void l010();
    virtual void l014();
    virtual void l018();
    virtual void l01c();
    virtual void l020();
    virtual void l024();
    virtual void l028();
    virtual Vec3* GetPosition();                                   // +0x2c
    virtual void l030();
    virtual void l034();
    virtual void l038();
    virtual void l03c();
    virtual void l040();
    virtual void l044();
    virtual void l048();
    virtual void l04c();
    virtual void l050();
    virtual void l054();
    virtual bool IsActive();                                       // +0x58
    virtual Vec3* GetNormal(Vec3* out);                            // +0x5c

    bool IsNearGoal();                                             // 0x00c42e20 (ret 0)
    void* GetCurveSample();                                        // 0x00c41ec0 (ret 0)
};

// --- the objects the tick drives --------------------------------------------

struct cAnimHandle {                                               // [creature+0xb54]
    unsigned char mPad000[0x18c];
    unsigned mObject;                                              // +0x18c
    void ReleaseObject(unsigned id);                               // 0x00a02ab0 (ret 4)
};

struct cEntityInfo {
    float mDoneTime;                                               // +0x00
    float mDistanceFromSlot2;                                      // +0x04
    int mSlotIdx;                                                  // +0x08
};

struct cInfoNode {
    cLocomotiveObject* mpKey;                                      // +0x00
    cEntityInfo mInfo;                                             // +0x04
    cInfoNode* mpNext;                                             // +0x10
};

struct cMovementIterator {
    cInfoNode* mpNode;                                             // +0x00
    void* mpBucket;                                                // +0x04
};

struct cEntityInfoMap {                                            // eastl::hash_map<loco*, cEntityInfo>
    int mnBucketCount;                                             // +0x00
    cInfoNode** mppBucketArray;                                    // +0x04
    int mnBucket;                                                  // +0x08
    unsigned mPad0c[7];                                            // +0x0c
    cMovementIterator find(cLocomotiveObject* const& key);          // 0x00db4780 (ret 8)
    cEntityInfo* FindOrAdd(cLocomotiveObject* const& key);          // 0x00db5cf0 (ret 4)
};

class cActionCircle {
public:
    unsigned char mPad000[0x50];
    void* mpSlotBegin;                                             // +0x50
    unsigned mSlotCount;                                           // +0x54
};

class cGroupMovement {
public:
    unsigned char mPad000[0x008];
    void* mpCurveBegin;                                            // +0x008
    void* mpCurveEnd;                                              // +0x00c
    void* mpCurveStorage;                                          // +0x010
    unsigned char mPad014[8];                                      // +0x014
    void* mpEntityBegin;                                           // +0x01c
    void* mpEntityEnd;                                             // +0x020
    void* mpEntityStorage;                                         // +0x024
    unsigned char mPad028[0x054 - 0x028];
    cEntityInfoMap mInfoMap;                                       // +0x054
    unsigned char mPad07c[0x190 - 0x07c];
    void* mpObject;                                                // +0x190
    Vec3 mGoal;                                                    // +0x194
    unsigned char mPad1a0[0x1c8 - 0x1a0];
    float mThreshold1c8;                                           // +0x1c8
    int mFlags1cc;                                                 // +0x1cc
    bool mbFlag1d0;                                                // +0x1d0
    bool mbFlag1d1;                                                // +0x1d1
    unsigned char mPad1d2[2];
    cActionCircle mCircle;                                         // +0x1d4
    unsigned char mPad230[0x23c - 0x230];
    bool mbFlag23c;                                                // +0x23c
    unsigned char mPad240[4];
    bool mbFlag244;                                                // +0x244
    unsigned char mPad245[3];
    unsigned mSoundId248;                                          // +0x248

    void FUN_00db6fa0(cLocomotiveObject* loco);                    // 0x00db6fa0 (ret 4)
    void FUN_00db6ef0(cLocomotiveObject* loco);                    // 0x00db6ef0 (ret 4)
};

struct cEntryState {
    unsigned char mPad000[0x1a4];
    unsigned mGroupSlotRef;                                        // +0x1a4
    cSPCreatureBase* CurrentId();                                  // 0x00bc9b10 (ret 0)
};

struct cAnimController {                                           // [mgr+0x008]
    unsigned mPad000;
    unsigned PlayIdle(int a, int b);                               // 0x00bc96a0 (ret 8)
    void PlaySound(int a, float b, int c, int d);                  // 0x00bc97f0 (ret 0x10)
};

struct cLocomotionMgr {
    unsigned char mPad000[8];
    cAnimController mAnimController;                               // +0x008
    unsigned char mPad00c[0x600 - 0x00c];
    cEntryState* mpEntry;                                          // +0x600
    bool SelectPath();                                             // 0x00bca2a0 (ret 0)
    void* GetCurrentTarget();                                      // 0x00bca2c0 (ret 0)
};

class cTimerSource {
public:
    float GetTime();                                               // 0x00bfc490 (ret 0)
    unsigned mPad004;
};

class cFoundSubObject {                                            // [found + 0x39]
public:
    Vec3* GetPosition();                                           // 0x00cce910 (ret 0)
};

class cSPCreatureBase : public IGameObject {
public:
    unsigned char mPad004[0x0c0 - 0x004];
    cLocomotiveObject mLocomotive;                                 // +0x0c0
    unsigned char mPad0c4[0x110 - 0x0c4];
    unsigned mFlags110;                                            // +0x110
    unsigned char mPad114[0x137 - 0x114];
    unsigned char mbFlag137;                                       // +0x137
    unsigned char mPad138[0x5a8 - 0x138];
    cTimerSource mTimerSource;                                     // +0x5a8
    unsigned char mPad5ac[0xb20 - 0x5ac];
    void* mpObjectB20;                                             // +0xb20
    unsigned char mPadB24[0xb4c - 0xb24];
    cLocomotionMgr* mpMgr;                                         // +0xb4c
    cAnimHandle* mpAnimHandle;                                     // +0xb54
    unsigned mFlagsB58;                                            // +0xb58

    bool IsEligible();                                             // 0x00c0c0e0 (ret 0)
    unsigned GetCurrentAnimation();                                // 0x00c0e040 (ret 0)
    bool CanPlayAnimation(unsigned anim);                          // 0x00c0e0c0 (ret 4)
    bool IsPlayingAnimation(unsigned anim);                        // 0x00c0e120 (ret 4)
    void InterruptAnimation(unsigned anim, int a, int b);          // 0x00c12310 (ret 0xc)
    void PlayAnimation(unsigned anim, int a, int b);               // 0x00c12190 (ret 0xc)
    void Hover(int on);                                            // 0x00c13a30 (ret 4)
    void UpdateLocomotion(int a);                                  // 0x00c14750 (ret 4)
    bool TryJumpToTarget(int a, Vec3* target, void* c, float d, int e);  // 0x00c190e0
    void SetSprinting(int on);                                     // 0x00c1ad10 (ret 4)
    int GetMoveState();                                            // 0x00c22dd0 (ret 0)
    unsigned GetMoveStateId();                                     // 0x00c22dc0 (ret 0)
    bool CheckVariant();                                           // 0x00c232d0 (ret 0)
    void* GetSoundSubject();                                       // 0x00c23750 (ret 0)
    bool CheckState(unsigned a, int b);                            // 0x00c25480 (ret 8)
    unsigned GetState();                                           // 0x00c26170 (ret 0)
};

struct cSoundSlots {
    unsigned PlayEffort(void* subject);                            // 0x00bc2240 (ret 4)
    void Stop(unsigned id);                                        // 0x00bc23d0 (ret 4)
};

struct cStrategyData {
    unsigned char mPad000[0x1c8];
    cSoundSlots* mpSoundSlots;                                     // +0x1c8
};

struct cAnimInfo {
    unsigned char mPad000[0x34];
    unsigned mAnimId;                                              // +0x34
};

struct MoveState {
    float mTimer0;                                                 // +0x00
    float mDistance4;                                              // +0x04
    float mHover8;                                                 // +0x08
    float mFieldC;                                                 // +0x0c
    unsigned char mPad10[1];
    bool mbFlag11;                                                 // +0x11
    unsigned char mPad12[6];
    void* mpObject18;                                              // +0x18
};

struct MoveTarget {
    cSPCreatureBase* mpSelf;                                       // +0x00
    void* mpObject;                                                // +0x04
    Vec3 mGoal;                                                    // +0x08
};

// --- callees ----------------------------------------------------------------

bool Vec3NotEqual(const Vec3* a, const Vec3* b);                   // 0x0041dd30
float Vec3Length(const Vec3* v);                                   // 0x004df2d0
bool __stdcall SelectPathStep(cSPCreatureBase* self, MoveState* state, MoveTarget* target);  // 0x00bcb510
cStrategyData* StrategyAt(Vec3 position);                          // 0x00cd40b0
unsigned FilterAnimation(unsigned a);                              // 0x00d999d0
unsigned FilterAnimation2(unsigned a);                             // 0x00d99470
cAnimInfo* GetAnimationInfo(unsigned id);                          // 0x00c9cec0
Vec3* TransformPosition(Vec3* out, Vec3* in);                      // 0x00b3d350
void* ProjectPosition(Vec3* p);                                    // 0x00b81630
bool FinishMove(cSPCreatureBase* self, void* object);              // 0x00da6270

// globals the retail tick reads; 0x0158122c / 0x0158123c hold 0 in the image and
// are tuned at runtime, the rest are the zero vectors/floats it compares against.
static float gStepLimit = 0.0f;                                    // 0x0158122c
static float gApproachScale = 0.0f;                               // 0x0158123c
static float gZero1230 = 0.0f;                                     // 0x01581230
static Vec3  gZeroVector = { 0.0f, 0.0f, 0.0f };                   // 0x01581234
static float gZero5378 = 0.0f;                                     // 0x01485378
static float gZero7a10 = 0.0f;                                     // 0x01687a10
static float gSprintDelta = 0.0f;                                  // 0x01581264

// the original calls min by reference (it selects between the two operands' addresses)
template <class T> const T& MinRef(const T& a, const T& b) { return b < a ? b : a; }

// @ 0x00DB7190
bool MOVE_Tick(cSPCreatureBase* self, int, int, int, unsigned flags, MoveState* state, int, float dt)
{
    // retail keeps the validated pointer in a local and only overwrites it with 0
    // after the class test, so spell it as one conditional expression.
    cSPCreatureBase* creature =
        (self != 0 && self->GetClassId() == 0x18eb4b7) ? self : 0;

    cEntryState* entry = self->mpMgr->mpEntry;
    unsigned slotRef = entry->mGroupSlotRef;
    cGroupMovement* group = gGroupMovementPool.Lookup(slotRef);

    state->mTimer0 = state->mTimer0 - dt;
    if (state->mTimer0 < 0.0f) {
        state->mTimer0 = 1.0f;
        MoveTarget target;
        target.mGoal = group->mGoal;
        target.mpSelf = self;
        target.mpObject = group->mpObject;
        if (!self->mpMgr->SelectPath()) {
            if (SelectPathStep(self, state, &target)) {
                cGroupMovement* oldGroup = gGroupMovementPool.GetObject(slotRef);
                if (oldGroup != 0)
                    oldGroup->FUN_00db6fa0(&self->mLocomotive);
                cGroupMovement* newGroup =
                    gGroupMovementPool.GetObject(self->mpMgr->mpEntry->mGroupSlotRef);
                newGroup->FUN_00db6ef0(&self->mLocomotive);
            }
        }
    }

    state->mDistance4 = state->mDistance4 - dt;
    state->mHover8 = state->mHover8 - dt;
    if (self->mbFlag137 == 0 || (self->mFlags110 & 0x1000) != 0)
        state->mFieldC = dt + state->mFieldC;

    bool bFlags2 = ((flags >> 2) & 1) != 0;
    bool near = self->mLocomotive.IsNearGoal();
    cSPCreatureBase* current = self;

    if (self->GetClassId() == 0x18eb4b7) {
    current = entry->CurrentId();
    if (self == current) {
        void* found = self->mpMgr->GetCurrentTarget();
        if (found != 0 && *(void**)found == state->mpObject18 &&
            *(unsigned short*)((char*)found + 0x4b) == 0x31) {
            Vec3* position = ((cFoundSubObject*)((char*)found + 0x39))->GetPosition();
            if (Vec3NotEqual(position, &group->mGoal)) {
                group->mGoal = *position;
                group->mbFlag1d1 = true;
                if (group->mpObject != 0) {
                    ((IGameObject*)group->mpObject)->v0c0();
                    group->mpObject = 0;
                }
            }
        }
    }
    }

    if (self->mLocomotive.IsActive()) {
        Vec3* position = self->mLocomotive.GetPosition();
        if (group->mSoundId248 == 0) {
            void* subject = current->GetSoundSubject();
            if (subject != 0)
                group->mSoundId248 =
                    StrategyAt(*position)->mpSoundSlots->PlayEffort(subject);
        } else {
            StrategyAt(*position)->mpSoundSlots->Stop(group->mSoundId248);
        }
    }

    bool bSkip = false;
    if (self->GetClassId() == 0x18eb4b7) {
        if (!creature->CheckState(creature->GetState(), 1)) {
            bSkip = true;
            goto afterAnim;
        }
        unsigned anim;
        bool bFlags3 = ((flags >> 3) & 1) != 0;
        unsigned guid = self->GetCurrentAnimation();
        near = self->mLocomotive.IsNearGoal();
        bool known = guid == 0x25b3bc0 || guid == 0x5261d56 || guid == 0x5261d78 ||
                     guid == 0x772ff58 || guid == 0x77300f2 || guid == 0x77300fb ||
                     guid == 0x7995fdc || guid == 0x7982fb2;

        if ((self->mFlagsB58 & 0x1000) != 0) {
            if (near)
                self->Hover(0);
            goto afterAnim;
        }
        if (known)
            goto afterAnim;
        if (!(self->mbFlag137 != 0 || (self->mFlags110 & 0x1000) != 0))
            goto afterAnim;
        if (near)
            goto updateLocomotion;

        if (bFlags2) {
            anim = 0x591b056;
            if (creature->CheckVariant())
                anim = 0x62ff867;
            else if (creature->GetMoveState() == 1) {
                unsigned id = GetAnimationInfo(creature->GetMoveStateId())->mAnimId;
                if (id == 0)
                    ;                                          // keep the default animation
                else if (id == 0xffffffff)
                    goto updateLocomotion;
                else
                    anim = id;
            }
        } else if (bFlags3) {
            anim = 0x62eed44;
            if (creature->GetMoveState() == 2) {
                unsigned id = GetAnimationInfo(creature->GetMoveStateId())->mAnimId;
                if (id == 0)
                    ;                                          // keep the default animation
                else if (id == 0xffffffff)
                    goto updateLocomotion;
                else
                    anim = id;
            }
        } else {
            unsigned idle = self->mpMgr->mAnimController.PlayIdle(0, 0x80);
            unsigned filtered = FilterAnimation2(FilterAnimation(idle));
            if (filtered == 0)
                goto updateLocomotion;
            if (self->mTimerSource.GetTime() * 100.0f < gZero7a10)
                anim = 0x2c38d92;
            else
                goto afterAnim;
        }

        if (!creature->CanPlayAnimation(anim)) {
            creature->InterruptAnimation(anim, -1, 0);
            self->mpAnimHandle->ReleaseObject(self->mpAnimHandle->mObject);
            self->mpAnimHandle->mObject = 0;
        } else if (!creature->IsPlayingAnimation(anim)) {
            creature->PlayAnimation(anim, 1, -1);
        }
        goto afterAnim;

    updateLocomotion:
        self->UpdateLocomotion(0);
    }

afterAnim:
    if (near || bFlags2 || bSkip)
        goto final;

    if (self->mpObjectB20 != 0 && *(int*)((char*)self->mpObjectB20 + 0x61c) != 0) {
        if (state->mDistance4 < 0.0f || state->mHover8 > gZero1230) {
            if ((self->mFlagsB58 & 0x1000) == 0) {
                state->mDistance4 = Vec3Length(&gZeroVector);
                state->mHover8 = 0.0f;
                self->Hover(1);
                goto final;
            }
            if (state->mHover8 <= gZero1230)
                goto final;
            state->mDistance4 = Vec3Length(&gZeroVector);
            state->mHover8 = 0.0f;
            self->Hover(0);
            goto final;
        }
    }

    if (self->mbFlag137 != 0 && *(int*)((char*)self->mpObjectB20 + 0x610) > 0 &&
        state->mDistance4 < 0.0f && !self->IsEligible()) {
        void* sample = self->mLocomotive.GetCurveSample();
        Vec3 target = *(Vec3*)((char*)sample + 0x14);
        Vec3* position = self->mLocomotive.GetPosition();
        Vec3 delta;
        delta.mX = target.mX - position->mX;
        delta.mY = target.mY - position->mY;
        delta.mZ = target.mZ - position->mZ;
        float length = sqrtf(delta.mX * delta.mX + delta.mY * delta.mY + delta.mZ * delta.mZ);
        float scale = 1.0f / length;
        Vec3 direction;
        direction.mX = delta.mX * scale;
        direction.mY = delta.mY * scale;
        direction.mZ = delta.mZ * scale;
        Vec3 normal;
        self->mLocomotive.GetNormal(&normal);
        float dot = normal.mX * direction.mX + normal.mY * direction.mY +
                    normal.mZ * direction.mZ;
        if (dot > 0.8f) {
            const float& step = MinRef(gStepLimit, length);
            Vec3 move;
            move.mX = direction.mX * step;
            move.mY = direction.mY * step;
            move.mZ = direction.mZ * step;
            Vec3* here = self->mLocomotive.GetPosition();
            Vec3 moved;
            moved.mX = here->mX + move.mX;
            moved.mY = here->mY + move.mY;
            moved.mZ = here->mZ + move.mZ;
            Vec3* goal = TransformPosition(&moved, &direction);
            state->mDistance4 = Vec3Length(&gZeroVector);
            self->TryJumpToTarget(0, (Vec3*)ProjectPosition(goal), (void*)0x169f3b8, 0.0f, 0);
            goto final;
        }
        state->mDistance4 = gApproachScale * 0.5f;
        goto final;
    }

    if (group->mbFlag244 && state->mHover8 < 0.0f) {
        state->mHover8 = 1.0f;
        cEntityInfo* info = group->mInfoMap.FindOrAdd(&self->mLocomotive);
        bool sprint = false;
        if (info->mDistanceFromSlot2 > group->mThreshold1c8) {
            // retail compares the square roots: sqrt(dist) - sqrt(threshold) > gSprintDelta
            if (sqrtf(info->mDistanceFromSlot2) - sqrtf(group->mThreshold1c8) > gSprintDelta)
                sprint = true;
        }
        if (sprint) {
            state->mbFlag11 = true;
            self->SetSprinting(1);
        } else {
            state->mbFlag11 = false;
            self->SetSprinting(0);
        }
    }

final:
    {
        cMovementIterator it = group->mInfoMap.find(&self->mLocomotive);
        cInfoNode* end = (cInfoNode*)group->mInfoMap.mppBucketArray[group->mInfoMap.mnBucket];
        if (it.mpNode != end && it.mpNode->mInfo.mDoneTime > gZero5378) {
            if (creature == 0)
                return false;
            if (creature->mLocomotive.IsActive())
                creature->v0f0(0x29c);
            creature->mpMgr->mAnimController.PlaySound(0, 3.402823466e+38f, 0x20, 0);
            return FinishMove(creature, state->mpObject18);
        }
    }

    if (creature != 0) {
        if (creature->mLocomotive.IsActive() &&
            (*(int*)((char*)creature->mpMgr + 0x1cc) & 0x80) != 0) {
            creature->SetSprinting(1);
            return true;
        }
    } else if (self->GetClassId() == 0x18eb45e) {
        void* object = self->Cast(0xd0036e08);
        if (object != 0 && *(float*)((char*)object + 0x169c) > gZero5378)
            self->SetSprinting(1);
    }
    return true;
}

} // namespace SP
