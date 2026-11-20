// @ 0x00c7a830
#include "types.h"
#include <intrin.h>

struct cFlagPair {
    uint32_t mKey;
    uint32_t mValue;
};

// 0x00d01260: lower_bound on the player's sorted flag entries.
cFlagPair* lowerBound(cFlagPair* begin, cFlagPair* end,
                      const uint32_t* key, uint8_t compareFlag);

namespace SP {
// Retail view: the generated player header has a different layout here.
struct cPlayer {
    uint8_t mPad0000[0x11d8];
    cFlagPair* mFlagMapRoot;
    cFlagPair* mFlagMapEnd;
    uint8_t mPad11e0[0x0c];
    uint8_t mFlagCompareFlag;

    uint8_t GetFlag4(uint32_t key);
};

uint8_t cPlayer::GetFlag4(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    cFlagPair* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    // Both comparisons below use the cached `end`, which keeps it in edi in retail.
    if (it == end || key < it->mKey || it == it + 1) {
        it = end;
    }
    if (it == end) {
        return 0;
    }
    return ((it->mValue >> 4) & 1) != 0;
}
}
