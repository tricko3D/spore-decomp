// SP::SetupHatchAtNest  @ 0x00d40ff0
//
// Seed the nest hatch: take the avatar's herd, snapshot the last surface
// position as the hover origin, put the avatar and every herd member into the
// hatch pose (hatch flag, cleared hover offset), then play the
// "CRG_Fade_From_Black_Egg" sequence.  When the current terrain sphere has no
// hatch move left, the creature-mode property list's spawn range decides
// whether the hatch is allowed; when the desired herd moved away, the old
// herd members are collected and released before the drama is triggered.

#include "sse_helpers.h"

typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned char uint8_t;

#define VT(o, off) (*(void***)(o))[(off) / 4]

typedef void  (__thiscall* TFnV)(void*);
typedef void  (__thiscall* TFnVI)(void*, int);
typedef void  (__thiscall* TFnVF)(void*, float);
typedef bool  (__thiscall* TFnI)(void*);
typedef void  (__thiscall* TFnVPP)(void*, void*, void*);
typedef void  (__thiscall* TFnVUL)(void*, uint32_t, int);
typedef void* (__thiscall* TFnP)(void*);

struct cVec3
{
    float x;
    float y;
    float z;
};

struct cQuat
{
    float x;
    float y;
    float z;
    float w;
};

struct cNest;
struct cHerd;
struct cCreature;
struct cPlanetModel;
struct cTerrainSphere;
struct cTriggerMgr;
struct cActionTarget;
struct cPropertyList;
struct cCreatureModeStrategy;
struct cLayerManager;
struct cPosseSimulator;
struct cSPDramaManager;
struct cGameNounManager;

// ---------------------------------------------------------------------------
// retail objects this entry point walks (field offsets as in the herd view of
// the sibling slices 0x00d40930 / 0x00d40cc0)
// ---------------------------------------------------------------------------
struct cNest
{
    void* FUN_00c69bc0(cVec3* pScratch, cVec3* pPos);          // 0x00c69bc0
};

struct cHerd
{
    uint8_t     mPad000[0x40];                                 // +0x000
    cCreature** mpBegin;                                       // +0x040
    cCreature** mpEnd;                                         // +0x044
    cCreature** mpCapacity;                                    // +0x048
    uint8_t     mPad04c[0x120 - 0x4c];                         // +0x04c
    int32_t     mRangeMin;                                     // +0x120
    int32_t     mRangeMax;                                     // +0x124
    uint8_t     mPad128[0x160 - 0x128];                        // +0x128
    cNest*      mpNest;                                        // +0x160

    cVec3* GetPosition();                                      // 0x00c6acc0
    void   FUN_00c6b540();                                     // 0x00c6b540
    void   FUN_00c6d0f0(const cVec3* pPos);                    // 0x00c6d0f0
};

// object hanging off the creature's +0xb54 slot (hover/height state)
struct cHoverState
{
    uint8_t mPad000[0x80];                                     // +0x000
    float   mField80;                                          // +0x080

    void FUN_00a02b00(int a);                                  // 0x00a02b00
};

struct cCreature
{
    void* GetAvatarHerd();                                     // 0x00c04590
    void  FUN_00c02c20(int a);                                 // 0x00c02c20
    void  FUN_00c0d9b0();                                      // 0x00c0d9b0
    void  FUN_00c0bbe0();                                      // 0x00c0bbe0

    uint8_t  mPad000[0xc0];                                    // +0x000
    uint8_t  mSubObject[0x135 - 0xc0];                         // +0x0c0 embedded sub-object
    uint8_t  mbHatchFlag;                                      // +0x135
    uint8_t  mPad136[0xb54 - 0x136];                           // +0x136
    cHoverState* mpFieldB54;                                   // +0xb54
    uint32_t mFlagsB58;                                        // +0xb58
    uint8_t  mPadB5c[0x10];                                    // +0xb5c

    void* Sub() { return mSubObject; }
};

// herd member list handed out by the noun manager
struct cCreatureList
{
    cCreature** mpBegin;                                       // +0x00
    cCreature** mpEnd;                                         // +0x04
};

struct cPlanetModel
{
    cQuat* BuildSurfaceOrientation_2(cQuat* pOut, const cVec3* pA,
                                     const cVec3* pB);         // 0x00b7f1f0
};

struct cGameNounManager
{
    void*           GetAvatar();                               // 0x00b1fdb0
    cCreatureList*  GetHerdMembers();                          // 0x00b1f9c0
    cTerrainSphere* GetCurrentTerrainSphere();                 // 0x00f67d90
};

struct cPosseSimulator
{
    void FUN_00d52e40();                                       // 0x00d52e40
};

struct cLayerManager
{
    void* mpVtable;
};

