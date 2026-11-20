// Slice s00ce3780 -- SP planner step: accumulate food toward the plan's desired
// food level, restart the plan's food cue, then refresh the plan timer.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"
#include "sse_helpers.h"

namespace SP {

// COM-style sub-interface: only its vtable pointer is used here.
struct cSubInterface {
    void* mpVtable;                                          // +0x00
};

// SP::cTribePlanData as seen by the planner step functions (retail size 0x58).
struct cPlanTableEntry {
    uint32_t mID;                                            // +0x00
    uint32_t mState;                                         // +0x04
    uint32_t mFrequency;                                     // +0x08
    float    mCheckTime;                                     // +0x0c
    int32_t  mFoodRequired;                                  // +0x10
    float    mFoodAccumulated;                               // +0x14
    uint32_t mPad18[0x3c / 4];                               // +0x18 .. +0x53
    void*    mpConfiguration;                                // +0x54
};

// SP::cTribePlanConfiguration (only the fields this step touches).
struct cPlanConfiguration {
    uint32_t mPad00[0x1c / 4];                               // +0x00 .. +0x1b
    uint32_t mNumDesiredAdults;                              // +0x1c
    uint32_t mPad20[4];                                      // +0x20 .. +0x2f
    float    mFoodRate;                                      // +0x30
    int32_t  mRequiredFood;                                  // +0x34
    int32_t  mDesiredFood;                                   // +0x38
};

// The planner host's first 0x20c bytes: host vtable, then a sub-interface at
// +0x34.  The host itself carries a further COM sub-interface as a second base
// at +0x20c, so passing it to a call is a derived-to-base conversion.
struct cHostHead {
    void*          mpHostVtbl;                               // +0x00
    uint32_t       mPad04[12];                               // +0x04 .. +0x33
    cSubInterface  mSub;                                     // +0x34
    uint32_t       mPad38[117];                              // +0x38 .. +0x20b
};

struct cPlannerHost : cHostHead, cSubInterface {
};

// Plan unit: its own sub-interface sits at +0xc0.
struct cPlannerUnit {
    uint32_t      mPad00[48];                                // +0x00 .. +0xbf
    cSubInterface mSub;                                      // +0xc0
};

// First parameter block of the planner step functions.
struct cPlanArgs {
    cPlannerHost* mpTribe;                                   // +0x00
    void*         mpPlanner;                                 // +0x04
    uint32_t      mPad08;                                    // +0x08
    uint32_t      mCount;                                    // +0x0c
    uint32_t      mPad10[4];                                 // +0x10 .. +0x1f
    void*         mpTarget;                                  // +0x20
};

// Plan key block passed as the sixth parameter: plan type, step index, timer.
struct cStepState {
    uint32_t mX;                                             // +0x00
    uint32_t mY;                                             // +0x04
    float    mTimeLeft;                                      // +0x08
};

} // namespace SP

extern "C" void  FindPlanEntry();                            // 0x00ce0e60 (thiscall: table, type, index)
extern "C" void* __cdecl GetTribePlanConfiguration(uint32_t uPlanId);      // 0x00ce2d20
extern "C" void* __cdecl GetTribeFromSlot(void* pSlot);                    // 0x00c8ea70
extern "C" float __cdecl GetPropertyFloat(void* pProps, uint32_t uId, float fDef); // 0x004e1c70
extern "C" void* __cdecl GetGlobalValue();                   // 0x00b3d2b0 (no-arg getter)
extern "C" void  GetTribeAudioArg();                         // 0x00c8e990 (thiscall, no args)
extern "C" uint32_t GetTribeIndex();                         // 0x00c8f3a0 (thiscall, no args)
extern "C" uint32_t GetTribeCount();                         // 0x00c8eae0 (thiscall, no args)
extern "C" void  PlayVocal();                                // 0x00ac7ab0 (thiscall, 3 args, void)
extern "C" float PlayVocalSound();                           // 0x00ac7d10 (thiscall, 3 args, float)
extern "C" unsigned PlayUnitVocal();                         // 0x00c1d640 (thiscall, 3 args)
extern "C" uint32_t __cdecl FindFirstEligiblePlanStep(SP::cPlanArgs* pArgs, uint32_t uKey, char bIgnore); // 0x00ce2f00
extern "C" void* __cdecl CreatePlanTimer(SP::cPlanArgs* pArgs, uint32_t uKey, uint32_t uIndex,
                                         uint32_t uKind, void* pTarget);  // 0x00ce0ff0

