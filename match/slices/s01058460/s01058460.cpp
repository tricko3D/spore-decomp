// @ 0x01058460
// SPGonzago/SPToolStrategies.obj: unlock every known-but-locked item listed by a
// cItemDataTable resource, then unlock one random item of category 0xa0325a4f.
#include "game/types.h"

// ---- addresses (declarations only; call targets are relocations) -----------
// 0x00b3d300 SP::NounManager()
// 0x00f67d90 cGameNounManager::GetCurrentTerrainSphere()
// 0x004bafc0 ResolveResource(uint32_t key, void** pResult)
// 0x00593980 MakeItemId(uint32_t hi, uint32_t lo)  -> (hi << 32) | lo
// 0x005939c0 cCollectableItems::GetDefaultItemGroupId()
// 0x005951f0 cCollectableItems::IsKnownItem(uint64_t)
// 0x00595110 cCollectableItems::IsItemUnlocked(uint64_t)
// 0x00596da0 cCollectableItems::UnlockItem(uint64_t, int cost)
// 0x00596fc0 cCollectableItems::GetItemsInCategory(sp_vector<uint64_t>&, uint32_t)
// 0x00a68fb0 RandomLinearCongruential::RandomUint32Uniform(unsigned)
// 0x00f47380 operator delete[](void*)

struct cGameNounManager {
    void* GetCurrentTerrainSphere();      // 0x00f67d90
};
cGameNounManager* NounManager();          // 0x00b3d300

void operator_delete_array(void* p);      // 0x00f47380

// eastl::fixed_vector<uint64_t,128> retail layout: 0x18 header, buffer at +0x18.
class fixed_vector_u64 {
public:
    uint64_t* mpBegin;                    // +0x00
    uint64_t* mpEnd;                      // +0x04
    uint64_t* mpCapacity;                 // +0x08
    uint32_t mAllocator;                  // +0x0c
    void* mpPoolBegin;                    // +0x10
    uint32_t mUnknown14;                  // +0x14
    __declspec(align(8)) uint64_t mBuffer[128];   // +0x18

    fixed_vector_u64()
        : mpBegin(mBuffer), mpEnd(mBuffer), mpCapacity(mBuffer + 128), mUnknown14(0) {}
    ~fixed_vector_u64()
    {
        if (mpBegin && *(int*)((char*)mpBegin - 4) != 0)
            operator_delete_array(mpBegin);
    }
    void reserve(uint32_t n);             // 0x004756f0
};

// eastl::vector<uint64_t, sp_vector_allocator>
class sp_vector_u64 {
public:
    uint64_t* mpBegin;
    uint64_t* mpEnd;
    uint64_t* mpCapacity;

    sp_vector_u64() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    ~sp_vector_u64()
    {
        if (mpBegin && *(int*)((char*)mpBegin - 4) != 0)
            operator_delete_array(mpBegin);
    }
};

// refcounted interface: AddRef is slot 0, Release is slot 1.
class IRefCountedObject {
public:
    virtual int AddRef();
    virtual int Release();
};

struct cItemDataRow {
    char pad_00[0x84];
    uint32_t mIdHigh;                     // +0x84
    uint32_t mIdLow;                      // +0x88
};

class cItemDataTable : public IRefCountedObject {
public:
    char pad_00[0x94];
    cItemDataRow* mRowsBegin;             // +0x98
    cItemDataRow* mRowsEnd;               // +0x9c
};

class cCollectableItems : public IRefCountedObject {
public:
    uint64_t GetDefaultItemGroupId();                 // 0x005939c0
    bool IsKnownItem(uint64_t id);                    // 0x005951f0
    bool IsItemUnlocked(uint64_t id);                 // 0x00595110
    bool UnlockItem(uint64_t id, int cost);           // 0x00596da0
    void GetItemsInCategory(sp_vector_u64& out, uint32_t category);  // 0x00596fc0
};

// EA::AutoRefCount<T>: a single owning pointer, released through the vtable.
template <typename T>
class AutoRefCount {
public:
    T* mpObject;

    AutoRefCount() : mpObject(0) {}
    ~AutoRefCount()
    {
        if (mpObject)
            mpObject->Release();
    }
    T* operator->() const { return mpObject; }
    operator T*() const { return mpObject; }
};

// cTerrainSphere holds the collectables pointer at +0x10e8.
struct cTerrainSphere {
    char pad_0000[0x10e8];
    AutoRefCount<cCollectableItems> mpItems;   // +0x10e8
};

uint64_t MakeItemId(uint32_t hi, uint32_t lo);                    // 0x00593980
bool ResolveResource(uint32_t key, void** pResult);               // 0x004bafc0

class RandomLinearCongruential {
public:
    uint32_t RandomUint32Uniform(uint32_t n);         // 0x00a68fb0
};
extern RandomLinearCongruential gMathRandom;          // 0x01601760

static const uint32_t kCategory = 0xa0325a4f;

// @ 0x01058460
void UnlockTableItems(uint32_t resourceKey)
{
    uint32_t group = 0;
    cTerrainSphere* sphere = (cTerrainSphere*)NounManager()->GetCurrentTerrainSphere();
    cCollectableItems* items = sphere ? sphere->mpItems : AutoRefCount<cCollectableItems>();
    if (!items)
        return;

    AutoRefCount<cItemDataTable> table;
    if (ResolveResource(resourceKey, (void**)&table)) {
        const int count = (int)(table->mRowsEnd - table->mRowsBegin);
        group = (uint32_t)sphere->mpItems->GetDefaultItemGroupId();

        fixed_vector_u64 reserved;
        reserved.reserve((uint32_t)count);

        for (int i = 0; i < count; ++i) {
            const cItemDataRow& row = table->mRowsBegin[i];
            const uint32_t low = row.mIdLow;
            uint32_t hi = row.mIdHigh;
            if (hi == 0xffffffff || hi == 0)
                hi = group;
            const uint64_t id = MakeItemId(hi, low);
            if (sphere->mpItems->IsKnownItem(id) && !sphere->mpItems->IsItemUnlocked(id))
                items->UnlockItem(id, 0);
        }

        sp_vector_u64 category;
        items->GetItemsInCategory(category, kCategory);
        if (category.mpBegin != category.mpEnd) {
            const uint32_t n = (uint32_t)(category.mpEnd - category.mpBegin);
            const uint64_t id = category.mpBegin[gMathRandom.RandomUint32Uniform(n)];
            items->UnlockItem(id, 0);
        }
    }
}
