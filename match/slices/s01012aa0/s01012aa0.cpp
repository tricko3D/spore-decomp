// @ 0x01012aa0 SP::cSimulatorUniverse::StartPirateRaidPlunder
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

struct cSPMission;

// A mission as seen through the generic ref-counted mission interface.
struct IMission {
    virtual void AddRef();                                      // +0x00
    virtual void Release();                                     // +0x04
    virtual void v08();                                         // +0x08
    virtual cSPMission* GetSPMission(uint32_t nKey);            // +0x0c
    void FUN_00c48e50(uint32_t nEvent);                         // 0x00c48e50
};

// The concrete mission sub-object used by the raid events.
struct cSPMission {
    virtual void v00();                                         // +0x00
    virtual void v04();                                         // +0x04
    uint8_t  mPad08[0x1f8];                                     // +0x08
    uint32_t mMissionID_200;                                    // +0x200
    uint32_t mElapsedTimeMS_204;                                // +0x204
    int  GenerateBomberCount();                                 // 0x00c5b530
    int  GeneratePirateCount();                                 // 0x00c5b510
    void SetTarget(uint32_t nTarget);                           // 0x00c47180
    void Accept();                                              // 0x00c485b0
};

struct cSPMissionManager {
    IMission* CreateMissionFromID(uint32_t nMissionID, uint32_t nArg2,
                                  uint32_t nArg3, uint32_t nArg4);  // 0x00fec590
};

struct cSpaceTokenTranslator {
    uint8_t mPad00[0x0c];                                       // +0x00
    void*   mpMission_0c;                                       // +0x0c
};

struct cEventLog {
    uint32_t PostFeedbackEvent();                               // 0x00dd8640
};

extern cSpaceTokenTranslator* g_pSpaceTokenTranslator;          // 0x016e0d08

cSPMissionManager* GetMissionManager();                         // 0x00feb9f0
cEventLog* __stdcall EventLog(uint32_t nA, uint32_t nB, uint32_t nC,
                              uint32_t nD, uint32_t nE, uint32_t nF);  // 0x00b3d3e0

struct cSimulatorUniverse {
    bool FUN_01012aa0(void* pEmpire, uint32_t nRaidID);         // 0x01012aa0
};

// @ 0x01012aa0
bool cSimulatorUniverse::FUN_01012aa0(void* pEmpire, uint32_t nRaidID)
{
    cSPMissionManager* pManager = GetMissionManager();
    if (pManager == 0)
        return false;
    if (nRaidID == 0)
        return true;

    IMission* pMission = pManager->CreateMissionFromID(0xc4956aa7, (uint32_t)pEmpire,
                                                       nRaidID, 0);
    if (pMission == 0)
        return true;

    cSPMission* pEvent = pMission->GetSPMission(0x397bff2);
    if (pEvent == 0)
        return true;

    int nBombers = pEvent->GenerateBomberCount();
    int nPirates = pEvent->GeneratePirateCount();
    pEvent->mMissionID_200 = (uint32_t)nPirates;
    pEvent->mElapsedTimeMS_204 = (uint32_t)nBombers;
    pEvent->SetTarget(nRaidID);
    pEvent->Accept();

    g_pSpaceTokenTranslator->mpMission_0c = (void*)nRaidID;

    cEventLog* pLog = EventLog(0x31df9ac0, 0x131a9f54, 0, 0, 1, 0);
    uint32_t nEvent = pLog->PostFeedbackEvent();
    pMission->FUN_00c48e50(nEvent);
    return true;
}