typedef void*    (__thiscall* FindPlanEntryThunk)(void* pPlanner, uint32_t uType, uint32_t uIndex);
typedef void*    (__thiscall* TribeGetPtrThunk)(void* pTribe);
typedef uint32_t (__thiscall* TribeGetUIntThunk)(void* pTribe);
typedef void     (__thiscall* PlayVocalThunk)(void* pThis, float fValue, void* pArg, void* pAudio);
typedef float    (__thiscall* PlayVocalSoundThunk)(void* pThis, float fValue, void* pArg, unsigned uZero);
typedef unsigned (__thiscall* PlayUnitVocalThunk)(void* pUnit, const char* pCode, int iA, int iB);

// @ 0x00ce3780
bool __cdecl FUN_00ce3780(SP::cPlanArgs* args, int a2, int a3, int a4, uint32_t a5,
                          SP::cStepState* state, int a7, float dt)
{
    // Plan type and step index; the type is also the timer key at the end, so it
    // stays live across the whole function.
    uint32_t uType = state->mX;
    uint32_t uIndex = state->mY;
    SP::cPlannerHost* pTribe = args->mpTribe;
    SP::cPlanTableEntry* pEntry = (SP::cPlanTableEntry*)
        ((FindPlanEntryThunk)FindPlanEntry)(args->mpPlanner, uType, uIndex);
    if (!pEntry)
        return false;
    if (args->mCount <= 1)
        return false;
    if (!pEntry->mpConfiguration)
        pEntry->mpConfiguration = GetTribePlanConfiguration(pEntry->mID);
    SP::cPlanConfiguration* pConfig = (SP::cPlanConfiguration*)pEntry->mpConfiguration;

    if (pConfig->mDesiredFood + pEntry->mFoodRequired
        >= ((int (__thiscall*)(void*))((void**)args->mpTribe->mpHostVtbl)[0xa4 / 4])(args->mpTribe)) {
        void* pHolder = GetTribeFromSlot(*(void**)((char*)pTribe + 0x550));
        float fTuning = GetPropertyFloat(*(void**)((char*)pHolder + 0xb4), 0x1bbd28cfu,
                                         *(const float*)0x01488874);
        const float& rRate = pConfig->mFoodRate > fTuning ? pConfig->mFoodRate : fTuning;
        pEntry->mFoodAccumulated = rRate * (float)args->mCount * dt + pEntry->mFoodAccumulated;
        if (pEntry->mFoodAccumulated > 1.0f) {
            ((PlayVocalThunk)PlayVocal)(
                GetGlobalValue(), pEntry->mFoodAccumulated,
                (void*)(SP::cSubInterface*)pTribe,
                ((TribeGetPtrThunk)GetTribeAudioArg)(pTribe));
            pEntry->mFoodAccumulated = 0.0f;
        }
    }

    uint32_t uAdults = ((TribeGetUIntThunk)GetTribeIndex)(pTribe);
    if (uAdults < pConfig->mNumDesiredAdults && uAdults < ((TribeGetUIntThunk)GetTribeCount)(pTribe)) {
        float fLimit = *(const float*)0x0158133c;
        int iLimit = SseTruncToInt(fLimit);
        if ((int)((int (__thiscall*)(void*))((void**)pTribe->mpHostVtbl)[0xa4 / 4])(pTribe) >= iLimit) {
            if (fLimit > *(const float*)0x0147792c) {
                ((PlayVocalSoundThunk)PlayVocalSound)(
                    GetGlobalValue(), fLimit, (void*)(SP::cSubInterface*)pTribe, 0u);
            }
            SP::cPlannerUnit* pUnit = (SP::cPlannerUnit*)
                ((void* (__thiscall*)(void*, int))((void**)pTribe->mpHostVtbl)[0x98 / 4])(pTribe, 0);
            if (pUnit) {
                SP::cPlannerHost* pSub = (SP::cPlannerHost*)
                    ((void* (__thiscall*)(void*))((void**)pTribe->mpHostVtbl)[0xac / 4])(pTribe);
                SP::cSubInterface* pUnitIface = &pUnit->mSub;
                void** pUnitVtable = (void**)pUnitIface->mpVtable;
                uint32_t uValue = ((uint32_t (__thiscall*)(void*))
                    ((void**)pSub->mSub.mpVtable)[0x2c / 4])(&pSub->mSub);
                ((void (__thiscall*)(void*, uint32_t))pUnitVtable[0x38 / 4])(pUnitIface, uValue);
                ((PlayUnitVocalThunk)PlayUnitVocal)(pUnit, "vcode_baby_born_cry", 2, 0);
            }
        }
    }

    if (a4 & 1) {
        uint32_t uStep = FindFirstEligiblePlanStep(args, uType, 1);
        if (uStep != 0xffffffffu)
            CreatePlanTimer(args, uType, uStep, 3, args->mpTarget);
    }
    return true;
}
