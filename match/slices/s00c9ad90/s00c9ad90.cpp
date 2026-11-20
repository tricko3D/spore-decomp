// Slice s00c9ad90 -- register the fishing hot-spot class on every hot spot of a map.
// Tribe module: /O2 /MD /Gy /TP /arch:SSE /fp:fast, no /EHsc.
//
// Looks `map` up in the global style table at 0x0152d69c.  When the lookup fails the call returns true straight
// away; otherwise (and only when a map was passed) each hot spot of the map, found by its dense key, has the
// 0x0157b7d0 attribute description applied under the name L"cFishingHotSpot".  The function always reports true.
#include "types.h"

// ------------------------------------------------------------------ hot spots
struct HotSpot { char mPad[0x58]; };
struct HotSpotElem {               // stride 0x60
    unsigned mKey;                 // +0x00
    unsigned mPad;                 // +0x04
    HotSpot  mValue;               // +0x08
};
struct HotSpotMap {
    HotSpotElem* mpBegin;          // +0x00
    HotSpotElem* mpEnd;            // +0x04
    HotSpot* at(const unsigned* key);                        // 0x00c9aa10 (thiscall, ret 4)
};

// ------------------------------------------------------------------ global style table
struct StyleHTNode;
struct StyleHTIter {
    StyleHTNode* mpNode;           // +0x0
    void*        mpBucket;         // +0x4
};
struct StyleMap {                  // eastl::hashtable<unsigned, TextStyleEx*> at 0x0152d69c
    char           mPad00[0x04];   // +0x0
    StyleHTNode**  mpBucketArray;  // +0x4  (0x0152d6a0)
    unsigned       mBucketCount;   // +0x8  (0x0152d6a4)
    // 0x00645ed0: thiscall, ret 8; the key is passed by reference (its address is pushed).
    void Find(StyleHTIter* out, const HotSpotMap* const& key);
    StyleHTNode* End() { return mpBucketArray[mBucketCount]; }
};
extern StyleMap g_StyleMap;        // 0x0152d69c

extern char g_SimAttributeTable[]; // 0x0157b7d0 (Simulator::Attribute descriptions)

struct HotSpotClassOwner {
    // 0x00695b40 (thiscall, ret 0xc): apply one attribute description to an object.
    void ApplyAttribute(HotSpot* object, void* attrs, const wchar_t* name);
};

// @ 0x00c9ad90
bool RegisterFishingHotSpotClasses(HotSpotClassOwner* owner, HotSpotMap* map)
{
    {
        StyleHTIter it;
        g_StyleMap.Find(&it, map);
        if (it.mpNode == g_StyleMap.End())
            return true;
    }
    if (!map)
        return true;

    unsigned count = (unsigned)(map->mpEnd - map->mpBegin);
    for (unsigned i = 0; i < count; ++i) {
        HotSpot* spot = map->at(&i);
        owner->ApplyAttribute(spot, g_SimAttributeTable, L"cFishingHotSpot");
    }
    return true;
}
