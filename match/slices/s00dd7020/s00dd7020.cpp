// Slice s00dd7020 -- SP::cSPUIEventLog constructor.
// 32-bit x86, MSVC 2008 SP1, flags /O2 /MD /Gy /TP /arch:SSE /GS-

#include <string.h>

#include "game/types.h"
#include "SP/cGonzagoSubsystem.h"

// ---------------------------------------------------------------------------
// The window-procedure base of the event log (only its vptr is relevant here).
// ---------------------------------------------------------------------------
struct IWinProc {
    virtual void v00();
    virtual void v04();
};

// ---------------------------------------------------------------------------
// Property manager / property-list interfaces used while preparing the
// event log's tuning properties and its animator.
// ---------------------------------------------------------------------------
struct IProperty {
    virtual void AddRef();
    virtual void Release();
};

struct PropertyManager {
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
    virtual bool GetProperty(unsigned int id, void** out);   // +0x30
};

struct PropertyList {
    void AddRef(IProperty* p);
};

extern PropertyManager* PropertyManager_get();     // 0x0067de30
extern PropertyList      g_015a2ca8;              // 0x015a2ca8
extern float             g_016a09bc;              // 0x016a09bc
extern const char        g_013f6b3c[];            // 0x013f6b3c

void* operator new(unsigned int size, const char* name, int flags, int align,
                   int file, int line);           // 0x00f473a0

// ---------------------------------------------------------------------------
// cSPUIEventLog layout (retail offsets).
// ---------------------------------------------------------------------------
struct RBNode {
    RBNode*  mpNodeLeft;    // +0x00
    RBNode*  mpNodeRight;   // +0x04
    RBNode*  mpNodeParent;  // +0x08
    unsigned char mColor;   // +0x0c
    char     pad0d[3];      // +0x0d
};

struct EventMap {
    unsigned mCompare;      // +0x00
    RBNode   mAnchor;       // +0x04
    unsigned mnSize;        // +0x14
    unsigned mAllocator;    // +0x18

    void reset()
    {
        memset(&mAnchor, 0, sizeof(mAnchor));
        mAnchor.mpNodeLeft = &mAnchor;
        mAnchor.mpNodeRight = &mAnchor;
        mAnchor.mpNodeParent = 0;
        mAnchor.mColor = 0;
        mnSize = 0;
    }
};

class cSPUIAnimator {
 public:
    char mData[0x20];
    cSPUIAnimator();
};

class cSPUIEventLog : public IWinProc, public SP::cGonzagoSubsystem {
 public:
    unsigned       mGlobalAlpha;      // +0x20
    bool           mbVisible;         // +0x24
    char           pad25[3];          // +0x25
    EventMap       mEvents;           // +0x28
    unsigned       mLastEventEntered; // +0x44
    float          mLastUpdateTime;   // +0x48
    float          mLogPosition[2];   // +0x4c
    float          mLogOffset[2];     // +0x54
    float          mLogTop;           // +0x5c
    float          mLogTopCurrent;    // +0x60
    int            mHistorySize;      // +0x64
    cSPUIAnimator* mpAnimator;        // +0x68
    IProperty*     mpTuningProps;     // +0x6c

    cSPUIEventLog();
};

// @ 0x00dd7020
cSPUIEventLog::cSPUIEventLog()
{
    mGlobalAlpha = 0xff;
    mbVisible = true;

    // EventMap::reset: clear the anchor's right/parent links, its colour and
    // the trailing padding, then make the empty tree point at itself.
    mEvents.reset();

    mLastEventEntered = 0;
    mLastUpdateTime = 0.0f;
    mHistorySize = 0;

    // The tuning-property slot goes through its address: retail keeps the slot
    // pointer in edi across the manager lookup and reuses it for every access.
    IProperty** slot = &mpTuningProps;
    *slot = 0;
    PropertyManager* pm = PropertyManager_get();
    if (*slot) {
        IProperty* old = *slot;
        *slot = 0;
        old->Release();
    }
    if (pm->GetProperty(0x5c770db7, (void**)slot))
        g_015a2ca8.AddRef(*slot);

    mLogTopCurrent = g_016a09bc;
    mLogTop = mLogTopCurrent;
    mpAnimator = new (g_013f6b3c, 0, 0, 0, 0) cSPUIAnimator();
}
