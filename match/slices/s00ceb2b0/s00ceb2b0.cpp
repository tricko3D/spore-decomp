// s00ceb2b0 -- intrusive map lookup-or-insert returning the mapped value.
//
// MSVC 2008 SP1, /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// The map is a red-black tree of reference counted keys: the anchor (end) node
// sits at this+0x04, the root of the tree at this+0x0c (the anchor's +0x08
// slot), the key of a node at +0x10 and its value at +0x14.  The lookup is a
// lower-bound descent; when the key is present the address of the stored value
// is returned, otherwise 0x00ceab60 inserts it and the value of the inserted
// node is returned.  The key is a reference counted pointer cell, so a
// temporary copy is kept alive around the insertion (AddRef/Release are the
// first two vtable slots of the pointee).
#include "game/types.h"

struct MapNode;

// Anything stored in this map is reference counted through its vtable.
struct RefCounted {
    virtual void AddRef();              // vtable +0x00
    virtual void Release();             // vtable +0x04
};

// Reference counting pointer cell; the map keys are cells of this shape.
struct RefPtr {
    RefCounted* mpRef;                  // +0x00

    RefPtr() : mpRef(0) {}
    RefPtr(const RefPtr& rhs) : mpRef(rhs.mpRef)
    {
        if (mpRef != 0)
            mpRef->AddRef();
    }
    ~RefPtr()
    {
        if (mpRef != 0)
            mpRef->Release();
    }
};

// One node of the tree.  The anchor node at this+0x04 uses the same layout;
// only its +0x08 slot (the root) is meaningful, the rest is the end marker.
struct MapNode {
    MapNode*    mpMore;                 // +0x00  subtree of greater keys
    MapNode*    mpLess;                 // +0x04  subtree of smaller keys
    MapNode*    mpRoot;                 // +0x08  anchor only: tree root
    void*       mPad0c;                 // +0x0c
    RefCounted* mpKey;                  // +0x10
    RefPtr      mValue;                 // +0x14
};

// Out-parameter block used by the insertion: the resulting node and the
// iterator it was found/created at.
struct MapInsertPos {
    MapNode* mpNode;                    // +0x00
    void*    mpOwner;                   // +0x04
};

class IntrusiveMap {
public:
    char     mPad00[4];                 // +0x00
    MapNode  mAnchor;                   // +0x04  end node / root holder

    RefPtr* LookupOrInsert(const RefPtr& key);
};

// 0x00ceab60: insert `key` at/next to `position`, writing the resulting node
// to `result`.
extern void __fastcall FUN_00ceab60(void* self, MapNode** result,
                                    MapNode* position, const RefPtr* key,
                                    int flag);

// ------------------------------------------------------------------ 0x00ceb2b0
// @ 0x00ceb2b0
RefPtr* IntrusiveMap::LookupOrInsert(const RefPtr& key)
{
    MapNode* bound = &mAnchor;
    MapNode* node = mAnchor.mpRoot;

    while (node != 0) {
        if (node->mpKey < key.mpRef) {
            node = node->mpMore;
        } else {
            bound = node;
            node = node->mpLess;
        }
    }

    MapNode* result;

    if (bound != &mAnchor && key.mpRef >= bound->mpKey) {
        result = bound;
    } else {
        RefPtr   copy(key);
        MapNode* inserted = 0;

        FUN_00ceab60(this, &inserted, bound, &copy, 0);
        result = inserted;
    }

    return &result->mValue;
}
