// Slice s00c47e20 -- SP::cSPMission::PickPlanet (PDB name SP::cSPMission::PickPlanet).
//
// Pick the planet a mission points at.  The function reads the targeting properties out of the
// mission property list (minimum range 0x17706bf8, maximum range 0x1f707856, jump limit
// 0x1f707858 and the three boolean gates 0x1f707860 / 0x1f707859 / 0x1f707857), fills a
// star-generation preference record from them, asks the star manager for the candidate planet
// records and then scans that list twice (a strict pass and a relaxed pass).  Each candidate is
// filtered by its flag bits and by the three 32-bit key masks the caller passes in, and every
// record it owns is scored through the mission's virtual slot 0x184 and the squared distance to
// the mission planet.  The winning record is returned, the candidate list is released.
//
// Subject object: SP::cSPMission, the target planet at +0x9c and the property list at +0x180
// (retail offsets, as in slice s00c47a10).
//
// Flags: /O2 /Ob2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-
#include "types.h"

#define PV(n) virtual void pv##n();

// --------------------------------------------------------------------------- property system
struct PropRec {
    uint8_t  mPad00[0x12];
    uint16_t mType;                                     // +0x12  1 = bool, 9 = int, 13 = float
    float*   AsFloat();                                 // 0x0041ea70
    int32_t* AsInt();                                   // 0x0041e990
    uint8_t* AsBool();                                  // 0x0041e920
};

struct PropertyList {
    PV(0) PV(1) PV(2) PV(3) PV(4) PV(5) PV(6) PV(7) PV(8)
    virtual bool Get(uint32_t id, PropRec** out);       // +0x24
};

// --------------------------------------------------------------------------- planets
struct cPlanetList {                                    // raw record range
    void** mBegin;                                      // +0x00
    void** mEnd;                                        // +0x04
};

struct cPlanetRecord;

struct cStarRecord {
    cPlanetRecord* FUN_00b8de30();                      // 0x00b8de30
};

struct cPlanet {
    uint8_t      mPad00[0x13c];
    cStarRecord* mField13C;                             // +0x13c
};

struct cPlanetRecord {
    virtual void v0();
    virtual void v1();                                  // +0x04  Release
    uint8_t      mPad08[0x28 - 0x04];
    uint32_t     mMask28;                               // +0x28
    bool         FUN_00bb9af0(uint32_t flag);           // 0x00bb9af0
    uint32_t     FUN_00bb9ae0();                        // 0x00bb9ae0
    bool         FUN_00bb9c90();                        // 0x00bb9c90
    uint32_t     FUN_00b8dab0();                        // 0x00b8dab0
    cPlanetList* FUN_00bba790();                        // 0x00bba790
    float*       FUN_005c65e0();                        // 0x005c65e0  position
};

struct cEmpireKeys {                                    // the caller's key masks
    uint32_t mMask0;                                    // +0x00
    uint32_t mMask1;                                    // +0x04
    uint32_t mMask2;                                    // +0x08
};

struct GenPrefs {                                       // star-generation preferences
    uint32_t mFlags;                                    // +0x00
    uint32_t mKeyMask;                                  // +0x04
    uint32_t mPad08;                                    // +0x08
    float    mMinRange;                                 // +0x0c
    float    mMaxRange;                                 // +0x10
    float    mRandom;                                   // +0x14
    int32_t  mNumJumps;                                 // +0x18
};

// 0x14 bytes: the star manager's planet range (same shape as SP::cPlanetRange in slice
// s00c47a10).  This function only ever touches the first three fields, but the retail frame
// reserves the whole object between the preference record and the mission position.
struct PlanetVector {
    cPlanetRecord** mBegin;                             // +0x00
    cPlanetRecord** mEnd;                               // +0x04
    cPlanetRecord** mCapacity;                          // +0x08
    void*           mAllocator;                         // +0x0c
    void*           mField10;                           // +0x10
};

struct StarManager {
    void FindPlanets(const float* pos, const GenPrefs* prefs, PlanetVector* out); // 0x00bb1080
    void FUN_00bb4af0(cPlanetRecord* record, cEmpireKeys* keys);                 // 0x00bb4af0
};

struct GameRandom {
    float FUN_00ffbfc0();                               // 0x00ffbfc0
};

struct RandomLinearCongruential {
    uint32_t RandomUint32Uniform(uint32_t range);       // 0x00a68fb0
};

extern RandomLinearCongruential g_Random;               // 0x01601760
extern const float g_FloatOne;                          // 0x0146d130  1.0f
extern const float g_FloatInit;                         // 0x013eb1bc  initial score / range
extern const float g_FloatDist;                         // 0x0146d134  initial distance

