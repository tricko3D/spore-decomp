// SP::cSPSimulatorSpaceGame::StartAmbush
// Flags: /O2 /Ob2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-
#include "types.h"

#define nullptr 0

// ---------------------------------------------------------------------------
// small engine types
// ---------------------------------------------------------------------------
struct Vec3 { float x, y, z; };

class IRefCount {
public:
    virtual void AddRef();          // +0x00
    virtual void Release();         // +0x04
};

struct AutoRefCount {
    IRefCount* mpObject;            // +0x00
};

struct VectorAllocHeader {
    int32_t mnCapacity;
};

// The three-dword model key filled by cUFOTypeOwner::GetModelKey.
struct ModelKey {
    int32_t mnInstanceId;           // +0x00
    int32_t mnGroupId;              // +0x04
    int32_t mnTypeId;               // +0x08
};

void __cdecl operator_delete(void* pBlock);     // 0x00f47380

// The game's eastl vector: three element pointers plus the allocator stamp,
// 0x14 bytes.  The size matters for the frame: the trailing allocator fields
// sit in the frame even though StartAmbush only clears the first three.
class SPVector {
public:
    AutoRefCount* mpBegin;          // +0x00
    AutoRefCount* mpEnd;            // +0x04
    AutoRefCount* mpCapacity;       // +0x08
    const char* mpAllocName;        // +0x0c
    uint32_t mAllocFlags;           // +0x10

    void DoInsertPtr(AutoRefCount* pPos, const AutoRefCount& rValue);   // 0x00aea5d0

    void Destroy()
    {
        for (AutoRefCount* p = mpBegin; p < mpEnd; ++p) {
            if (p->mpObject != nullptr)
                p->mpObject->Release();
        }
        if (mpBegin != nullptr &&
            reinterpret_cast<VectorAllocHeader*>(mpBegin)[-1].mnCapacity != 0) {
            operator_delete(mpBegin);
        }
    }
};

// ---------------------------------------------------------------------------
// game nouns
// ---------------------------------------------------------------------------
class cGameData {
public:
    uint8_t mPad00[0x3c];
    Vec3 mCities;                   // +0x3c

    Vec3* GetCities();              // 0x005c65e0
};

class cProperty {
public:
    uint8_t mPad00[0x12];
    uint16_t mnType;                // +0x12
    float* GetFloat();              // 0x0041ea70
};

class ISimPropertyHost {
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
    virtual bool GetProperty(uint32_t nPropertyId, void** ppOut);   // +0x24
};

class cSimUniverse {
public:
    uint8_t mPad00[0x10];
    ISimPropertyHost* mpPropertyHost;   // +0x10
};

struct StarRecordFilter {
    int32_t mnFlags;                // +0x00
    int32_t mnTypeMask;             // +0x04
    int32_t mnField08;              // +0x08
    float   mfField0c;              // +0x0c
    float   mfRadius;               // +0x10
    float   mfField14;              // +0x14
    int32_t mnField18;              // +0x18
};

class cStarManager {
public:
    void GetStarRecords(Vec3* pPosition, StarRecordFilter* pFilter, SPVector* pOut); // 0x00bb1080
};

class cUFOTypeOwner {
public:
    void GetModelKey(ModelKey* pKey);   // 0x0102f660
};

class cAmbushContext {
public:
    uint8_t mPad00[0x160];
    cUFOTypeOwner* mpUFOTypeOwner;  // +0x160
};

class cSPGameDataUFO : public IRefCount {
public:
    uint8_t mPad04[0x7b0 - 0x04];
    cGameData* mpTarget;            // +0x7b0

    void SetDesiredModel(const ModelKey& rKey, int32_t nFlags);     // 0x00c37180
};

class cSPSimulatorSpaceGame {
public:
    void StartAmbush();             // 0x010095e0

private:
    uint8_t mPad00[0x78];
    SPVector mNPC_UFOs;             // +0x078
};

class RandomLinearCongruential {
public:
    uint8_t mPad00[1];
};

