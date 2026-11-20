// @ 0x00d6ded0
//
// Resolves the noun that owns the given ID (the ID comes from the target's
// vtable slot 0x4c), takes that community's leader and measures the distance
// between the two bodies: 1.0f when the leader is closer than two units.
#include "game/types.h"

namespace nSPBehaviorTree {
struct behavior_stimuli {
    uint32_t mStimulusID;    // +0x00
    void*    mField04;       // +0x04 (a behavior object of the leader)
    uint32_t mPad08[8];      // +0x08 .. +0x27
    behavior_stimuli& GetAny();   // 0x00d997e0
};
}

struct cSpatialObject_s00d6_ded0 {
    char mPad[4];
};

struct cBehavior_s00d6_ded0 {
    char mPad[4];
};

// Vtable prefix of a game data object; slot 0x4c returns the noun ID.
struct cGameData_s00d6_ded0 {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48();
    virtual uint32_t GetNounID();                 // +0x4c
};

// The creature/leader: embedded spatial body at +0xc0, its current behavior
// object at +0x5a8, the behavior pointer at +0xe7c and a "busy" flag at +0xb5e.
struct cCreature_s00d6_ded0 : cGameData_s00d6_ded0 {
    uint32_t mPad04[(0xc0 - 0x04) / 4];
    cSpatialObject_s00d6_ded0 mBody;              // +0xc0
    uint32_t mPadC4[(0x5a8 - 0xc4) / 4];
    cBehavior_s00d6_ded0 mBehavior;               // +0x5a8
    char mPad5ac[0xb5e - 0x5ac];                  // +0x5ac (not a multiple of 4)
    uint8_t mbB5e;                                // +0xb5e
    char mPadB5f[0xe7c - 0xb5f];
    cBehavior_s00d6_ded0* mpBehaviorE7c;          // +0xe7c
    cBehavior_s00d6_ded0* GetBehavior();          // 0x00c0ee60
};

struct cNoun_s00d6_ded0 {
    cCreature_s00d6_ded0* GetLeader();            // 0x00c00650
};

struct cNounManager_s00d6_ded0 {
    cNoun_s00d6_ded0* FindNoun(uint32_t nID);     // 0x00b23a40
};

cNounManager_s00d6_ded0* FUN_00b3d300();          // 0x00b3d300

float ComputeBodyDistance(cSpatialObject_s00d6_ded0* pA,
                          cSpatialObject_s00d6_ded0* pB,
                          uint32_t nFlag);        // 0x00d99ac0

float FUN_00d6ded0(cCreature_s00d6_ded0* pSelf, uint32_t, uint32_t, uint32_t,
                   nSPBehaviorTree::behavior_stimuli* pStimulus, bool* pOutFlag)
{
    cNoun_s00d6_ded0* pCommunity = FUN_00b3d300()->FindNoun(pSelf->GetNounID());

    if (pCommunity != 0) {
        cCreature_s00d6_ded0* pLeader = pCommunity->GetLeader();

        if (!pLeader->mbB5e && pLeader->GetBehavior() != &pSelf->mBehavior) {
            if (pStimulus != 0)
                pStimulus->GetAny();
            *pOutFlag = true;
            pStimulus->mField04 = &pLeader->mBehavior;

            if (2.0f > ComputeBodyDistance(&pLeader->mBody, &pSelf->mBody, 1))
                return 1.0f;
        }
    }
    return 0.0f;
}