StarManager*   GetStarManager();                        // 0x00b3d2a0
GameRandom*    FUN_00ffbe50();                          // 0x00ffbe50
cPlanetRecord* GetCapitalPlanet();                      // 0x01021370
void           FUN_01021300();                          // 0x01021300
void           VectorFree(void* begin);                 // 0x00f47380

// --------------------------------------------------------------------------- helpers
// One bit of a 32-bit key mask.  Retail masks the index at the call site and again inside the
// shift, which is why the compiled code carries two "and ..., 0x1f" per test.
static uint32_t KeyBit(uint32_t index)
{
    return 1u << (index & 0x1f);
}

// --------------------------------------------------------------------------- cSPMission
struct cSPMission {
    PV(0) PV(1) PV(2) PV(3) PV(4) PV(5) PV(6) PV(7) PV(8) PV(9)
    PV(10) PV(11) PV(12) PV(13) PV(14) PV(15) PV(16) PV(17) PV(18) PV(19)
    PV(20) PV(21) PV(22) PV(23) PV(24) PV(25) PV(26) PV(27) PV(28) PV(29)
    PV(30) PV(31) PV(32) PV(33) PV(34) PV(35) PV(36) PV(37) PV(38) PV(39)
    PV(40) PV(41) PV(42) PV(43) PV(44) PV(45) PV(46) PV(47) PV(48) PV(49)
    PV(50) PV(51) PV(52) PV(53) PV(54) PV(55) PV(56) PV(57) PV(58) PV(59)
    PV(60) PV(61) PV(62) PV(63) PV(64) PV(65) PV(66) PV(67) PV(68) PV(69)
    PV(70) PV(71) PV(72) PV(73) PV(74) PV(75) PV(76) PV(77) PV(78) PV(79)
    PV(80) PV(81) PV(82) PV(83) PV(84) PV(85) PV(86) PV(87) PV(88) PV(89)
    PV(90) PV(91) PV(92) PV(93) PV(94) PV(95) PV(96)
    virtual float pv97(void* record, void* out);        // +0x184  candidate score

    uint8_t mPad04[0x9c - 0x04];
    cPlanet* mPlanet9C;                                 // +0x9c  mission planet
    uint8_t mPadA0[0x180 - 0xa0];
    void*   mProps180;                                  // +0x180 mission property list

    cPlanetRecord* PickPlanet(cEmpireKeys* keys);       // 0x00c47e20
};

