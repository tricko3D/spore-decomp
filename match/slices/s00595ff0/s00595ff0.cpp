// @ 0x00595ff0
#include "game/types.h"

namespace SP {
struct ItemRowValue {
    uint64_t first;
    uint64_t second;
    ItemRowValue(const uint64_t& key, const uint64_t& value)
        : first(key), second(value) {}
};
struct ItemRowNode {
    ItemRowValue mValue;
    ItemRowNode* mpNext;
};
struct ItemRowIterator {
    ItemRowNode* mpNode;
    ItemRowNode** mpBucket;
};
struct ItemRowInsertResult {
    ItemRowIterator first;
    bool second;
};
struct ItemRowInsertUniqueTag {};
class ItemRowMap {
public:
    uint32_t mHashCodeBase;
    ItemRowNode** mpBucketArray;
    uint32_t mnBucketCount;

    ItemRowIterator find(const uint64_t& key); // 0x00a7bf40
    ItemRowInsertResult DoInsertValue(const ItemRowValue& value,
                                     ItemRowInsertUniqueTag tag); // 0x00594d50
    uint64_t& index(const uint64_t& key);
};

// @ 0x00595ff0
uint64_t& ItemRowMap::index(const uint64_t& key)
{
    ItemRowIterator it = find(key);
    if (it.mpNode == mpBucketArray[mnBucketCount])
        return DoInsertValue(ItemRowValue(key, uint64_t()), ItemRowInsertUniqueTag()).first.mpNode->mValue.second;
    return it.mpNode->mValue.second;
}
}
