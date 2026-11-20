// Plan-step tick: counts the step timer down, accumulates the per-order weight
// and starts the next step of the order once the accumulator passes 1.0.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
// (the timer compares with comiss+ja and the weight max with fcompi, both the
// SSE / fast-math shapes; without /fp:fast cl emits fcompp + fnstsw instead.)
#include "types.h"

namespace SP {

// Plan step record returned by GetPlanData().
struct PlanData_s00ce_3b60 {
    uint32_t muId00;                                          // +0x00
    uint32_t mPad04;                                          // +0x04
    float    mfValue08;                                       // +0x08
    uint32_t mPad0c;                                          // +0x0c
    int32_t  mnValue10;                                       // +0x10
    float    mfValue14;                                       // +0x14
    uint8_t  mPad18[0x3c];                                    // +0x18 .. +0x53
    void*    mpSub54;                                         // +0x54
};

// Shared sub record cached in PlanData::mpSub54.
struct PlanSub_s00ce_3b60 {
    uint32_t mPad00;                                          // +0x00
    uint32_t mPad04;                                          // +0x04
    float    mfValue08;                                       // +0x08
    uint8_t  mPad0c[0x24];                                    // +0x0c .. +0x2f
    float    mfValue30;                                       // +0x30
    uint32_t mPad34;                                          // +0x34
    int32_t  mnValue38;                                       // +0x38
};

struct Info_s00ce_3b60 {
    uint8_t mPad00[0xb4];                                     // +0x00 .. +0xb3
    void*   mpValueB4;                                        // +0xb4
};

// Table of per-order weights owned by the plan reference.
struct FloatTable_s00ce_3b60 {
    uint8_t mPad00[0x2c];                                     // +0x00 .. +0x2b
    float*  mpfData2c;                                        // +0x2c
};

// Step definition passed to the callback (order type, key, timer).
struct StepCtx_s00ce_3b60 {
    uint32_t muOrderType;                                     // +0x00
    uint32_t muKey;                                           // +0x04
    float    mfTimer08;                                       // +0x08
};

// Plan reference: owner object, weight table and the tribe's citizen count.
struct PlanRef_s00ce_3b60 {
    void*                  mpObject;                          // +0x00
    FloatTable_s00ce_3b60* mpTable;                           // +0x04
    uint32_t               mPad08;                            // +0x08
    uint32_t               muCount0c;                         // +0x0c
};

// The object the plan belongs to (tribe / simulation unit).
struct Obj_s00ce_3b60 {
    void**  mpVtable;                                         // +0x00
    uint8_t mPad04[0x11c];                                    // +0x04 .. +0x11f
    void**  mpSubVt120;                                       // +0x120
    uint8_t mPad124[0xe8];                                    // +0x124 .. +0x20b
    uint8_t mValue20c;                                        // +0x20c
    uint8_t mPad20d[0x343];                                   // +0x20d .. +0x54f
    void*   mpValue550;                                       // +0x550
};

} // namespace SP

extern "C" void* GetPlanData();                       // 0x00ce0e60
extern "C" void* Sub_ce2d20();                        // 0x00ce2d20
extern "C" void* Sub_c8ea70();                        // 0x00c8ea70
extern "C" float Sub_4e1c70();                        // 0x004e1c70
extern "C" void  Sub_b3d2b0();                        // 0x00b3d2b0
extern "C" void  Sub_ac7ab0();                        // 0x00ac7ab0
extern "C" void  Sub_c904b0();                        // 0x00c904b0
extern "C" void* Sub_c9cec0();                        // 0x00c9cec0
extern "C" void* Sub_b3d350();                        // 0x00b3d350
extern "C" void  Sub_c8f6e0();                        // 0x00c8f6e0
// Two folded float helpers share 0x00572a10: a two-argument and a three-argument
// form (identical code folding; both call sites are visible below).
extern "C" float Sub_572a10a();                       // 0x00572a10 (float, float)
extern "C" float Sub_572a10b();                       // 0x00572a10 (float, float, float)
extern "C" void  Sub_b81a40();                        // 0x00b81a40
extern "C" void  Sub_b7f190();                        // 0x00b7f190
extern "C" void  Sub_c8f660();                        // 0x00c8f660
extern "C" void  Sub_c998c0();                        // 0x00c998c0
extern "C" void* Sub_b3d240();                        // 0x00b3d240
extern "C" void* Sub_c8e990();                        // 0x00c8e990

extern "C" float kfValue0169b418;                     // 0x0169b418
extern "C" const char kName014fc194[];                // 0x014fc194

