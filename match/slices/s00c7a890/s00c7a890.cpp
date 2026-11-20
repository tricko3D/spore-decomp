// @ 0x00c7a890
#include "types.h"

struct cFlagPair {
    uint32_t mKey;
    uint32_t mValue;
};

// Sorted flag-map lower_bound, 0x00d01260.
cFlagPair* lowerBound(cFlagPair* begin, cFlagPair* end,
                      const uint32_t* key, uint8_t compareFlag);

namespace SP {
// Retail layout view; the generated cPlayer header's flag container differs.
struct cPlayer {
    uint8_t mPad0000[0x11d8];
    cFlagPair* mFlagMapRoot;
    cFlagPair* mFlagMapEnd;
    uint8_t mPad11e0[0x0c];
    uint8_t mFlagCompareFlag;

    uint8_t GetFlag5(uint32_t key);
};

uint8_t cPlayer::GetFlag5(uint32_t key)
{
    // `end` is cached across the call (edi); the later member read keeps the
    // receiver live in esi, as retail.
    cFlagPair* end = mFlagMapEnd;
    cFlagPair* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == end || key < it->mKey || it == it + 1) {
        it = end;
    }
    if (it == end) {
        return 0;
    }
    return (it->mValue >> 5) & 1;
}
}