// --------------------------------------------------------------------------- 0x00c47e20
// @ 0x00c47e20
cPlanetRecord* cSPMission::PickPlanet(cEmpireKeys* keys)
{
    if (keys == 0) {
        return 0;
    }
    FUN_01021300();
    cPlanetRecord* holder = ((cPlanet*)mPlanet9C)->mField13C->FUN_00b8de30();
    float* holderPos = holder->FUN_005c65e0();
    float pos[3];
    pos[0] = holderPos[0];
    pos[1] = holderPos[1];
    pos[2] = holderPos[2];

    float minRange = 0.0f;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x17706bf8u, &rec) && rec->mType == 13) {
                minRange = *rec->AsFloat();
            }
        }
    }

    float maxRange = 0.0f;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x1f707856u, &rec) && rec->mType == 13) {
                maxRange = *rec->AsFloat();
            }
        }
    }

    int32_t numJumps = 0;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x1f707858u, &rec) && rec->mType == 9) {
                numJumps = *rec->AsInt();
            }
        }
    }

    // Three byte gates read from the mission properties.  Only the last one selects the picking
    // rule; the middle byte's slot is what the scoring virtual writes its second argument to.
    uint8_t flags[3];
    flags[2] = 0;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x1f707860u, &rec) && rec->mType == 1) {
                flags[2] = *rec->AsBool();
            }
        }
    }
    flags[1] = 0;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x1f707859u, &rec) && rec->mType == 1) {
                flags[1] = *rec->AsBool();
            }
        }
    }
    flags[0] = 0;
    {
        PropertyList* props = (PropertyList*)mProps180;
        if (props != 0) {
            PropRec* rec;
            if (props->Get(0x1f707857u, &rec) && rec->mType == 1) {
                flags[0] = *rec->AsBool();
            }
        }
    }

    // Candidate-scan state.  The retail frame lays these ten slots out in exactly this order
    // (lowest address first), which is why they are declared together at function scope.
    float bestScore;
    cPlanetRecord* chosen;
    cPlanetRecord* best;
    int32_t examined;
    float bestDist;
    int32_t idx;
    float score;
    float dist2;
    int32_t i;
    int32_t pass;

    bestDist = g_FloatDist;
    best = 0;
    bestScore = g_FloatInit;
    StarManager* starMgr = GetStarManager();
    cPlanetRecord* capital = GetCapitalPlanet();

    GenPrefs prefs;
    prefs.mMinRange = g_FloatInit;
    prefs.mMaxRange = g_FloatInit;
    prefs.mRandom = g_FloatInit;
    prefs.mPad08 = 0;
    prefs.mNumJumps = 0;
    prefs.mFlags = 0x1ff1;
    prefs.mKeyMask = keys->mMask1;
    if (minRange >= 0.0f) {
        prefs.mMinRange = minRange;
    }
    if (maxRange > 0.0f) {
        prefs.mMaxRange = maxRange;
    }
    prefs.mRandom = FUN_00ffbe50()->FUN_00ffbfc0();
    prefs.mNumJumps = numJumps;

    PlanetVector vec;
    vec.mBegin = 0;
    vec.mEnd = 0;
    vec.mCapacity = 0;
    GetStarManager()->FindPlanets(pos, &prefs, &vec);

    int32_t count = (int32_t)(vec.mEnd - vec.mBegin);
    chosen = 0;
    examined = 0;
    dist2 = 0.0f;
    for (pass = 0; pass < 2; ++pass) {
        if (chosen != 0) {
            break;
        }
        idx = (int32_t)g_Random.RandomUint32Uniform((uint32_t)count);
        for (i = 0; i < count; idx = (idx + 1) % count, ++i) {
            if (chosen != 0) {
                break;
            }
            if (examined >= count) {
                break;
            }
            cPlanetRecord* cand = vec.mBegin[idx];
            if (cand->FUN_00bb9af0(1)) {
                continue;
            }
            if (cand->FUN_00bb9af0(0x40000)) {
                continue;
            }
            if (cand->FUN_00bb9af0(0x20000)) {
                continue;
            }
            if ((keys->mMask1 & KeyBit(cand->FUN_00bb9ae0() & 0x1f)) == 0) {
                continue;
            }
            if (cand == holder) {
                continue;
            }
            {
                float* candPos = cand->FUN_005c65e0();
                float dx = pos[0] - candPos[0];
                float dy = pos[1] - candPos[1];
                float dz = pos[2] - candPos[2];
                dist2 = dy * dy + dz * dz + dx * dx;
            }
            switch (pass) {
            case 0:
                if (!cand->FUN_00bb9c90()) {
                    continue;
                }
                ++examined;
                break;
            case 1:
                if (cand->FUN_00bb9c90()) {
                    continue;
                }
                starMgr->FUN_00bb4af0(cand, keys);
                break;
            }
            {
                cPlanetList* list = cand->FUN_00bba790();
                int32_t n = (int32_t)(list->mEnd - list->mBegin);
                for (int32_t j = 0; j < n; ++j) {
                    if (chosen != 0) {
                        break;
                    }
                    cPlanetRecord* rec = (cPlanetRecord*)list->mBegin[j];
                    if (rec == capital) {
                        continue;
                    }
                    if ((keys->mMask0 & KeyBit(rec->mMask28 & 0x1f)) == 0) {
                        continue;
                    }
                    if ((keys->mMask2 & KeyBit(rec->FUN_00b8dab0() & 0x1f)) == 0) {
                        continue;
                    }
                    score = pv97(rec, &flags[1]);
                    if (score <= 0.0f) {
                        continue;
                    }
                    if (flags[0] == 0) {
                        if (score >= g_FloatOne) {
                            chosen = rec;
                            continue;
                        }
                        if (score > bestScore) {
                            bestScore = score;
                            best = rec;
                        }
                    } else if (score == bestScore) {
                        if (dist2 < bestDist) {
                            bestScore = score;
                            bestDist = dist2;
                            best = rec;
                        }
                    } else if (score > bestScore) {
                        bestScore = score;
                        bestDist = dist2;
                        best = rec;
                    }
                }
            }
        }
    }
    if (chosen == 0) {
        chosen = best;
    }
    cPlanetRecord** it = vec.mBegin;
    cPlanetRecord** const end = vec.mEnd;           // retail caches the bound in edi
    while (it < end) {
        if (*it != 0) {
            (*it)->v1();
        }
        ++it;
    }
    if (vec.mBegin != 0 && ((int32_t*)vec.mBegin)[-1] != 0) {
        VectorFree(vec.mBegin);
    }
    return chosen;
}
