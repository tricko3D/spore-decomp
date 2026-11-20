// @ 0x01058950
//
// Artifact activation helper for the space tool strategies: take the artifact
// owned by the tool's owner object, mark it as activated, remember its sub
// object in the tool and notify the behaviour system about the closest noun in
// the planet game data.
//
// Local stand-ins for the SP classes; the byte patterns they describe are the
// ones seen in the original (secondary vtables at +0x34 / +0x120, artifact sub
// object at +0x5d0, state fields at +0x224 / +0x658 / +0x65e).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include "types.h"
#include <math.h>
#include <intrin.h>

struct Vector3 {
    float x;
    float y;
    float z;
};

// Object with a position getter at vtable slot +0x2c.
class IPositioned {
public:
    virtual void      v00();
    virtual void      v04();
    virtual void      v08();
    virtual void      v0c();
    virtual void      v10();
    virtual void      v14();
    virtual void      v18();
    virtual void      v1c();
    virtual void      v20();
    virtual void      v24();
    virtual void      v28();
    virtual Vector3*  GetPosition();   // +0x2c
};

// Sub object of an artifact (behaviour target), 0x18 bytes.
class cArtifactSub {
public:
    virtual void   v00();
    virtual void   v04();
    virtual void   v08();
    virtual void*  v0c(uint32_t nType, void* pOwner, int nFlag);  // +0x0c
    virtual void   v10();
    virtual void   v14();
    virtual void   v18();
    virtual void   v1c();
    virtual void*  v20();              // +0x20
    virtual uint32_t GetID();          // +0x24
    virtual void   v28();
    virtual void   v2c();
private:
    uint8_t mData[0x14];
};

class cArtifact {
public:
    uint8_t       m00[0x34];
    IPositioned   mPos34;              // +0x034
    uint8_t       m38[0x120 - 0x38];
    IPositioned   mPos120;             // +0x120
    uint8_t       m124[0x224 - 0x124];
    uint32_t      m224;                // +0x224
    uint8_t       m228[0x5d0 - 0x228];
    cArtifactSub  mSub5d0;             // +0x5d0
    uint8_t       m5e8[0x658 - 0x5e8];
    int           m658;                // +0x658
    uint8_t       m65c[2];
    uint8_t       m65e;                // +0x65e
};

// Element of the planet game data list.
class cNoun {
public:
    uint8_t       m00[0x34];
    IPositioned   mPos34;              // +0x034
    uint8_t       m38[0x120 - 0x38];
    IPositioned   mPos120;             // +0x120
};

struct NounList {
    void** begin;
    void** end;
};

class cNounManager {
public:
    NounList* GetPlanetGameData();     // 0x00ace2c0
};

class cBehaviorManager {
public:
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38(void* pSub);      // +0x38
};

// Ref counted pointer to the selected noun.
struct AutoRefCount {
    void* mp;
    AutoRefCount& operator=(void* pValue);      // 0x00b5f950 AutoRefCount::operator=
};

// Ref counted pointer stored in the tool at +0x128.
struct WinTextRef {
    void* mp;
    WinTextRef& operator=(void* pValue);        // 0x00b5f950
};

// Owner object of the tool.
class cOwnerObject {
public:
    void* FUN_006e87e0();                       // 0x006e87e0
    void  FUN_00cb5bb0();                       // 0x00cb5bb0
};

class cSpaceToolData {
public:
    uint8_t m00[0x124];
    void*   mpOwnerObject;             // +0x124
    void*   mRef128;                   // +0x128
};

// Stack slot message used to notify the messaging server.
struct SlotMessage {
    void**   vtbl;                     // +0x00
    uint32_t mRefCount;                // +0x04
    void*    mpObject;                 // +0x08
    uint32_t m0c;                      // +0x0c
    uint32_t m10;                      // +0x10
    uint32_t m14;                      // +0x14
    uint32_t m18;                      // +0x18
    uint32_t m1c;                      // +0x1c
    uint32_t m20;                      // +0x20
    uint32_t m24;                      // +0x24
    uint32_t m28;                      // +0x28
    uint32_t m2c;                      // +0x2c
    uint32_t mKind;                    // +0x30
    uint32_t m34;                      // +0x34
    uint32_t m38;                      // +0x38
    uint32_t m3c;                      // +0x3c

