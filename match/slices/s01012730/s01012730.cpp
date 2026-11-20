// Slice s01012730 -- SP::PirateRaid::Start and the eastl array-copy helpers.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, size_t size);

namespace eastl {

struct cIntArray {
    int* mpBegin;                                            // +0x00
    int* mpEnd;                                              // +0x04
    void Reserve(int count, const void* allocator);          // 0x00b93c60
};

struct cArrayCopySrc {
    int      mKey;                                           // +0x00
    cIntArray mValues;                                       // +0x04
    uint32_t  mPad0c;                                        // +0x0c
    cIntArray mAllocator;                                    // +0x10
};

struct cArrayCopyDst {
    int      mKey;                                           // +0x00
    cIntArray mValues;                                       // +0x04
    cArrayCopyDst* Assign(const cArrayCopySrc& src);
};

// @ 0x01012f30
cArrayCopyDst* cArrayCopyDst::Assign(const cArrayCopySrc& src)
{
    mKey = src.mKey;
    int bytes = (int)((char*)src.mValues.mpEnd - (char*)src.mValues.mpBegin);
    mValues.Reserve(bytes >> 2, &src.mAllocator);
    int* dst = mValues.mpBegin;
    memcpy(dst, src.mValues.mpBegin, (size_t)bytes);
    mValues.mpEnd = dst + (bytes >> 2);
    return this;
}

} // namespace eastl

// ---------------------------------------------------------------------------------------------
// SP::PirateRaid::Start -- create the raid mission for a target noun, initialise its
// cSPMission sub-object (key 0x3960c0a) and hand the mission (ref-counted) to the caller's
// mission list.
// ---------------------------------------------------------------------------------------------

struct cRefObj {
    virtual void AddRef();                                    // +0x00
    virtual void Release();                                   // +0x04
};

struct cSPMission {
    int  FUN_c5b530();                                        // 0x00c5b530
    void Init(int flag);                                      // 0x00c5b370
    void SetTarget(int pNoun);                                // 0x00c47180
    void SetEmpire(int pEmpire);                              // 0x00c5b390
    void Accept();                                            // 0x00c485b0
};

struct IMission : cRefObj {
    virtual void v08();                                       // +0x08
    virtual cSPMission* GetSPMission(int key);                // +0x0c
};

struct cGameNounManager {
    void* GetAvatar();                                        // 0x00b1fdb0
};

struct cGameNounOwner {
    cGameNounManager* GetNounManager();                       // 0x00b8de30
};

struct cStarManager {
    void* GetEmpireByID();                                    // 0x00ba9370
};

cStarManager* __stdcall StarManager(void* pAvatar);           // 0x00b3d2a0

struct cSPMissionManager {
    IMission* CreateMissionFromID(unsigned id, int source, void* empire, int flag);  // 0x00fec590
};

void* GetMissionManager();                                    // 0x00feb9f0

// eastl pointer-vector grow/inplace insert (member, __thiscall: value taken by address)
struct cMissionList;

// Auto-ref-counted pointer holder, the element type of the mission list.
struct AutoRefCount {
    void* mpObject;                                           // +0x00

    AutoRefCount(void* pObject) : mpObject(pObject)
    {
        if (mpObject)
            ((cRefObj*)mpObject)->AddRef();
    }

    ~AutoRefCount()
    {
        if (mpObject)
            ((cRefObj*)mpObject)->Release();
    }
};

// Mission list: begin/end/capacity-end pointer triple (eastl vector layout).
class cMissionList {
public:
    void** mpBegin;                                           // +0x00
    void** mpEnd;                                             // +0x04
    void** mpCapacityEnd;                                     // +0x08

    void push_back(const AutoRefCount& value);
    void DoInsert(void** ppPos, const void** ppValue);         // 0x00aea5d0
};

__forceinline void cMissionList::push_back(const AutoRefCount& value)
{
    if (mpEnd < mpCapacityEnd) {
        void** pSlot = mpEnd;
        mpEnd = pSlot + 1;
        if (pSlot) {
            *pSlot = value.mpObject;
            ((cRefObj*)value.mpObject)->AddRef();
        }
    } else {
        DoInsert(mpEnd, (const void**)&value);
    }
}

struct cPirateRaid {
    int Start(int pNoun, cMissionList* pList);                // 0x01012730  (ret 8)
};

// @ 0x01012730
int cPirateRaid::Start(int pNoun, cMissionList* pList)
{
    cSPMissionManager* mgr = (cSPMissionManager*)GetMissionManager();
    if (pNoun == 0)
        return 0;

    cGameNounManager* nouns = ((cGameNounOwner*)pNoun)->GetNounManager();
    void* avatar = nouns->GetAvatar();
    cStarManager* stars = StarManager(avatar);
    void* empire = stars->GetEmpireByID();

    IMission* mission = mgr->CreateMissionFromID(0xa4b0a2df, pNoun, empire, 0);
    if (mission == 0)
        return 0;

    cSPMission* sp = mission->GetSPMission(0x3960c0a);
    int enemy = sp->FUN_c5b530();
    sp->Init(0);
    sp->SetTarget(pNoun);
    sp->SetEmpire(enemy);
    sp->Accept();

    pList->push_back(AutoRefCount(mission));
    return 1;
}