struct cTerrainSphere
{
    bool GetProperty(uint32_t nKey, int* pValue);              // 0x00c77380
    void SetProperty(uint32_t nKey, int nValue);               // 0x00c79f00
    void RemoveProperty(uint32_t nKey);                        // 0x00c77320
};

struct cCreatureModeStrategy
{
    uint8_t       mPad000[0xb4];                               // +0x000
    cPropertyList* mpProperties;                               // +0x0b4
};

// The original keeps the action-target temporary as raw stack storage and
// calls its constructor/destructor explicitly (no C++ unwind state).
struct cActionTarget
{
    uint8_t  mPad000[0x2c];                                    // +0x000
    void*    mpContext;                                        // +0x02c

    cActionTarget* Setup(void* pObject);                       // 0x00ad7a30
    void           Clear();                                    // 0x00ad7ad0
};

struct cTriggerMgr
{
    void* PostTrigger(const char* pName, int a, int b, int c, int d, int e); // 0x00ae0930
    void  PostAction(uint32_t nId, cActionTarget* pTarget, void* pContext); // 0x00ae09b0
};

struct cDOMNode
{
    void* GetParent();                                         // 0x00ff0420
};

struct cSPDramaManager
{
    void FUN_00d4d6c0();                                       // 0x00d4d6c0
    void TriggerDramaEvent(uint32_t nEvent, int a);            // 0x00d4d790
};

// ---------------------------------------------------------------------------
// helpers (retail addresses in comments)
// ---------------------------------------------------------------------------
cGameNounManager* NounManager();                               // 0x00b3d300
cPlanetModel*     PlanetModel();                               // 0x00b3d350
cLayerManager*    LayerManager();                              // 0x0067cb20
void*             GetTriggerMgr();                             // 0x00b3d4d0
cPosseSimulator*  PosseSimulator();                            // 0x00d539d0
void              FUN_00d3c3b0();                              // 0x00d3c3b0
cSPDramaManager*  GetDramaManager();                           // 0x00d51660
void              FUN_00d40e70();                              // 0x00d40e70
int               GetDesiredAvatarHerd(int nMode);             // 0x00d40b40
float             FUN_00d2e360();                              // 0x00d2e360
bool              GetPropertyAsIntArray(cPropertyList* pList, uint32_t nKey,
                                        int* pCount, uint32_t** ppValues); // 0x006a07d0
void*             operator new[](unsigned int nSize, const char* pName,
                                 int flags, int debugFlags,
                                 const char* pFile, int nLine);   // 0x00f473a0
void              operator delete[](void* pBlock);             // 0x00f47380
void              FUN_00829110(void* pContext, cCreature** pBuffer,
                               cCreature** pEnd, cCreature** pBegin,
                               cCreature*** ppOutEnd);         // 0x00829110

extern cCreatureModeStrategy* gCreatureModeStrategy;           // 0x0169e294
extern cVec3 gLastSurface;                                     // 0x0169e298
extern cVec3 gHoverSaved;                                      // 0x0169e3b4
extern const float kHatchBlendTime;                            // 0x01488874 (0.1f)

