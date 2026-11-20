// @ 0x00595eb0
#include "game/types.h"

namespace SP {
struct ItemStatusValue {
    uint64_t first;
    uint8_t second;
    ItemStatusValue(const uint64_t& key, const uint8_t& value)
        : first(key), second(value) {}
};
struct ItemStatusNode {
    ItemStatusValue mValue;
    ItemStatusNode* mpNext;
};
struct ItemStatusIterator {
    ItemStatusNode* mpNode;
    ItemStatusNode** mpBucket;
};
struct ItemStatusInsertResult {
    ItemStatusIterator first;
    bool second;
};
struct InsertUniqueTag {};
class ItemStatusMap {
public:
    uint32_t mHashCodeBase;
    ItemStatusNode** mpBucketArray;
    uint32_t mnBucketCount;

    ItemStatusIterator find(const uint64_t& key); // 0x00a7bf40
    ItemStatusInsertResult DoInsertValue(const ItemStatusValue& value,
                                        InsertUniqueTag tag); // 0x00594d50
    uint8_t& index(const uint64_t& key);
};

// @ 0x00595eb0
uint8_t& ItemStatusMap::index(const uint64_t& key)
{
    ItemStatusIterator it = find(key);
    if (it.mpNode == mpBucketArray[mnBucketCount])
        return DoInsertValue(ItemStatusValue(key, uint8_t()), InsertUniqueTag()).first.mpNode->mValue.second;
    return it.mpNode->mValue.second;
}
}
