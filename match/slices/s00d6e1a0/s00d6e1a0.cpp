// Slice s00d6e1a0 -- predicate for the "use interactable object" creature action:
// cast the noun's object-database entry to a spatial object and to an interactable,
// gate on visibility/distance/state, then post the stimulus that drives it.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

// ---------------------------------------------------------------------------
// callees: referenced by address and called through the convention the retail
// call sites set up (ecx for a first argument = thiscall). Call targets are
// masked by the checker, so only the argument setup has to match.
// ---------------------------------------------------------------------------
typedef void* (__thiscall *VfnCast)(void*, unsigned int);
typedef void* (__thiscall *ThisGetFn)(void*);
typedef char  (__thiscall *CreatureCheckFn)(void*, void*, int, int);
typedef void* (__cdecl    *StdFn1)(void*);
typedef void* (__thiscall *StimulusSetFn)(void*, unsigned __int64, float, void*);

void FUN_00c13c60();                    // 0x00c13c60  creature gate (thiscall, 3 args)
void FUN_00c04590();                    // 0x00c04590  getter
void FUN_00c6acc0();                    // 0x00c6acc0  CreatureTuning::GetPosition
void FUN_00c9f060();                    // 0x00c9f060  owner/interface getter
void FUN_00bc97f0();                    // 0x00bc97f0  behavior_stimuli::Set

extern float g_13ec4d0;                 // 0x013ec4d0  minimum squared distance
extern float g_1473c70;                 // 0x01473c70  stimulus strength

// ---------------------------------------------------------------------------
// layout (offsets from the retail instruction diff)
// ---------------------------------------------------------------------------
struct NounRef                            // first parameter
{
    char  _p000[4];
    bool  mEnabled;                     // +0x004
    char  _p005[3];
    void* mpObjectDb;                   // +0x008
};

struct SpatialObject                      // Cast(0x1186577)
{
    char  _p000[0x70];
    bool  mReady;                       // +0x070
};

struct Interactable                       // Cast(0x398420d)
{
    char  _p000[0x108];
    void* mpActor;                      // +0x108
    char  _p10c[4];
    int   mState;                       // +0x110
};

struct Creature
{
    char  _p000[0xb4c];
    void* mpTree;                       // +0xb4c
};

struct Stimulus
{
    char _p000[0xc];
    int  mKind;                         // +0x00c
};

// ===========================================================================
// @ 0x00d6e1a0
// ===========================================================================
bool __cdecl Interactable_Activate(NounRef* pNoun, Creature* pCreature)
{
    void* pDb = pNoun->mpObjectDb;
    SpatialObject* pTarget = pDb
        ? (SpatialObject*)((VfnCast)(*(void***) pDb)[3])(pDb, 0x1186577)
        : 0;
    void* pDb2 = pNoun->mpObjectDb;
    Interactable* pInteractable = pDb2
        ? (Interactable*)((VfnCast)(*(void***) pDb2)[3])(pDb2, 0x398420d)
        : 0;

    if (pTarget == 0 || !pNoun->mEnabled || !pTarget->mReady)
        return false;

    if (!((CreatureCheckFn) FUN_00c13c60)(pCreature, pTarget, 0, 1))
        return false;

    if (pInteractable != 0 && pInteractable->mpActor != 0 && pInteractable->mpActor != pCreature)
        return false;

    void* pOwner = ((ThisGetFn) FUN_00c04590)(pCreature);
    float* pTargetPos = (float*)((ThisGetFn)(*(void***) pTarget)[0xb])(pTarget);
    float* pOwnerPos = (float*)((ThisGetFn) FUN_00c6acc0)(pOwner);

    float dx = pOwnerPos[0] - pTargetPos[0];
    float dy = pOwnerPos[1] - pTargetPos[1];
    float dz = pOwnerPos[2] - pTargetPos[2];
    float distSq = dz * dz + dy * dy + dx * dx;

    if (g_13ec4d0 > distSq)
        return false;

    if (pInteractable != 0 && (pInteractable->mState == 1 || pInteractable->mState == 3))
        return false;

    void* pUser = ((StdFn1) FUN_00c9f060)(pInteractable);
    Stimulus* pStimulus = (Stimulus*)((StimulusSetFn) FUN_00bc97f0)(
        (char*) pCreature->mpTree + 8, 0x4000000000000ULL, g_1473c70, pUser);

    if (pStimulus == 0)
        return false;

    pStimulus->mKind = 4;
    return true;
}