// ---------------------------------------------------------------------------
// engine accessors
// ---------------------------------------------------------------------------
cGameData*      __cdecl GetLocalCiv();          // 0x01021240
cAmbushContext* __cdecl GetAmbushContext();     // 0x00fd9c60
cStarManager*   __cdecl StarManager();          // 0x00b3d2a0
cSPGameDataUFO* __cdecl CreateUFOForSolarOrGalaxy(int32_t nState, void* pOwner); // 0x0102ac50
void            __cdecl ShuffleStarRecords(AutoRefCount* pBegin, AutoRefCount* pEnd,
                                           RandomLinearCongruential* pRandom);   // 0x00b96130

extern "C" cSimUniverse* gSimUniverse;          // 0x016dc798
extern "C" RandomLinearCongruential gRandom;    // 0x01601760
extern "C" int32_t gnAmbushUFOCount;            // 0x015b68b8
extern "C" float g_zero;                        // 0x013eb1bc

// ---------------------------------------------------------------------------
// 0x010095e0 -- start an ambush against the current held-back UFO list
// ---------------------------------------------------------------------------
// @ 0x010095e0
void cSPSimulatorSpaceGame::StartAmbush()
{
    cGameData* pCiv = GetLocalCiv();

    ModelKey modelKey;
    modelKey.mnInstanceId = 0;
    modelKey.mnGroupId = 0;
    modelKey.mnTypeId = 0;
    // Naming the context keeps it in a register across the GetModelKey call,
    // which is the shape retail has.
    cAmbushContext* pAmbush = GetAmbushContext();
    pAmbush->mpUFOTypeOwner->GetModelKey(&modelKey);

    // The radius lookup and the filter it feeds share one scope: retail reuses
    // the radius/property stack slots for the spawn loop below.
    StarRecordFilter filter;
    {
        float fRadius = 10.0f;
        cProperty* pProperty;           // written by GetProperty
        ISimPropertyHost* pPropertyHost = gSimUniverse->mpPropertyHost;
        if (pPropertyHost != nullptr &&
            pPropertyHost->GetProperty(0x55e7dad, (void**)&pProperty) &&
            pProperty->mnType == 0xd) {
            fRadius = *pProperty->GetFloat();
        }

        filter.mfField14 = g_zero;
        filter.mnField08 = 0;
        filter.mnField18 = 0;
        filter.mfField0c = 0.0f;
        filter.mfRadius = fRadius;
        filter.mnTypeMask = 0x1f;
        filter.mnFlags = 0x1ffd;
    }

    SPVector records;
    records.mpBegin = nullptr;
    records.mpEnd = nullptr;
    records.mpCapacity = nullptr;

    StarManager()->GetStarRecords(pCiv->GetCities(), &filter, &records);

    ShuffleStarRecords(records.mpBegin, records.mpEnd, &gRandom);

    int32_t nSpawned = 0;
    while (nSpawned < gnAmbushUFOCount) {
        cGameData* pTarget = pCiv;
        if (nSpawned > 0) {
            if (records.mpBegin == records.mpEnd)
                break;
            pTarget = (cGameData*)(records.mpEnd - 1)->mpObject;
            --records.mpEnd;
            if (records.mpEnd->mpObject != nullptr)
                records.mpEnd->mpObject->Release();
        }

        cSPGameDataUFO* pUFO = CreateUFOForSolarOrGalaxy(8, nullptr);
        pUFO->SetDesiredModel(modelKey, 1);
        pUFO->mpTarget = pTarget;

        AutoRefCount* pEnd = mNPC_UFOs.mpEnd;
        AutoRefCount* pCapacity = mNPC_UFOs.mpCapacity;
        AutoRefCount temp;
        temp.mpObject = pUFO;
        pUFO->AddRef();
        if (pEnd < pCapacity) {
            if (pEnd != nullptr) {
                pEnd->mpObject = pUFO;
                pUFO->AddRef();
            }
            mNPC_UFOs.mpEnd = pEnd + 1;
        } else {
            mNPC_UFOs.DoInsertPtr(pEnd, temp);
        }
        if (temp.mpObject != nullptr)
            temp.mpObject->Release();

        ++nSpawned;
    }

    AutoRefCount* pRecordEnd = records.mpEnd;
    for (AutoRefCount* p = records.mpBegin; p < pRecordEnd; ++p) {
        if (p->mpObject != nullptr)
            p->mpObject->Release();
    }
    if (records.mpBegin != nullptr &&
        reinterpret_cast<VectorAllocHeader*>(records.mpBegin)[-1].mnCapacity != 0) {
        operator_delete(records.mpBegin);
    }
}
