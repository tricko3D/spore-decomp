// SP::cSPSimulatorSpaceGame::ShutdownForGame
//
// 0x01009820 -- tear down the space-game simulator: unregister from the
// SimTicker, drop the messaging handler, shut down the sub-managers and
// release every refcounted member.
//
// Layout note: this is the same class as s01008910 / s01009470, where the
// NPC-UFO SPVector sits at this+0x78; the remaining member offsets below are
// taken from this build's disassembly (the shipped PDB places the class
// differently).  Helper callees keep their retail address in a comment.
#include "types.h"

#define nullptr 0

// ---------------------------------------------------------------------------
// EA::RefCountVTemplate<int> -- AddRef/Release plus the per-subsystem
// Destroy/Unregister slots used by the space-game members.
// ---------------------------------------------------------------------------
class IRefCount {
public:
    virtual void AddRef();          // +0x00
    virtual void Release();         // +0x04
    virtual void Destroy();         // +0x08
    virtual void Unregister();      // +0x0c

    void Cleanup54();               // 0x00ffa2c0
    void CleanupC0();               // 0x01015130
    void Cleanup20();               // 0x00d12fc0
    void CleanupB4();               // 0x01001110
    void Shutdown18();              // 0x010762d0
    void Shutdown14();              // 0x01071570
};

class IMissionManager : public IRefCount {
public:
    void ShutdownForGame();         // 0x00fef5d0
};

class IWalkAround : public IRefCount {
public:
    void ShutdownForGame();         // 0x010168c0
};

class cSpaceStageUniverse {
public:
    void Shutdown();                // 0x00fe71c0
};

// SimTicker singleton: the tracked objects are added/removed through vtable
// slot +0x54.
class ISimTicker {
public:
    virtual void V00();             // +0x00
    virtual void V04();             // +0x04
    virtual void V08();             // +0x08
    virtual void V0c();             // +0x0c
    virtual void V10();             // +0x10
    virtual void V14();             // +0x14
    virtual void V18();             // +0x18
    virtual void V1c();             // +0x1c
    virtual void V20();             // +0x20
    virtual void V24();             // +0x24
    virtual void V28();             // +0x28
    virtual void V2c();             // +0x2c
    virtual void V30();             // +0x30
    virtual void V34();             // +0x34
    virtual void V38();             // +0x38
    virtual void V3c();             // +0x3c
    virtual void V40();             // +0x40
    virtual void V44();             // +0x44
    virtual void V48();             // +0x48
    virtual void V4c();             // +0x4c
    virtual void V50();             // +0x50
    virtual void AddObject(void* pObject);  // +0x54
};

ISimTicker* __cdecl SimTicker();    // 0x00b3d330

// Lazily created 0x40-byte helper object registered with the SimTicker.
class CHelper {
public:
    void* operator new(size_t nSize, const char* pSite,
                       int nA, int nB, int nC, int nD);   // 0x00f473a0
    CHelper();                      // 0x00ae6a70
    void Initialize();              // 0x00ae6570

private:
    uint8_t mData[0x40];
};

extern CHelper* g_pHelper;              // 0x0167a690
extern const char g_newSiteName[];      // 0x0145f924

// Lazily created helper, as the original inlined it at the one call site.
static inline CHelper* GetOrCreateHelper()
{
    if (g_pHelper == nullptr)
        g_pHelper = new (g_newSiteName, 0, 0, 0, 0) CHelper;
    return g_pHelper;
}

void __cdecl EA_Messaging_RemoveHandler(void* pServer, uint32_t nHandler,
                                        uint32_t nIds, uint32_t nCount,
                                        uint32_t nPriority);    // 0x00571db0

// Smart-pointer resets, inlined exactly as the retail build does them.
static inline void Reset4(IRefCount*& rpObject)
{
    IRefCount* pOld = rpObject;
    if (pOld != nullptr) {
        rpObject = nullptr;
        pOld->Release();
    }
}

static inline void Reset8(IRefCount*& rpObject)
{
    IRefCount* pOld = rpObject;
    if (pOld != nullptr) {
        rpObject = nullptr;
        pOld->Destroy();
    }
}

static inline void ResetC(IRefCount*& rpObject)
{
    IRefCount* pOld = rpObject;
    if (pOld != nullptr) {
        rpObject = nullptr;
        pOld->Unregister();
    }
}

