// SP::cPlayer war-flag getters: 0x00c7a6c0 (bit 0) .. 0x00c7a890 (bit 5).
// All six share one body; only the bit index differs, so it is an inlined
// helper here.  Retail inlines it and tests the bit with a byte-wide `and al`.
#include "types.h"

// Element of the sorted (key, value) flag table reached through the player at
// +0x11d8 (begin) / +0x11dc (end).  mValue carries one bit per war flag.
struct cFlagPair {
    uint32_t mKey;                          // +0x00
    uint32_t mValue;                        // +0x04
};

// 0x00d01260: lower_bound over the player's sorted flag entries.
cFlagPair* lowerBound(cFlagPair* begin, cFlagPair* end,
                      const uint32_t* key, uint8_t compareFlag);

namespace SP {
// Retail view: the generated player header has a different layout here.
struct cPlayer {
    uint8_t mPad0000[0x11d8];
    cFlagPair* mFlagMapRoot;                // +0x11d8
    cFlagPair* mFlagMapEnd;                 // +0x11dc
    uint8_t mPad11e0[0x0c];
    uint8_t mFlagCompareFlag;               // +0x11ec

    __forceinline uint8_t GetFlagBit(const uint32_t& key, uint32_t bit)
    {
        cFlagPair* end = mFlagMapEnd;
        cFlagPair* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
        if (it == end || key < it->mKey || it == it + 1) {
            it = end;
        }
        // The map is re-read at each use (a call may store through the table
        // pointers), which also keeps the receiver in a callee-saved register.
        if (it == mFlagMapEnd) {
            return 0;
        }
        return (it->mValue >> bit) & 1;
    }

    uint8_t GetFlag0(uint32_t key);
};

// @ 0x00c7a6c0  map flag bit 0
uint8_t cPlayer::GetFlag0(uint32_t key)
{
    return GetFlagBit(key, 0);
}
}
