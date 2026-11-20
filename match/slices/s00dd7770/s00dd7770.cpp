// Slice s00dd7770 -- Simulator::cFeedbackEvent destructor (0x00dd7770).
// 32-bit x86, MSVC 2008 SP1, /O2 /MD /Gy /EHsc /TP /arch:SSE /GS-.
//
// The event owns the event-log layout pool's cSPUILayout (returned to the
// pool and hidden), an auto-refcounted callback target and the event string.
// The class is polymorphic through two bases, so the destructor republishes the
// derived vtables first and, after the members are gone, the two base vtables.
//
// The members are spelled as plain data and destroyed by explicit statements in
// the body: with real member destructors the /EHsc build wraps every call in an
// unwind state (a 65-byte larger function); retail has no frame at all, so the
// destruction sequence is written out here.
#include "game/types.h"
#include "cSPUILayout.h"
#include <new>

// ---------------------------------------------------------------------------
// The event-log layout pool: three pointer fields of an EASTL vector.
// (Same shape as the one in slice s00dd7820.)
// ---------------------------------------------------------------------------
struct EventLogLayoutPool {
    cSPUILayout** mpBegin;
    cSPUILayout** mpEnd;
    cSPUILayout** mpCapacity;

    void Insert(cSPUILayout** position, cSPUILayout* const& layout); // 0x00cd0550

    __forceinline void push_back(cSPUILayout* const& layout)
    {
        if (mpEnd < mpCapacity) {
            cSPUILayout** p = mpEnd++;
            if (p) *p = layout;
        } else {
            Insert(mpEnd, layout);
        }
    }
};
extern EventLogLayoutPool gEventLogLayoutPool; // 0x015a2c40

// ---------------------------------------------------------------------------
// Members.
// ---------------------------------------------------------------------------

// Callback target released by the event: Release is virtual slot 1.
struct EventRefCounted {
    virtual void AddRef();
    virtual void Release();
};

// The event text: an EASTL-style string (begin/end/capacity).
struct EventString {
    wchar_t* mpBegin;             // +0x00
    wchar_t* mpEnd;               // +0x04
    wchar_t* mpCapacity;          // +0x08
};

// ---------------------------------------------------------------------------
// Simulator::cFeedbackEvent : polymorphic base head + content validation.
// (Retail vtables 0x0147ce34 / 0x0147ce24; bases 0x013eb938 / 0x013ec458.)
// ---------------------------------------------------------------------------
struct FeedbackEventBase0 {
    virtual void Slot0();
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual void Slot4();
    virtual ~FeedbackEventBase0() {}
};

struct FeedbackEventBase1 {
    virtual void Slot0();
    virtual void Slot1();
    virtual void Slot2();
    virtual void Slot3();
    virtual void Slot4();
    virtual ~FeedbackEventBase1() {}
};

class cFeedbackEvent : public FeedbackEventBase0, public FeedbackEventBase1 {
 public:
    virtual ~cFeedbackEvent() throw();

    char pad008[0x50];            // 0x08..0x57
    EventRefCounted* mpUserData;  // 0x58
    char pad05c[0x0c];            // 0x5c..0x67
    EventString mText;            // 0x68..0x73
    char pad074[0x04];            // 0x74..0x77
    cSPUILayout* mpLayout;        // 0x78
};

// 0x00dd7770 -- return the event's layout to the pool, hide it, then release
// the string buffer and the callback target.
// @ 0x00dd7770
cFeedbackEvent::~cFeedbackEvent() throw()
{
    cSPUILayout* layout = this->mpLayout;
    gEventLogLayoutPool.push_back(layout);
    layout->SetVisibility(false);

    wchar_t* pText = this->mText.mpBegin;
    int nBytes = (int)((char*)this->mText.mpCapacity - (char*)pText) & ~1;
    if (nBytes > 2 && pText)
        operator delete(pText);

    EventRefCounted* pUserData = this->mpUserData;
    if (pUserData)
        pUserData->Release();
}