// @ 0x00ce3b60
bool __cdecl TickPlanStep(SP::PlanRef_s00ce_3b60* pRef, uint32_t, uint32_t,
                          uint32_t, uint32_t, SP::StepCtx_s00ce_3b60* pCtx,
                          uint32_t, float fDeltaTime)
{
    SP::FloatTable_s00ce_3b60* pTable = pRef->mpTable;
    SP::Obj_s00ce_3b60* pObj = (SP::Obj_s00ce_3b60*)pRef->mpObject;
    uint32_t uOrderType = pCtx->muOrderType;

    SP::PlanData_s00ce_3b60* pPlan = (SP::PlanData_s00ce_3b60*)
        ((void* (__thiscall*)(void*, uint32_t, uint32_t))GetPlanData)(
            pTable, pCtx->muOrderType, pCtx->muKey);
    if (pPlan == 0)
        return false;

    pCtx->mfTimer08 -= fDeltaTime;
    if (pCtx->mfTimer08 > 0.0f)
        return true;
    pCtx->mfTimer08 = 2.0f;

    int nCount = ((int (__thiscall*)(void*))((void**)pObj->mpVtable)[0xa4 / 4])(pObj);

    // Cached sub-record; the accessor is inlined twice below (the second copy
    // still carries its own null check, exactly as in the original).
    if (pPlan->mpSub54 == 0)
        pPlan->mpSub54 = ((void* (__cdecl*)(uint32_t))Sub_ce2d20)(pPlan->muId00);
    SP::PlanSub_s00ce_3b60* pSub = (SP::PlanSub_s00ce_3b60*)pPlan->mpSub54;

    if (pSub->mnValue38 + pPlan->mnValue10 >= nCount) {
        if (pPlan->mpSub54 == 0)
            pPlan->mpSub54 = ((void* (__cdecl*)(uint32_t))Sub_ce2d20)(pPlan->muId00);
        pSub = (SP::PlanSub_s00ce_3b60*)pPlan->mpSub54;

        SP::Info_s00ce_3b60* pInfo =
            (SP::Info_s00ce_3b60*)((void* (__stdcall*)(void*))Sub_c8ea70)(pObj->mpValue550);
        float fSample = ((float (__cdecl*)(void*, uint32_t, float))Sub_4e1c70)(
            pInfo->mpValueB4, 0x1bbd28cfu, 0.1f);
        // 0xce3c45: the larger of the sampled weight and the step threshold.
        // Both arms are lvalues, so the conditional itself is an lvalue and the
        // original picks the address instead of the value (fcompi + two leas).
        const float& rfWeight = (pSub->mfValue30 > fSample) ? pSub->mfValue30 : fSample;

        pPlan->mfValue14 += rfWeight * (float)pRef->muCount0c * fDeltaTime;
        if (pPlan->mfValue14 > 1.0f) {
            void* pValue = ((void* (__stdcall*)(float, void*, void*))Sub_b3d2b0)(
                pPlan->mfValue14, &pObj->mValue20c,
                ((void* (__thiscall*)(void*))Sub_c8e990)(pObj));
            ((void (__thiscall*)(void*))Sub_ac7ab0)(pValue);
            pPlan->mfValue14 = 0.0f;
        }
    }

    if (nCount > pPlan->mnValue10) {
        // The lookup fills in a single out pointer, which the original zeroes
        // first and then passes on to the widget lookup below.
        void* pFound = 0;
        bool bFlag = ((bool (__thiscall*)(void*, void*))Sub_c904b0)(pObj, &pFound);
        if (pFound != 0) {
            uint8_t* pNext = (uint8_t*)((void* (__cdecl*)(void*))Sub_c9cec0)(pFound);
            pPlan->mnValue10 = 0;
            if (!bFlag) {
                pPlan->mnValue10 = *(int32_t*)(pNext + 0x38);
                return true;
            }

            // Three 12-byte scratch blocks are shared by the calls below.
            float fOutA[3];
            float fOutB[3];
            float fOutC[3];

            uint8_t* pCtx2 = (uint8_t*)((void* (__cdecl*)())Sub_b3d350)();
            if (((int (__thiscall*)(void*, void*))Sub_c8f6e0)(pObj, pFound) != 0)
                return false;

            float fA = ((float (__cdecl*)(float, float))Sub_572a10a)(0.0f, kfValue0169b418);
            float fB = ((float (__cdecl*)(float, float, float))Sub_572a10b)(4.0f, 10.0f, fA);
            void* pValue = ((void* (__thiscall*)(void*, float, float))
                            ((void**)pObj->mpSubVt120)[0x2c / 4])(pObj->mpSubVt120, fB, fA);

            ((void (__thiscall*)(void*, void*, void*))Sub_b81a40)(pCtx2, fOutB, pValue);
            ((void (__thiscall*)(void*, void*, void*))Sub_b7f190)(pCtx2, fOutA, fOutC);

            ((void (__thiscall*)(void*, int))Sub_c8f660)(pObj, -*(int32_t*)(pNext + 0x38));

            uint8_t* pWidget = (uint8_t*)((void* (__thiscall*)(void*, void*))
                                          ((void**)pObj->mpVtable)[0xb4 / 4])(pObj, pFound);
            void** pWidgetVt = *(void***)pWidget;
            ((void (__thiscall*)(void*, void*))pWidgetVt[0x64 / 4])(pWidget, pObj);

            uint8_t* pSubObj = pWidget + 0x34;
            void** pSubVt = *(void***)pSubObj;
            ((void (__thiscall*)(void*, void*))pSubVt[0x38 / 4])(pSubObj, fOutA);
            ((void (__thiscall*)(void*, void*))pSubVt[0x3c / 4])(pSubObj, fOutC);

            ((void (__thiscall*)(void*, int))Sub_c998c0)(pObj, 0);

            uint8_t* pTarget = (uint8_t*)((void* (__cdecl*)())Sub_b3d240)();
            void* pValue5c = ((void* (__thiscall*)(void*, void*))
                              pSubVt[0x5c / 4])(pSubObj, fOutB);
            void* pValue2c = ((void* (__thiscall*)(void*))pSubVt[0x2c / 4])(pSubObj);

            ((void (__thiscall*)(void*, const char*, void*, void*))
             (*(void***)pTarget)[0x60 / 4])(pTarget, kName014fc194, pValue5c, pValue2c);
        }
    }

    pTable->mpfData2c[uOrderType] = pSub->mfValue08;
    return false;
}
