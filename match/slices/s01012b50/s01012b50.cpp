// Slice s01012b50 -- SP::cSimulatorUniverse happiness-mission spawn helper.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

struct cGameNounManager;
struct cStarManager;
struct cSPMissionManager;
struct cMissionHappinessEvent;

struct cSPMission {
    virtual void AddRef();                              // +0x00
    virtual void Release();                             // +0x04
    virtual void* v08(unsigned);                        // +0x08
    virtual void* GetSPMission(unsigned type);          // +0x0c
};

// Ref-counted handle the mission vector stores: the constructor takes the
// reference the handle owns, the destructor gives it back.
struct cMissionRef {
    cSPMission* mpObject;
    cMissionRef(cSPMission* p) : mpObject(p)
    {
        if (mpObject != 0)
            mpObject->AddRef();
    }
    ~cMissionRef()
    {
        if (mpObject != 0)
            mpObject->Release();
    }
};

// out of line in the original (0x00aea5d0), __stdcall (ret 8)
void __stdcall DoInsertPtr(void** where, cMissionRef const& value);

struct U32Vec {
    unsigned* mpBegin;                                  // +0x00
    unsigned* mpEnd;                                    // +0x04
    unsigned* mpCapacity;                               // +0x08
    unsigned* mpAllocName;                              // +0x0c
};

struct cNounBase {
    cGameNounManager* BioProtectorSort();               // 0x00b8de30
};

struct cGameNounManager {
    void* GetAvatar();                                  // 0x00b1fdb0
};

struct cStarManager {
    void* GetEmpireByID();                              // 0x00ba9370
};

cStarManager* __stdcall StarManager(void* avatar);      // 0x00b3d2a0

struct cSPMissionManager {
    cSPMission* CreateMissionFromID(unsigned id, void* mission, void* empire,
                                    unsigned flags);    // 0x00fec590
};

struct cMissionHappinessEvent {
    void SetTargetPlanet(void* planet);                 // 0x00c47180
    void Accept();                                      // 0x00c485b0
};

void* __cdecl GetMissionManager();                      // 0x00feb9f0

// @ 0x01012b50
int __stdcall CreateHappinessMission(void* pNoun, U32Vec* accepted)
{
    cSPMissionManager* mgr = (cSPMissionManager*)GetMissionManager();
    if (pNoun == 0)
        return 0;

    cGameNounManager* nouns = ((cNounBase*)pNoun)->BioProtectorSort();
    void* avatar = nouns->GetAvatar();
    cStarManager* stars = StarManager(avatar);
    void* empire = stars->GetEmpireByID();

    cSPMission* mission = mgr->CreateMissionFromID(0xa1396bc8, pNoun, empire, 0);
    if (mission == 0)
        return 0;

    cMissionHappinessEvent* ev =
        (cMissionHappinessEvent*)mission->GetSPMission(0x4f3bde8);
    if (ev == 0)
        return 0;

    ev->SetTargetPlanet(pNoun);
    ev->Accept();

    // The original expands the vector's push_back at the call site: the
    // growing branch is an out-of-line helper, the in-place branch below.
    cMissionRef ref(mission);
    if (accepted->mpEnd < accepted->mpCapacity)
    {
        unsigned* slot = accepted->mpEnd;
        accepted->mpEnd = slot + 1;
        if (slot)
        {
            *slot = (unsigned)mission;
            mission->AddRef();
        }
    }
    else
    {
        DoInsertPtr((void**)accepted->mpEnd, ref);
    }
    return 1;
}
