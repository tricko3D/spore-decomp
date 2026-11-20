// SP::cGameEditInputStrategy::FUN_00d16820
// Returns the definition id of the selected gameplay marker: it hashes the
// marker class name, asks the property manager for it, reads the marker combo
// box (window 0xabc0fff9 in the +0xe0 layout) and picks the selection-th noun
// definition of that class from the noun manager's definition table.
#include "game/types.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned __int64 u64;

struct Obj;
struct IWindow;

// ---- the strategy (only the members this function touches) -----------------
struct cSPUILayout {
    IWindow* FindWindowByID(u32 nID, int nFlag);            // 0x008105b0
};

struct cGameEditInputStrategy {
    u8 mPad0000[0xe0];
    cSPUILayout mLayout;                                    // 0x0e0

    u32 GetGameplayMarkerTypeFromUI();                      // 0x00d15fb0
    u32 FUN_00d16820();                                     // 0x00d16820
};

// ---- retail callees --------------------------------------------------------
u32  __cdecl GetGameplayMarkerClassId(u32 classId);         // 0x00d15b30
Obj* __cdecl SP_PropertyManager();                          // 0x0067de30
u32  __cdecl EA_Hash_FNV1_String16(const wchar_t* pText, u32 nSeed, int nLower); // 0x00932f30
extern const wchar_t DAT_01479b30[];                        // 0x01479b30 L"LevelEditor"

// ---- noun manager definition table -----------------------------------------
// eastl::hash_map<u64, SP::cNounDefinition>: pair in the node, chain link last.
struct cNounDefinition {
    u32  mClassID;                                          // 0x00
    u32  mDefinitionID;                                     // 0x04
    u32  mResourceID;                                       // 0x08
    void* mpPropertyList;                                   // 0x0c
};

struct DefinitionEntry {
    u64             mKey;                                   // 0x00
    cNounDefinition mValue;                                 // 0x08
};

struct HashNode {
    DefinitionEntry mValue;                                 // 0x00
    HashNode*       mpNext;                                 // 0x18
};

struct HashIterator;

struct Hashtable {
    u32         mFunctors;                                  // 0x00
    HashNode**  mpBucketArray;                              // 0x04
    u32         mnBucketCount;                              // 0x08
    u32         mnElementCount;                             // 0x0c
    u32         mPad10[4];                                  // 0x10

    void Begin(HashIterator* pOut);                         // 0x00594410
};

struct HashIterator {
    HashNode*  mpNode;                                      // 0x00
    HashNode** mpBucket;                                    // 0x04
};

struct cGameNounManager {
    u8        mPad00[0x20];
    Hashtable mDefinitions;                                 // 0x020
};

cGameNounManager* __cdecl SP_NounManager();                 // 0x00b3d300

// ---- vtable dispatch -------------------------------------------------------
struct IRefCounted {
    virtual void AddRef();                                  // 0x00
    virtual void Release();                                 // 0x04
};

static inline bool Call_FindProperty(void* pObj, u32 nHash, void** ppOut)
{
    return ((bool (__thiscall*)(void*, u32, void**))(*(void***)pObj)[0x30 / 4])(pObj, nHash, ppOut);
}

static inline void* Call_GetChild(void* pObj, u32 nID)
{
    return ((void* (__thiscall*)(void*, u32))(*(void***)pObj)[0x0c / 4])(pObj, nID);
}

static inline void Call_AddRef(void* pObj)
{
    ((void (__thiscall*)(void*))(*(void***)pObj)[0])(pObj);
}

static inline void Call_Release(void* pObj)
{
    ((void (__thiscall*)(void*))(*(void***)pObj)[0x04 / 4])(pObj);
}

static inline void* Call_GetSelection(void* pObj)
{
    return ((void* (__thiscall*)(void*))(*(void***)pObj)[0x40 / 4])(pObj);
}

// Drops a reference the caller holds in `pRef` and clears the slot first, the
// way the shipped code leaves the property slot empty before it is filled in.
static inline void ReleaseSlot(void*& pRef)
{
    if (pRef != 0) {
        void* pOld = pRef;
        pRef = 0;
        Call_Release(pOld);
    }
}

// @ 0x00d16820
u32 cGameEditInputStrategy::FUN_00d16820()
{
    u32 markerType = GetGameplayMarkerTypeFromUI();
    u32 classId = GetGameplayMarkerClassId(markerType);

    void* pProperty = 0;
    Obj* pManager = SP_PropertyManager();
    ReleaseSlot(pProperty);

    u32 hName = EA_Hash_FNV1_String16(DAT_01479b30, 0x811c9dc5, 1);

    if (Call_FindProperty(pManager, hName, &pProperty)) {
        IWindow* pWindow = mLayout.FindWindowByID(0xabc0fff9, 1);
        if (pWindow != 0) {
            Obj* pCombo = (Obj*)Call_GetChild(pWindow, 0x2f5528d9);
            if (pCombo != 0) {
                Call_AddRef(pCombo);
                u32 nSelection = (u32)Call_GetSelection(pCombo);

                cGameNounManager* pNouns = SP_NounManager();
                Hashtable* pTable = &pNouns->mDefinitions;

                HashIterator iter;
                pTable->Begin(&iter);

                HashNode* pEnd = pTable->mpBucketArray[pTable->mnBucketCount];
                HashNode* pNode = iter.mpNode;
                u32 nMatch = 0;

                if (pNode != pEnd) {
                    // Walking the bucket array only advances the local copy,
                    // exactly like the shipped iteration loop.
                    HashNode** pBucket = iter.mpBucket;
                    for (;;) {
                        if (classId == pNode->mValue.mValue.mClassID) {
                            if (nMatch == nSelection) {
                                u32 nResult = pNode->mValue.mValue.mDefinitionID;
                                Call_Release(pCombo);
                                if (pProperty != 0)
                                    Call_Release(pProperty);
                                return nResult;
                            }
                            ++nMatch;
                        }
                        pNode = pNode->mpNext;
                        if (pNode == 0) {
                            do {
                                pNode = *++pBucket;
                            } while (pNode == 0);
                        }
                        if (pNode == pEnd)
                            break;
                    }
                }

                Call_Release(pCombo);
            }
        }
    }

    if (pProperty != 0)
        Call_Release(pProperty);
    return 0;
}
