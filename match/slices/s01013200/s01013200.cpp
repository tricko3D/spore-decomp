// Pick one planet for a universe event: walk the space game's planet array,
// drop planets the exclusion predicate rejects, drop the home planet unless it
// wins the owner-value comparison, keep the rest in a temporary reference list,
// pick one index with the global LCG and let the caller's selector consume it.
#include "game/types.h"

typedef unsigned int u32;
typedef int s32;

struct cGameData_s0101_3200;

// 0x00c8ba00
bool __cdecl IsExcludedPlanet_s0101_3200(cGameData_s0101_3200* p);
// 0x01021370
cGameData_s0101_3200* __cdecl GetPlayerHomePlanet_s0101_3200();
// 0x0107bcb0
struct cUniverseContext_s0101_3200;
cUniverseContext_s0101_3200* __cdecl GetUniverseContext_s0101_3200();
// 0x00c702f0
s32 __cdecl GetHomePlanetValue_s0101_3200(cGameData_s0101_3200* p);
// 0x01002bd0
void* __cdecl SpaceGameGet_s0101_3200();
// 0x00f47380
void __cdecl operator_delete_array_s0101_3200(void* p);

// 0x01601760
struct RandomLCG_s0101_3200 {
    u32 __thiscall RandomUint32Uniform(u32 range);  // 0x00a68fb0
};
extern RandomLCG_s0101_3200 g_sMathRandom;

// 0x0100fb00: global selection filter (thiscall on 0x016dc798)
struct PlanetFilter_s0101_3200 {
    bool Accept(cGameData_s0101_3200* p);
};
extern PlanetFilter_s0101_3200* g_planetFilter;

// The selector the caller hands in; slot 1 consumes the picked planet and
// returns the caller's result.
struct UniverseSelector_s0101_3200 {
    virtual void Slot0();
    virtual void* Choose(cGameData_s0101_3200* planet, void* dest);
};

struct cGameData_s0101_3200 {
    virtual void AddRef();   // slot 0
    virtual void Release();  // slot 1
};

struct cUniverseContext_s0101_3200 {
    s32 GetOwnerValue();  // 0x00967e70
};

// The planet array hanging off the space game's +0x30 object; read through a
// pointer so the begin pointer is reloaded on every iteration (the predicates
// may rebuild it).
struct PlanetArray_s0101_3200 {
    cGameData_s0101_3200** mpBegin;
    cGameData_s0101_3200** mpEnd;
};

// One counted reference held by the temporary candidate list.
struct PlanetRef_s0101_3200 {
    cGameData_s0101_3200* mp;
};

struct CandidateList_s0101_3200 {
    PlanetRef_s0101_3200* mpBegin;
    PlanetRef_s0101_3200* mpEnd;
    PlanetRef_s0101_3200* mpCap;

    // releases every reference and the element storage; written inline so it is
    // expanded at each of the two return points. The caller passes the running
    // end it kept while filling the list.
    __forceinline void Destroy(PlanetRef_s0101_3200* pEnd)
    {
        for (PlanetRef_s0101_3200* it = mpBegin; it < pEnd; ++it) {
            cGameData_s0101_3200* p = it->mp;
            if (p)
                p->Release();
        }
        if (mpBegin && ((u32*)mpBegin)[-1] != 0)
            operator_delete_array_s0101_3200(mpBegin);
    }

    // 0x00aea5d0
    void Insert(PlanetRef_s0101_3200* pos, PlanetRef_s0101_3200* value);
};

class cSPLivingUniverse_s0101_3200 {
public:
    void* FUN_01013200(UniverseSelector_s0101_3200* selector, void* dest);

private:
    u32 mField00;
    u32 mField04;
    u32 mField08;
    u32 mField0C;
};

// @ 0x01013200
void* cSPLivingUniverse_s0101_3200::FUN_01013200(UniverseSelector_s0101_3200* selector,
                                                 void* dest)
{
    s32 i;
    cGameData_s0101_3200* home;
    PlanetRef_s0101_3200 temp;
    CandidateList_s0101_3200 candidates;

    mField08 = 0;
    mField0C = 0;

    char* pGame = (char*)SpaceGameGet_s0101_3200();
    PlanetArray_s0101_3200& planets =
        *(PlanetArray_s0101_3200*)(*(char**)(pGame + 0x30) + 0x2c);

    home = GetPlayerHomePlanet_s0101_3200();

    // running end of the temporary reference list
    PlanetRef_s0101_3200* pEnd = 0;
    candidates.mpBegin = 0;
    candidates.mpEnd = pEnd;
    candidates.mpCap = 0;

    s32 count = (s32)(planets.mpEnd - planets.mpBegin);

    for (i = 0; i < count; ++i) {
        cGameData_s0101_3200* p = planets.mpBegin[i];
        if (IsExcludedPlanet_s0101_3200(p))
            continue;
        if (p == home) {
            s32 owner = GetUniverseContext_s0101_3200()->GetOwnerValue();
            if (GetHomePlanetValue_s0101_3200(home) <= owner)
                continue;
        }
        if (!g_planetFilter->Accept(p))
            continue;

        temp.mp = p;
        if (temp.mp)
            temp.mp->AddRef();
        if (pEnd < candidates.mpCap) {
            PlanetRef_s0101_3200* slot = pEnd++;
            candidates.mpEnd = pEnd;
            if (slot) {
                slot->mp = p;
                if (slot->mp)
                    slot->mp->AddRef();
            }
        } else {
            candidates.Insert(pEnd, &temp);
            pEnd = candidates.mpEnd;
        }
        if (temp.mp)
            temp.mp->Release();
    }

    if (candidates.mpBegin != pEnd) {
        u32 index = g_sMathRandom.RandomUint32Uniform(
            (u32)(pEnd - candidates.mpBegin));
        cGameData_s0101_3200* picked = candidates.mpBegin[index].mp;
        if (picked) {
            void* result = selector->Choose(picked, dest);
            candidates.Destroy(pEnd);
            return result;
        }
    }

    candidates.Destroy(pEnd);
    return 0;
}
