// @ 0x00929bf0
#include "game/types.h"
#include "EA/Thread/Mutex.h"

extern unsigned g_timeNone; // 0x0143e340

namespace EA { namespace CallbackSystem {
struct Callback;
struct Entry {
    int nextTime;
    int lastTime;
    Callback* cb;
    bool oneShot;
    char pad1, pad2, pad3;
};
struct EntryVec {
    Entry* b;
    Entry* e;
    Entry* c;
};
// CallbackProcessor layout shared with UTFSpore_Win32/EACallback.cpp.
struct CallbackProcessor {
    void* vtable;
    EntryVec mEntries;
    int pad10[2];
    void* mpStopwatch;
    int* mpCounterA;
    int* mpCounterB;
    bool mbRunning;
    bool mbThreaded;
    unsigned mRandom;
    float mfScale;
    int mLast0;
    int mLast1;
    int mNextDue0;
    int mNextDue1;
    EA::Thread::Mutex mMutex;
    void* mpThread;

    bool Remove(Callback* cb);
};

bool CallbackProcessor::Remove(Callback* cb)
{
    mMutex.Lock(g_timeNone);
    unsigned count = (unsigned)(mEntries.e - mEntries.b);
    unsigned i = 0;
    if (count > 0) {
        Entry* base = mEntries.b;
        Entry* p = base;
        while (p->cb != cb) {
            ++i;
            ++p;
            if (i >= count) {
                mMutex.Unlock();
                return true;
            }
        }
        base[i].cb = 0;
    }
    mMutex.Unlock();
    return true;
}
}}
