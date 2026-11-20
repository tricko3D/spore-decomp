// @ 0x01012c20
// eastl::map<uint32_t, uint64_t>::operator[] -- rbtree lower_bound, then DoInsertValue
// (0x01011280) when the key is absent.  Node layout mirrors eastl/map.h: the root lives in
// mAnchor.mpNodeParent (this + 0xc), the pair sits at node +0x10 (key) / +0x18 (value).
// The object label is `MapU32U64::At` only because tools/difftest/slice.py:_name_matches cannot
// match `??A...` (operator[]) symbols; the retail symbol is the map's operator[].
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

namespace eastl {

struct NodeBase {
    NodeBase* mpNodeRight;   // +0x00
    NodeBase* mpNodeLeft;    // +0x04
    NodeBase* mpNodeParent;  // +0x08
    char      mColor;        // +0x0c
    char      mPad0d[3];
};

struct Value {
    uint32_t first;          // +0x10
    uint32_t mPad14;         // +0x14
    uint64_t second;         // +0x18
};

struct Node : public NodeBase {
    Value mValue;            // +0x10
};

class MapU32U64 {
 public:
    char      mCompare;      // +0x00
    char      mPad01[3];
    NodeBase  mAnchor;       // +0x04
    uint32_t  mnSize;        // +0x14
    void*     mAllocator;    // +0x18

    uint64_t& At(const uint32_t& key);   // retail: map::operator[]
    NodeBase* DoInsertValue(NodeBase* where, const Value& value);
};

uint64_t& MapU32U64::At(const uint32_t& key)
{
    NodeBase* node  = mAnchor.mpNodeParent;
    NodeBase* where = &mAnchor;

    while (node) {
        if (static_cast<Node*>(node)->mValue.first < key) {
            node = node->mpNodeRight;
        } else {
            where = node;
            node  = node->mpNodeLeft;
        }
    }

    if (where == &mAnchor || key < static_cast<Node*>(where)->mValue.first) {
        Value value = {key, 0, 0};
        where = DoInsertValue(where, value);
    }

    return static_cast<Node*>(where)->mValue.second;
}

}  // namespace eastl