    void Destruct();                   // 0x00421cf0 SlotMessage::Destruct
};

// unnamed retail callees
void* __cdecl FUN_00b3d260();                       // 0x00b3d260 BehaviourManager
void* __cdecl FUN_00b3d300();                       // 0x00b3d300 NounManager
void* __cdecl FUN_00883860();                       // 0x00883860 Messaging::GetServer

// @ 0x01058950
bool __cdecl FUN_01058950(cSpaceToolData* pTool, const Vector3& position, int a3, int a4)
{
    cArtifact* pArtifact = 0;

    void* pOwnerNoun = ((cOwnerObject*)pTool->mpOwnerObject)->FUN_006e87e0();
    if (pOwnerNoun != 0) {
        pArtifact = (cArtifact*)((void* (__thiscall*)(void*, uint32_t))
                                 (*(void***)pOwnerNoun)[0x0c / 4])(pOwnerNoun, 0x2dd8c33);
    }

    if (pArtifact != 0 && pArtifact->m65e != 0 &&
        pArtifact->mSub5d0.GetID() != 0x114242e6 &&
        (pArtifact->m658 == 4 || pArtifact->m658 == 7)) {
        ((cBehaviorManager*)FUN_00b3d260())->v38(&pArtifact->mSub5d0);
        pArtifact->m224 = 4;

        void* pOwnerPart = pTool->mpOwnerObject != 0
                         ? (void*)((uint8_t*)pTool->mpOwnerObject + 0x11c)
                         : 0;
        pArtifact->mSub5d0.v0c(*(uint32_t*)0x16df2f4, pOwnerPart, 0);
        ((WinTextRef*)((uint8_t*)pTool + 0x128))->operator=(pArtifact->mSub5d0.v20());

        float fBest = *(float*)0x149af8c;
        AutoRefCount nearest;
        nearest.mp = 0;
        NounList* pList = ((cNounManager*)FUN_00b3d300())->GetPlanetGameData();
        int nCount = (int)(pList->end - pList->begin);
        for (int i = 0; i < nCount; ++i) {
            cNoun* pNoun = (cNoun*)pList->begin[i];
            Vector3* pA = pArtifact->mPos34.GetPosition();
            Vector3* pB = pNoun->mPos120.GetPosition();
            float fDX = pB->x - pA->x;
            float fDY = pB->y - pA->y;
            float fDZ = pB->z - pA->z;
            float fDist = sqrtf(fDX * fDX + fDY * fDY + fDZ * fDZ);
            if (fDist < fBest) {
                fBest = fDist;
                nearest = (void*)pNoun;
            }
        }

        cNoun* pSelected = (cNoun*)nearest.mp;
        if (pSelected != 0) {
            SlotMessage msg;
            msg.mKind = 0;
            msg.vtbl = (void**)0x13eb90c;
            _InterlockedExchange((long*)&msg.mRefCount, 0);
            msg.vtbl = (void**)0x13eb844;
            msg.m38 = 0;
            msg.mKind = 0x30aa01b;
            msg.mpObject = pSelected;
            msg.m10 = 0xffff0000;
            msg.m18 = 0;
            msg.m20 = 0xffffffff;

            void* pServer = FUN_00883860();
            ((void (__thiscall*)(void*, uint32_t, void*, int))
             (*(void***)pServer)[0x14 / 4])(pServer, msg.mKind, &msg, 0);

            msg.Destruct();
            ((void (__thiscall*)(void*))(*(void***)pSelected)[1])(pSelected);
        }
    }

    if (pTool->mpOwnerObject != 0)
        ((cOwnerObject*)pTool->mpOwnerObject)->FUN_00cb5bb0();
    return true;
}