namespace SP {

class cSPSimulatorSpaceGame {
public:
    void ShutdownForGame();                                         // 0x01009820
    void DestroyNPC_UFOs(char bParam2, char bParam3, int32_t nFlags); // 0x01009470

private:
    uint8_t mPad00[0x14];               // +0x000
    IRefCount* mpRef14;                 // +0x014
    IRefCount* mpRef18;                 // +0x018
    uint8_t mPad1c[0x04];               // +0x01c
    IRefCount* mpRef20;                 // +0x020
    uint8_t mPad24[0x04];               // +0x024
    IRefCount* mpRef28;                 // +0x028
    uint8_t mPad2c[0x04];               // +0x02c
    cSpaceStageUniverse* mpUniverse;    // +0x030
    uint8_t mPad34[0x0c];               // +0x034
    IRefCount* mpRef40;                 // +0x040
    IRefCount* mpRef44;                 // +0x044
    uint8_t mPad48[0x0c];               // +0x048
    IRefCount* mpRef54;                 // +0x054
    void* mpMessageServer;              // +0x058
    uint32_t mnMessageHandler;          // +0x05c
    uint32_t mnMessageIds;              // +0x060
    uint32_t mnMessageCount;            // +0x064
    uint32_t mnMessagePriority;         // +0x068
    uint8_t mPad6c[0x04];               // +0x06c
    IRefCount* mpTicker70;              // +0x070
    uint8_t mPad74[0x04];               // +0x074
    uint8_t mNPC_UFOs[0x0c];            // +0x078 SPVector<AutoRefCount>
    uint8_t mPad84[0x30];               // +0x084
    IRefCount* mpRefB4;                 // +0x0b4
    IMissionManager* mpMissionManager;  // +0x0b8
    IWalkAround* mpWalkAround;          // +0x0bc
    IRefCount* mpRefC0;                 // +0x0c0
};

} // namespace SP

// ---------------------------------------------------------------------------
// 0x01009820
// ---------------------------------------------------------------------------
// @ 0x01009820
void SP::cSPSimulatorSpaceGame::ShutdownForGame()
{
    SimTicker()->AddObject(this);

    if (mpMessageServer != nullptr) {
        void* pServer = mpMessageServer;
        mpMessageServer = nullptr;
        EA_Messaging_RemoveHandler(pServer, mnMessageHandler,
                                   mnMessageIds, mnMessageCount,
                                   mnMessagePriority);
    }

    Reset4(mpRef28);
    Reset4(mpRef40);

    if (mpRef54 != nullptr) {
        mpRef54->Cleanup54();
        Reset8(mpRef54);
    }

    if (mpRefC0 != nullptr) {
        mpRefC0->CleanupC0();
        Reset8(mpRefC0);
    }

    if (mpMissionManager != nullptr) {
        mpMissionManager->ShutdownForGame();
        IRefCount* pMission = mpMissionManager;
        if (pMission != nullptr) {
            mpMissionManager = nullptr;
            pMission->Destroy();
        }
    }

    if (mpWalkAround != nullptr) {
        mpWalkAround->ShutdownForGame();
        IRefCount* pWalkAround = mpWalkAround;
        if (pWalkAround != nullptr) {
            mpWalkAround = nullptr;
            pWalkAround->Destroy();
        }
    }

    if (mpRef20 != nullptr) {
        mpRef20->Cleanup20();
        ResetC(mpRef20);
    }

    mpUniverse->Shutdown();

    DestroyNPC_UFOs(0, 0, 1);

    if (mpRefB4 != nullptr) {
        mpRefB4->CleanupB4();
        Reset8(mpRefB4);
    }

    if (mpTicker70 != nullptr) {
        SimTicker()->AddObject(mpTicker70);
        Reset8(mpTicker70);
    }

    CHelper* pHelper = GetOrCreateHelper();
    pHelper->Initialize();
    SimTicker()->AddObject(pHelper);

    if (mpRef18 != nullptr) {
        mpRef18->Shutdown18();
        Reset4(mpRef18);
    }

    if (mpRef14 != nullptr) {
        mpRef14->Shutdown14();
        Reset4(mpRef14);
    }

    if (mpRef44 != nullptr)
        Reset8(mpRef44);
}
