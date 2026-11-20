// cSPUIEventLog: remove/dismiss every event whose owner fields match, by walking mEvents and
// collecting the event ids first (the walk invalidates nothing, removal happens afterwards).
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE
//
// Parameter order is read from the retail body, not from the name: the third stack slot is
// loaded as a dword and compared against the event's +0x54 owner sub id (with a zero test),
// while the second slot is loaded as a byte and tested to choose remove_event (0) versus the
// in-place dismiss (non-zero).  So the flag is the second parameter and the sub id the third.
#include "game/types.h"

// ---- local stand-ins (measured retail layout, same shape as slice s00dd6a70 and
//      src/game/include/eastl/fixed_vector.h); no canonical cSPUIEventLog header exists yet.
// eastl red-black tree node: right child first in this build, then left, parent, colour.
struct EventNodeBase_s00dd_7620 {
    EventNodeBase_s00dd_7620* mpNodeRight;   // +0x00  keys greater than this node
    EventNodeBase_s00dd_7620* mpNodeLeft;    // +0x04  keys smaller
    EventNodeBase_s00dd_7620* mpNodeParent;  // +0x08
    uint32_t                  mnColor;       // +0x0c
};
struct EventNode_s00dd_7620 : EventNodeBase_s00dd_7620 {
    uint32_t mKey;    // +0x10
    void*    mValue;  // +0x14  AutoRefCount<cFeedbackEvent>::mpObject
};
struct EventTree_s00dd_7620 {
    uint32_t                 mPad00;    // +0x00
    EventNodeBase_s00dd_7620 mAnchor;   // +0x04
    uint32_t                 mnSize;    // +0x14

    // eastl rbtree lower_bound: the first node whose key is not less than `key`, or the
    // anchor when there is none (same inline form as EventMap::GetOrInsert in s00dd85c0).
    EventNodeBase_s00dd_7620* find(uint32_t key)
    {
        EventNodeBase_s00dd_7620* y = &mAnchor;
        EventNodeBase_s00dd_7620* x = mAnchor.mpNodeParent;
        while (x != 0) {
            if (((EventNode_s00dd_7620*)x)->mKey >= key) {
                y = x;
                x = x->mpNodeLeft;
            } else {
                x = x->mpNodeRight;
            }
        }
        if (y != &mAnchor && !(key < ((EventNode_s00dd_7620*)y)->mKey))
            return y;
        return &mAnchor;
    }
};

struct cFeedbackEvent_s00dd_7620 {
    void*    mpVtbl;                  // +0x00
    char     pad004[0x08];            // +0x04..+0x0b
    union {
        uint32_t mFlags;              // +0x0c
        struct {
            uint32_t mTextDirty : 1;  // bit 0
            uint32_t mB1        : 1;
            uint32_t mB2        : 1;  // bit 2, still on screen
            uint32_t            : 3;
            uint32_t mB6        : 1;
            uint32_t            : 2;
            uint32_t mInHistory : 1;
            uint32_t mB10       : 1;
            uint32_t mPaused    : 1;
            uint32_t            : 20;
        };
    };
    char     pad010[0x0c];            // +0x10..+0x1b
    int      m1c;                     // +0x1c
    char     pad020[0x10];            // +0x20..+0x2f
    float    mRestartTime;            // +0x30
    float    mDuration;               // +0x34
    char     pad038[0x10];            // +0x38..+0x47
    int      m48;                     // +0x48  event instance id (mEvents key)
    int      m4c;                     // +0x4c  owner id
    int      m50;                     // +0x50
    int      m54;                     // +0x54  owner sub id
    void*    m58;                     // +0x58
    int      m5c;                     // +0x5c
    int      mCloseCallback;          // +0x60
    void (*mCallback)(int, int);      // +0x64
};

// eastl::fixed_vector<uint32_t, 32> with the measured retail layout: begin/end/capacity, the
// overflow allocator word (never written here), the pool pointer, one unused word, then the
// 32 inline slots at +0x18.  The push_back body is the same inline form as slice s00dd7770.
struct EventIdList_s00dd_7620 {
    uint32_t* mpBegin;        // +0x00
    uint32_t* mpEnd;          // +0x04
    uint32_t* mpCapacity;     // +0x08
    void*     mpAllocator;    // +0x0c  never written by the constructor
    uint32_t* mpPoolBegin;    // +0x10  points at mBuffer
    uint32_t  m14;            // +0x14  never written by the constructor
    uint32_t  mBuffer[32];    // +0x18

    void Insert(uint32_t* position, const uint32_t& value);   // 0x00cd0550

    __forceinline void push_back(const uint32_t& value)
    {
        if (mpEnd < mpCapacity) {
            uint32_t* p = mpEnd++;
            if (p) *p = value;
        } else {
            Insert(mpEnd, value);
        }
    }
};

EventNodeBase_s00dd_7620* eastl_RBTreeIncrement(EventNodeBase_s00dd_7620*);  // 0x00921580

struct cSPUIEventLog_s00dd_7620 {
    void*                mpVtbl;         // +0x00
    char                 pad004[0x1c];   // +0x04..+0x1f
    uint32_t             mGlobalAlpha;   // +0x20
    uint8_t              mbVisible;      // +0x24
    char                 pad025[3];
    EventTree_s00dd_7620 mEvents;        // +0x28..+0x3f

    void remove_event(unsigned id);                                       // 0x00dd6bc0
    bool RemoveMatchingEvents(unsigned ownerID, char immediate, unsigned subID);
};

// @ 0x00dd7620
bool cSPUIEventLog_s00dd_7620::RemoveMatchingEvents(unsigned ownerID, char immediate, unsigned subID)
{
    EventNode_s00dd_7620* node = (EventNode_s00dd_7620*)this->mEvents.mAnchor.mpNodeLeft;
    EventIdList_s00dd_7620 ids;
    ids.mpCapacity  = ids.mBuffer + 32;
    ids.mpPoolBegin = ids.mBuffer;
    ids.mpEnd       = ids.mBuffer;
    ids.mpBegin     = ids.mBuffer;

    volatile bool matched = false;
    if (node != &this->mEvents.mAnchor) {
        do {
            cFeedbackEvent_s00dd_7620* ev = (cFeedbackEvent_s00dd_7620*)node->mValue;
            if (ev->m4c == ownerID && (subID == 0 || ev->m54 == subID))
                ids.push_back(*(const uint32_t*)&ev->m48);
            node = (EventNode_s00dd_7620*)eastl_RBTreeIncrement(node);
        } while ((EventNodeBase_s00dd_7620*)node != &this->mEvents.mAnchor);
    }

    if (ids.mpBegin != ids.mpEnd) {
        matched = true;
        while (ids.mpBegin != ids.mpEnd) {
            uint32_t id = ids.mpEnd[-1];
            if (immediate == 0) {
                this->remove_event(id);
            } else {
                // dismiss in place: clear the on-screen bit and stop its timer
                EventNodeBase_s00dd_7620* found = this->mEvents.find(id);
                if (found != &this->mEvents.mAnchor) {
                    cFeedbackEvent_s00dd_7620* ev =
                        (cFeedbackEvent_s00dd_7620*)((EventNode_s00dd_7620*)found)->mValue;
                    if (ev != 0) {
                        ev->mB2 = 0;   // clear the still-on-screen bit
                        ev->mDuration = 0.0f;
                    }
                }
            }
            ids.mpEnd -= 1;
        }
    }

    if (ids.mpBegin != 0 && ids.mpBegin != ids.mpPoolBegin)
        operator delete[](ids.mpBegin);
    return matched;
}
