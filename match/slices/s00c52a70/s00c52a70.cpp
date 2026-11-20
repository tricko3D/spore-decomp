// Slice s00c52a70 - SporeApp 0x00c52a70
// SP::cSPMissionFetch::IsSatisfied: true when the player inventory already holds
// the fetch target selected by the tag at +0x1f4 (commodity, artifact cargo,
// animal, animal-or-plant pair, every sampler-plate sample, or a rare item).
// Shipping layout: cSPMission base is 0x1f0 bytes; state +0x1f0, fetch type
// +0x1f4, artifact key +0x1f8, artifact position +0x204, plant key +0x210,
// animal key +0x21c, commodity key +0x228, sampler-plate vector +0x234.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast.
#include "types.h"

typedef unsigned char  u8;
typedef unsigned int   u32;

#define VSLOT(n) virtual void _v##n();

// Resource key, three dwords, in member order instance, type, group.
struct Key {
    u32 mInstance;   // 0x00
    u32 mType;       // 0x04
    u32 mGroup;      // 0x08
};

// The shared empty key.
extern Key gEmptyKey;                                       // 0x016907d4

struct cSPVector3 {
    float x, y, z;
};

struct cFetchSample {            // 0x10-byte element of the sampler plate
    bool mIsAnimal;              // 0x00
    Key  mSpeciesKey;            // 0x04
};

struct cFetchSampleVector {
    cFetchSample* mpBegin;       // 0x00
    cFetchSample* mpEnd;         // 0x04
    cFetchSample* mpCapacity;    // 0x08
};

// Local stand-ins for the inventory classes; the virtual slots match the retail
// vtable offsets this function calls through.
class cInventoryItem {
public:
    VSLOT(0) VSLOT(1) VSLOT(2) VSLOT(3) VSLOT(4) VSLOT(5)
    virtual Key* GetKey();                           // +0x18
};

class cInventory {
public:
    VSLOT(0) VSLOT(1) VSLOT(2) VSLOT(3) VSLOT(4) VSLOT(5) VSLOT(6) VSLOT(7)
    VSLOT(8) VSLOT(9) VSLOT(10) VSLOT(11) VSLOT(12) VSLOT(13) VSLOT(14)
    VSLOT(15) VSLOT(16) VSLOT(17) VSLOT(18) VSLOT(19) VSLOT(20) VSLOT(21)
    VSLOT(22)
    virtual int GetItemCount();                      // +0x5c
    virtual cInventoryItem* GetItem(int nIndex);     // +0x60
    virtual int FindItem(int nType, const Key* pKey); // +0x64
};

class cSpaceGame {
public:
    cInventory* GetPlayerInventory();                // 0x00a1ad60
};

cSpaceGame* SpaceGameGet();                          // 0x01002bd0
bool __cdecl IsItemARare(Key* pKey);                 // 0x0103a3c0

class cSPMissionFetch {
public:
    u8  mPad000[0x1f0];
    int mState1f0;                       // 0x1f0
    int mTypeOfFetch;                    // 0x1f4 (signed: the switch is a signed compare)
    Key mFetchArtifactCargoItemID;       // 0x1f8
    cSPVector3 mFetchArtifactPos;        // 0x204
    Key mFetchPlantSpeciesKey;           // 0x210
    Key mFetchAnimalSpeciesKey;          // 0x21c
    Key mFetchCommodityKey;              // 0x228
    cFetchSampleVector mSamplerPlate;    // 0x234

    bool IsSatisfied();
};

// @ 0x00c52a70
bool cSPMissionFetch::IsSatisfied()
{
    cInventory* pInventory = SpaceGameGet()->GetPlayerInventory();
    if (pInventory == 0)
        return false;

    switch (mTypeOfFetch) {
    case 0xbfb44ccc:
        return pInventory->FindItem(4, &mFetchCommodityKey) != -1;
    case 0x19ec2c8b:
        return pInventory->FindItem(5, &mFetchArtifactCargoItemID) != -1;
    case 0xfb3b1635:
    case 0x3e994b2b:
    case 0x69412983:
        return pInventory->FindItem(6, &mFetchAnimalSpeciesKey) != -1;
    case 0x2dc69129:
    case 0x06d5c48c:
    case 0x736c417f: {
        int nIndex = -1;
        if (mFetchAnimalSpeciesKey.mInstance != gEmptyKey.mInstance
            || mFetchAnimalSpeciesKey.mType != gEmptyKey.mType
            || mFetchAnimalSpeciesKey.mGroup != gEmptyKey.mGroup) {
            nIndex = pInventory->FindItem(1, &mFetchAnimalSpeciesKey);
        } else if (mFetchPlantSpeciesKey.mInstance != gEmptyKey.mInstance
                   || mFetchPlantSpeciesKey.mType != gEmptyKey.mType
                   || mFetchPlantSpeciesKey.mGroup != gEmptyKey.mGroup) {
            nIndex = pInventory->FindItem(0, &mFetchPlantSpeciesKey);
        }
        return nIndex != -1;
    }
    case 0x23da98ff: {
        int nCount = (int)(mSamplerPlate.mpEnd - mSamplerPlate.mpBegin);
        for (int i = 0; i < nCount; ++i) {
            cFetchSample* pSample = &mSamplerPlate.mpBegin[i];
            int nType = pSample->mIsAnimal ? 1 : 0;
            if (pInventory->FindItem(nType, &pSample->mSpeciesKey) == -1)
                return false;
        }
        return true;
    }
    case 0x7ecbe6f5: {
        int nCount = pInventory->GetItemCount();
        for (int i = 0; i < nCount; ++i) {
            cInventoryItem* pItem = pInventory->GetItem(i);
            if (pItem != 0 && IsItemARare(pItem->GetKey()))
                return true;
        }
        return false;
    }
    default:
        break;
    }
    return false;
}
