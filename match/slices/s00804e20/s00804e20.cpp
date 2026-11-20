// @ 0x00804e20
#include "game/types.h"

namespace UTFWin {
// Secondary sequencer interface, embedded at offset 8.
struct FrameSequencerState {
    void* vtable;
    float mTime;
    float mFrameTime;
    float mDuration;
    float mOOFrameCount;
    float mOOFrameCountPP;
    float mFrameDuration;
    float mOOFrameDuration;
    int mDirection;
    int mEndingMode;
    int mFrameCount;
    int mCurrentFrame;
    int mLoopCount;
    int mCurrentLoop;
    int mStateFlags;
    void* mpFS;
    void* mpVisitor;
    unsigned char mContext[16];
    unsigned short mFlags;
    unsigned short mFlags2;

    bool SetSequence(void* sequence); // 0x00804920
};

struct cSPUIFrameSequencer {
    void* vtable;
    int mRefCount;
    FrameSequencerState mSequencer;
    cSPUIFrameSequencer();
};

cSPUIFrameSequencer::cSPUIFrameSequencer() {
    mRefCount = 0;
    // The base-interface table is installed before the derived tables.
    *(void* volatile*)&mSequencer.vtable = (void*)0x01417718;
    vtable = (void*)0x01417804;
    mSequencer.vtable = (void*)0x01417790;
    mSequencer.mFrameCount = 0;
    mSequencer.mCurrentFrame = 0;
    mSequencer.mLoopCount = 0;
    mSequencer.mCurrentLoop = 0;
    mSequencer.mpFS = 0;
    mSequencer.mpVisitor = 0;
    mSequencer.mFlags = 0;
    mSequencer.mFlags2 = 0;
    mSequencer.SetSequence(0);
}
}
