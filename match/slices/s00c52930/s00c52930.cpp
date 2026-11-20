// Slice s00c52930 - SporeApp 0x00c52930
// SP::cSPMissionFetch: reports whether the fetch target selected by the tag at
// 0x1f4 holds a non-empty key (or, for the sampler-plate tag, any sample).
// Flags: /O2 /MD /Gy /TP /arch:SSE
#include "types.h"

typedef unsigned char  u8;
typedef unsigned int   u32;

// Resource key, three dwords, in member order instance, type, group.
struct Key {
    u32 mInstance;   // 0x00
    u32 mType;       // 0x04
    u32 mGroup;      // 0x08
};

// The shared empty key, three dwords.
extern Key gEmptyKey;                                       // 0x016907d4

bool __cdecl operator!=(const Key& a, const Key& b);         // 0x005f78f0

class cSPMissionFetch {
public:
    u8  mPad0000[0x1f0];
    int mState1f0;                          // 0x1f0
    int mTag1f4;                            // 0x1f4 (signed: the switch is a signed compare)
    Key mFetchArtifactCargoItemID;          // 0x1f8
    u8  mPad0204[0x0c];                     // 0x204
    Key mFetchPlantSpeciesKey;              // 0x210
    Key mFetchAnimalSpeciesKey;             // 0x21c
    Key mFetchCommodityKey;                 // 0x228
    void* mpSamples234;                     // 0x234
    void* mpSamples238;                     // 0x238

    bool HasFetchTarget();
};

// @ 0x00c52930
bool cSPMissionFetch::HasFetchTarget()
{
    switch (mTag1f4) {
    case 0xbfb44ccc:
        return mFetchCommodityKey != gEmptyKey;
    case 0x6d5c48c:
        return mFetchPlantSpeciesKey != gEmptyKey;
    case 0x19ec2c8b:
        return mFetchArtifactCargoItemID != gEmptyKey;
    case 0xfb3b1635:
    case 0x2dc69129:
    case 0x3e994b2b:
    case 0x69412983:
        return mFetchAnimalSpeciesKey.mInstance != gEmptyKey.mInstance
            || mFetchAnimalSpeciesKey.mType != gEmptyKey.mType
            || mFetchAnimalSpeciesKey.mGroup != gEmptyKey.mGroup;
    case 0x736c417f:
        return mFetchPlantSpeciesKey.mInstance != gEmptyKey.mInstance
            || mFetchPlantSpeciesKey.mType != gEmptyKey.mType
            || mFetchPlantSpeciesKey.mGroup != gEmptyKey.mGroup
            || mFetchAnimalSpeciesKey != gEmptyKey;
    case 0x23da98ff:
        return mpSamples234 != mpSamples238;
    case 0x7ecbe6f5:
        return true;
    default:
        return false;
    }
}
