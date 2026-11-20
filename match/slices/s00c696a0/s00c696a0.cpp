// @ 0x00c696a0
#include "game/types.h"

struct Node_s00bc_8ff0;
struct Cursor;

struct Stimuli {
    unsigned char mStorage[0x1c8];
    unsigned int* UpdateStimuli(float dt); // 0x00bc9980
};

bool __cdecl WalkTree(float dt, double elapsed, Node_s00bc_8ff0* root,
                      int owner, Cursor* cursor, int state, int flags,
                      int stimuli); // 0x00bc8ff0

namespace Simulator {
struct NanoDroneBehavior {
    Node_s00bc_8ff0* mRoot;
    unsigned int mReserved;
    Stimuli mStimuli;
    unsigned char mCursorStorage[0x418];
    int mState;
    unsigned char mBeforeFlags[0x10];
    int mFlags;
};

class cNanoDrone {
public:
    unsigned char mBeforeBehavior[0x1d0];
    NanoDroneBehavior* mBehavior;

    bool UpdateBehavior(float dt, double elapsed);
};

bool cNanoDrone::UpdateBehavior(float dt, double elapsed)
{
    if (mBehavior && mBehavior->mRoot) {
        int flags = mBehavior->mFlags;
        unsigned int* stimuli = mBehavior->mStimuli.UpdateStimuli(dt);
        return WalkTree(dt, elapsed, mBehavior->mRoot, (int)this,
                        reinterpret_cast<Cursor*>(mBehavior->mCursorStorage),
                        mBehavior->mState, flags, (int)stimuli);
    }
    return false;
}
}
