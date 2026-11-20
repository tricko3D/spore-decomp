// Creates the generic pressure-event mission (class id 0xa7c92499) for the
// empire that owns the given source object, and appends it to a mission vector.
#include "game/types.h"

namespace {

class cMission;

// The source object the event is created for; owns a civ/avatar which in turn
// knows its empire.
class cSourceObject {
public:
    void* GetOwnerCiv();                        // 0x00b8de30
};

class cOwnerCiv {
public:
    uint32_t GetEmpireID();                     // 0x00b1fdb0
};

class cStarManager {
public:
    void* GetEmpireByID(uint32_t nEmpireID);    // 0x00ba9370
};

// Missions are intrusively reference counted through their vtable.
class cMission {
public:
    virtual void AddRef();                      // vtable +0x0
    virtual void Release();                     // vtable +0x4
    void Initialize();                          // 0x00c485b0
};

class cMissionManager {
public:
    cMission* CreateMissionFromID(uint32_t nMissionID, uint32_t nSource,
                                  void* pEmpire, uint32_t nFlags);  // 0x00fec590
};

cMissionManager* __cdecl GetMissionManager();   // 0x00feb9f0
cStarManager*    __cdecl GetStarManager();      // 0x00b3d2a0

// Owning handle for a mission: copies take a reference, the last owner drops it.
class MissionPtr {
public:
    cMission* mpMission;

    MissionPtr() : mpMission(0) {}
    explicit MissionPtr(cMission* pMission) : mpMission(pMission)
    {
        if (mpMission)
            mpMission->AddRef();
    }

    MissionPtr(const MissionPtr& x) : mpMission(x.mpMission)
    {
        if (mpMission)
            mpMission->AddRef();
    }

    MissionPtr& operator=(const MissionPtr& x)
    {
        cMission* pOld = mpMission;
        mpMission = x.mpMission;
        if (mpMission)
            mpMission->AddRef();
        if (pOld)
            pOld->Release();
        return *this;
    }

    ~MissionPtr()
    {
        if (mpMission)
            mpMission->Release();
    }
};

// Inline vector of mission handles; growing the storage is out of line.
class MissionVector {
public:
    MissionPtr* mpBegin;
    MissionPtr* mpEnd;
    MissionPtr* mpCapacity;

    void DoInsertValue(MissionPtr* pPosition, const MissionPtr& value);  // 0x00aea5d0

    void push_back(const MissionPtr& value)
    {
        if (mpEnd < mpCapacity) {
            MissionPtr* pSlot = mpEnd;
            mpEnd = pSlot + 1;
            if (pSlot) {
                pSlot->mpMission = value.mpMission;
                if (pSlot->mpMission)
                    pSlot->mpMission->AddRef();
            }
        } else {
            DoInsertValue(mpEnd, value);
        }
    }
};

}  // namespace

// @ 0x01012910
bool __stdcall CreatePressureMission(cSourceObject* pSource, MissionVector* pMissions)
{
    if (pSource) {
        cMissionManager* pManager = GetMissionManager();
        uint32_t nEmpireID = ((cOwnerCiv*)pSource->GetOwnerCiv())->GetEmpireID();
        void* pEmpire = GetStarManager()->GetEmpireByID(nEmpireID);

        cMission* pMission = pManager->CreateMissionFromID(0xa7c92499u, (uint32_t)pSource,
                                                           pEmpire, 0);
        pMission->Initialize();

        pMissions->push_back(MissionPtr(pMission));

        return true;
    }
    return false;
}
