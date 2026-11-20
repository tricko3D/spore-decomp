// SP::cGameNounManager::RemovePoliticalNoun(cGameData* pNoun)
//
// Drops pNoun from the political-ID map (the eastl rbtree at +0xd0, whose
// header sits at +0xd4): look the noun's political ID (vtable slot +0x4c) up
// with the rbtree find at 0xe5c780, and only when the entry's noun really is
// pNoun erase it through 0x1045590.  Finally, if pNoun is the cached +0x70
// reference, clear it and Release() the old object (vtable slot +0x04).
#include "game/types.h"

class cGameData;

struct tRBNode {
    tRBNode*  mpLeft;      // +0x00
    tRBNode*  mpRight;     // +0x04
    tRBNode*  mpParent;    // +0x08
    uint32_t  mColor;      // +0x0c
    uint32_t  mKey;        // +0x10 (political ID)
    cGameData* mpValue;    // +0x14 (noun)
};

// A 4-byte tree iterator, passed by value to erase.
struct tTreeIter {
    tRBNode* mpNode;
    explicit tTreeIter(tRBNode* pNode) : mpNode(pNode) {}
};

struct tRBTree {
    uint32_t mPad00;       // +0x00
    tRBNode  mAnchor;      // +0x04 (end() sentinel)
    uint32_t mnSize;       // +0x14

    tTreeIter find(const uint32_t& rKey);                          // 0x00e5c780
    tTreeIter erase(tTreeIter position);                           // 0x01045590
};

struct tNounMap {
    uint32_t mPad00;       // +0x00
    tRBTree  mTree;        // +0x04
};

class cGameData {
public:
    virtual void     v00();               // +0x00
    virtual void     Release();           // +0x04
    virtual void     v08();               // +0x08
    virtual void     v0c();               // +0x0c
    virtual void     v10();               // +0x10
    virtual void     v14();               // +0x14
    virtual void     v18();               // +0x18
    virtual void     v1c();               // +0x1c
    virtual void     v20();               // +0x20
    virtual void     v24();               // +0x24
    virtual void     v28();               // +0x28
    virtual void     v2c();               // +0x2c
    virtual void     v30();               // +0x30
    virtual void     v34();               // +0x34
    virtual void     v38();               // +0x38
    virtual void     v3c();               // +0x3c
    virtual void     v40();               // +0x40
    virtual void     v44();               // +0x44
    virtual void     v48();               // +0x48
    virtual uint32_t GetPoliticalID();    // +0x4c
};

class cGameNounManager {
public:
    uint8_t    mUnused00[0x70];
    cGameData* mPlayerTribe;              // +0x70
    uint8_t    mUnused74[0xcc - 0x74];
    tNounMap   mPoliticalMap;             // +0xcc

    void FUN_00b21410(cGameData* pNoun);  // 0x00b21410
};

// @ 0x00b21410
void cGameNounManager::FUN_00b21410(cGameData* pNoun)
{
    uint32_t id = pNoun->GetPoliticalID();
    if (id != 0xffffffffu) {
        tRBNode* pNode = mPoliticalMap.mTree.find(id).mpNode;
        if (pNode != &mPoliticalMap.mTree.mAnchor && pNode->mpValue == pNoun) {
            tTreeIter it(pNode);
            mPoliticalMap.mTree.erase(it);
        }
    }
    if (mPlayerTribe == pNoun) {
        cGameData* pOld = mPlayerTribe;
        if (pOld) {
            mPlayerTribe = 0;
            pOld->Release();
        }
    }
}