namespace SP
{

// @ 0x00d40ff0
void SetupHatchAtNest()
{
    cGameNounManager* nouns = NounManager();
    cCreature* avatar = (cCreature*)nouns->GetAvatar();
    cHerd* herd = (cHerd*)avatar->GetAvatarHerd();

    const cVec3* pPosition = herd->GetPosition();
    cVec3 pos;
    pos.x = pPosition->x;
    pos.y = pPosition->y;
    pos.z = pPosition->z;
    cQuat scratch;
    pos = *(const cVec3*)herd->mpNest->FUN_00c69bc0((cVec3*)&scratch, &pos);

    void* sub = avatar->Sub();
    gHoverSaved = gLastSurface;

    const cVec3* hint = (const cVec3*)((TFnP)VT(sub, 0x30))(sub);
    cQuat* orientation = PlanetModel()->BuildSurfaceOrientation_2(&scratch, &pos, hint);
    ((TFnVPP)VT(sub, 0x44))(sub, &pos, orientation);

    cLayerManager* layers = LayerManager();
    ((TFnVUL)VT(layers, 0x2c))(layers, 0xf41bb04c, 0);

    // hatch pose of the avatar
    ((TFnVI)VT(avatar, 0xe8))(avatar, 0);
    ((TFnVF)VT(sub, 0x40))(sub, kHatchBlendTime);
    avatar->FUN_00c02c20(0);
    ((TFnVI)VT(avatar, 0xcc))(avatar, 0);
    ((TFnV)VT(avatar, 0xbc))(avatar);
    avatar->mFlagsB58 |= 0x800;
    avatar->mpFieldB54->mField80 = 0.0f;
    avatar->mpFieldB54->FUN_00a02b00(1);
    ((TFnVI)VT(avatar, 0xac))(avatar, 1);
    avatar->FUN_00c0d9b0();
    avatar->mbHatchFlag = 1;
    avatar->FUN_00c0bbe0();
    herd->FUN_00c6d0f0(&pos);
    avatar->mbHatchFlag = 0;

    cPosseSimulator* posse = PosseSimulator();
    posse->FUN_00d52e40();

    // every other herd member gets the same hatch pose
    cCreatureList* members = ((cGameNounManager*)NounManager())->GetHerdMembers();
    cCreature** it = members->mpBegin;
    cCreature** end = members->mpEnd;
    for (; it != end; ++it) {
        cCreature* creature = *it;
        void* creatureSub = creature->Sub();
        ((TFnVF)VT(creatureSub, 0x40))(creatureSub, kHatchBlendTime);
        creature->mbHatchFlag = 0;
        ((TFnVI)VT(creature, 0xcc))(creature, 0);
        ((TFnV)VT(creature, 0xbc))(creature);
        ((TFnVI)VT(creature, 0xac))(creature, 1);
        creature->mFlagsB58 |= 0x800;
        creature->mpFieldB54->mField80 = 0.0f;
    }

    FUN_00d3c3b0();

    int sphereValue = 0;
    cTerrainSphere* sphere = ((cGameNounManager*)NounManager())->GetCurrentTerrainSphere();
    if (!sphere->GetProperty(0x514a219, &sphereValue)) {
        cCreatureModeStrategy* strategy = gCreatureModeStrategy;
        int count = 0;
        uint32_t* values = 0;
        if (GetPropertyAsIntArray(strategy->mpProperties, 0x4098097, &count, &values)) {
            int low = 0;
            int high = SseRoundToInt(FUN_00d2e360());
            sphere->GetProperty(0x54fb6bf, &low);

            bool inRange = false;
            for (int i = 0; i < count; ++i) {
                if (inRange)
                    break;
                inRange = values[i] > low && values[i] <= high;
            }
            if (inRange) {
                sphere->SetProperty(0x514a219, 1);
                sphere->SetProperty(0x54fb6bf, high);
                herd->mRangeMax = -1;
                herd->mRangeMin = 5;
            }
        }
    }

    if (sphere->GetProperty(0x514a219, &sphereValue)) {
        if (GetDesiredAvatarHerd(0) != (int)herd)
            goto reclaim;

        sphere->RemoveProperty(0x514a219);
    }
    herd->FUN_00c6b540();

drama:
    {
        cSPDramaManager* drama = GetDramaManager();
        drama->FUN_00d4d6c0();
        GetDramaManager()->TriggerDramaEvent(4, 0);
    }

    if (sphere->GetProperty(0x514a219, &sphereValue) && sphereValue == 1) {
        FUN_00d40e70();
        return;
    }

    {
        cTriggerMgr* trigger = (cTriggerMgr*)GetTriggerMgr();
        void* handle = trigger->PostTrigger("CRG_Fade_From_Black_Egg", 1, 1, 0, 0, 0);
        {
            cActionTarget hatchTarget;
            ((cTriggerMgr*)GetTriggerMgr())->PostAction(0x28930a2c,
                                                        hatchTarget.Setup(sub), handle);
            hatchTarget.Clear();
        }
        {
            void* parent = ((cDOMNode*)GetDramaManager())->GetParent();
            void* parentPos = parent ? (char*)parent + 0xc0 : 0;
            cActionTarget uiTarget;
            ((cTriggerMgr*)GetTriggerMgr())->PostAction(0x927b68d9,
                                                        uiTarget.Setup(parentPos), handle);
            uiTarget.Clear();
        }
    }
    return;

reclaim:
    {
        int count = (int)(herd->mpEnd - herd->mpBegin);
        cCreature** buffer = 0;
        if (count != 0)
            buffer = (cCreature**)operator new[](count * 4, "Simulator", 0, 0,
                                                  "allocator.h", 0xd1);

        cCreature** outEnd = buffer;
        FUN_00829110(sub, buffer, herd->mpEnd, herd->mpBegin, &outEnd);

        for (cCreature** it = buffer; it != outEnd; ++it) {
            cCreature* creature = *it;
            void* creatureSub = creature->Sub();
            if (!((TFnI)VT(creatureSub, 0x58))(creatureSub))
                ((TFnVI)VT(creature, 0xc8))(creature, 1);
        }

        for (cCreature** it = buffer; it < outEnd; ++it) {
            cCreature* creature = *it;
            if (creature != 0)
                ((TFnV)VT(creature, 4))(creature);
        }

        if (buffer != 0 && ((int*)buffer)[-1] != 0)
            operator delete[](buffer);

        goto drama;
    }
}

} // namespace SP
