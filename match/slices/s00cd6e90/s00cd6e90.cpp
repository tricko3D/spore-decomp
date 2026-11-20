// @ 0x00cd6e90
#include "game/types.h"
#include <intrin.h>

namespace SP {
struct TribeObjectNode {
    unsigned int mKey;
    void* mValue;
    TribeObjectNode* mpNext;
};
struct TribeObjectTable {
    void* mpPolicy;
    TribeObjectNode** mpBuckets;
    unsigned int mnBucketCount;
    unsigned int mnElementCount;
};
class cTribeModeStrategy {
public:
    void ClearObjectPointers();
    unsigned char mUnknown[0xa8];
    TribeObjectTable mObjects;
};

void cTribeModeStrategy::ClearObjectPointers()
{
    TribeObjectNode** bucket = mObjects.mpBuckets;
    TribeObjectNode* node = *bucket;
    while (!node) {
        ++bucket;
        node = *bucket;
    }
    _ReadWriteBarrier();
    while (node != mObjects.mpBuckets[mObjects.mnBucketCount]) {
        node->mValue = 0;
        node = node->mpNext;
        if (!node) {
            do {
                node = *++bucket;
            } while (!node);
        }
    }
}
}
