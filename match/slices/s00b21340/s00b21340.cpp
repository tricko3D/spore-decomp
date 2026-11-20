// SP::cGameNounManager::GetGameDataVector
//   tGameDataVector* (__cdecl*)(void)                          creator
//   void (__cdecl*)(tGameDataVector*)                          clear
//   void (__cdecl*)(tGameDataVector*, cGameData*)              add
//   bool (__cdecl*)(cGameData*, unsigned int)                  filter
//   unsigned int                                               typeID
// thiscall, ret 0x14.
//
// Look the typeID up in the game-data-vector map (the eastl rbtree based at
// +0x98, its end() sentinel at +0x9c).  When the entry is absent, build a
// fresh vector through the caller's creator and insert (typeID, vector) with
// the out-of-line rbtree insert at 0xba8420.  When the vector is flagged
// dirty, run the caller's clear function, add every noun of the +0x78
// intrusive list that the caller's filter accepts (the intrusive node of a
// cGameData sits at +0xc) through the caller's add function, and drop the
// flag.
#include "game/types.h"

class cGameData;

// eastl intrusive list node, embedded at +0xc of cGameData.
struct tListNode {
    tListNode* mpNext;     // +0x00
    tListNode* mpPrev;     // +0x04
};

// eastl rbtree node: header, the 32-bit type key at +0x10, the vector at +0x14.
struct tRBNode {
    tRBNode*   mpRight;    // +0x00
    tRBNode*   mpLeft;     // +0x04
    tRBNode*   mpParent;   // +0x08
    uint32_t   mColor;     // +0x0c
    uint32_t   mKey;       // +0x10
    void*      mpValue;    // +0x14
};

// The typeID -> vector map.  End() is the address of the sentinel at +0x04.
struct tVectorMap {
    uint32_t mPad00;       // +0x00
    tRBNode  mAnchor;      // +0x04 (end() sentinel)
    uint32_t mnSize;       // +0x14
    uint32_t mPad18;       // +0x18
    uint32_t mPad1c;       // +0x1c
    void* End() { return (void*)&mAnchor; }
    void  Find(void* pOut, void* pKey);                 // 0x00e5c780
    void  Insert(void* pOut, void* pValue, bool bFlag); // 0x00ba8420
};

// 0x10-byte argument block of the insert: the pair to insert first, the
// 8-byte {node, inserted} result behind it.
struct tInsertArgs {
    uint32_t mKey;         // +0x00
    void*    mpVector;     // +0x04
    void*    mpNode;       // +0x08 (written by Insert)
    uint32_t mInserted;    // +0x0c (written by Insert)
};

// eastl intrusive list anchor (the +0x78 noun list).
struct tNounList {
    tListNode mAnchor;     // +0x00
};

struct tGameDataVector {
    bool dirty;            // +0x00
};

// The embedded node of a cGameData, at +0xc.
inline cGameData* ToNoun(tListNode* pNode) {
    return pNode ? (cGameData*)((char*)pNode - 0xc) : 0;
}
inline tListNode* ToNode(cGameData* pNoun) {
    return (tListNode*)((char*)pNoun + 0xc);
}

struct tGameNounManager {
    uint8_t  mPad00[0x78];     // +0x00
    tNounList mNouns;          // +0x78
    uint8_t  mPad80[0x18];     // +0x80
    tVectorMap mGameDataVectorMap;  // +0x98

    tGameDataVector* GetGameDataVector(
        tGameDataVector* (__cdecl* pCreate)(void),
        void (__cdecl* pClear)(tGameDataVector*),
        void (__cdecl* pAdd)(tGameDataVector*, cGameData*),
        bool (__cdecl* pFilter)(cGameData*, unsigned int),
        unsigned int typeID);
};

// @ 0x00b21340
tGameDataVector* tGameNounManager::GetGameDataVector(
    tGameDataVector* (__cdecl* pCreate)(void),
    void (__cdecl* pClear)(tGameDataVector*),
    void (__cdecl* pAdd)(tGameDataVector*, cGameData*),
    bool (__cdecl* pFilter)(cGameData*, unsigned int),
    unsigned int typeID)
{
    void* pFound;
    tGameDataVector* pVector;

    mGameDataVectorMap.Find(&pFound, &typeID);
    if (pFound != mGameDataVectorMap.End()) {
        pVector = (tGameDataVector*)((tRBNode*)pFound)->mpValue;
    } else {
        pVector = pCreate();
        tInsertArgs args;
        args.mKey = typeID;
        args.mpVector = pVector;
        mGameDataVectorMap.Insert(&args.mpNode, &args.mKey, false);
    }

    if (pVector->dirty) {
        pClear(pVector);
        cGameData* pEnd = ToNoun(&mNouns.mAnchor);
        cGameData* pNoun = ToNoun(mNouns.mAnchor.mpNext);
        while (pNoun != pEnd) {
            if (pFilter(pNoun, typeID))
                pAdd(pVector, pNoun);
            pNoun = ToNoun(ToNode(pNoun)->mpNext);
        }
        pVector->dirty = false;
    }
    return pVector;
}
