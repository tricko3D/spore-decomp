// @ 0x00c7a770
#include "types.h"

struct cFlagPair {
    uint32_t mKey;
    uint32_t mValue;
};

// Sorted flag-map lower_bound, 0x00d01260.
uint8_t* lowerBound(cFlagPair* begin, cFlagPair* end,
                    const uint32_t* key, uint8_t compareFlag);

namespace SP {
// Retail layout view; the generated cPlayer header's flag container differs.
struct cPlayer {
    uint8_t mPad0000[0x11d8];
    cFlagPair* mFlagMapRoot;
    cFlagPair* mFlagMapEnd;
    uint8_t mPad11e0[0x0c];
    uint8_t mFlagCompareFlag;

    uint8_t GetFlag2(uint32_t key);
};

uint8_t cPlayer::GetFlag2(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == reinterpret_cast<uint8_t*>(end) ||
        key < reinterpret_cast<cFlagPair*>(it)->mKey || it + 8 == it) {
        it = reinterpret_cast<uint8_t*>(end);
    }
    if (it == reinterpret_cast<uint8_t*>(end)) {
        return 0;
    }
    return (reinterpret_cast<cFlagPair*>(it)->mValue >> 2) & 1;
}
}
