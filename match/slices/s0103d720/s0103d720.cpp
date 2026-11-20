// SP::cSPSpaceTrading::CreateCommodityEntry @ 0x0103d720 (644 bytes).
// Flags: /O2 /MD /Gy /TP /arch:SSE (no /EHsc); same module as slice s0103d380.
//
// Recovered from the retail instruction stream:
//   1. PropertyManager (0x67de30) vtbl+0x2c(key.instance, 0x34d97fa, &list) stores
//      the commodity's property list into a smart pointer that is released on the
//      function's single epilogue (0x103d941); the explicit null reset right after
//      the manager lookup is the store-0/Release block at 0x103d734,
//   2. SpaceGameGet (0x1002bd0) -> GetPlayerInventory (0xa1ad60) -> IndexOf(4, key);
//      -1 skips the body, otherwise GetItemEntry(index) (vtbl+0x60) and a float
//      compare of the unit price against 0x1485378 (0.0f),
//   3. the price multiplier comes from property 0x34f1a4a when its tag (+0x12) is 13
//      (float), read through Property::GetFloat (0x41ea70),
//   4. a 19-entry switch (jump table 0x103d958) on the commodity type at +0x58 picks
//      the price property id (0x5b80b32..0x5b80b3d) passed to GetFloatProperty
//      (0x40cf10); the scale starts at 1.0f (0x1485720),
//   5. CreateCommodityFromID(item, key, 0, scale * multiplier) (0x103a480), then
//      the item is appended to mNPCItems (+0x88) as a refcounted element.
//
// Scope layout matters here: the property-fetch block closes before the scale is
// declared, so its slot is reused by the float and the whole frame stays 0xc with
// the temporary item in the dead first-argument slot, exactly as in retail.
#include "game/types.h"
#include <stddef.h>

inline void* operator new(size_t, void* p) { return p; }
inline void operator delete(void*, void*) {}

namespace sp0103d720 {

struct ResourceKey {
    uint32_t instance, type, group;                                       // +0x00
};

struct IRefCounted {
    virtual void _v0();                                                   // +0x00
    virtual int Release();                                                // +0x04
    virtual int AddRef();                                                 // +0x08
};

struct Property {
    void* mpData;                                                         // +0x00
    uint32_t pad04;
    int mnItemCount;                                                      // +0x08
    uint32_t pad0c;
    uint16_t mnFlags;                                                     // +0x10
    uint16_t mnType;                                                      // +0x12
    float* GetFloat();                                                    // 0x0041ea70
};

struct PropertyList : IRefCounted {
    virtual void _v3(); virtual void _v4();
    virtual void _v5(); virtual void _v6();
    virtual bool HasProperty(uint32_t id);                                // +0x1c
    virtual void _v8();
    virtual bool GetProperty(uint32_t id, Property*& pOut);               // +0x24
    virtual Property* GetPropertyObject(uint32_t id);                     // +0x28
};

struct IPropertyManager {
    virtual void _v0();  virtual void _v1();  virtual void _v2();         // +0x00..
    virtual void _v3();  virtual void _v4();  virtual void _v5();
    virtual void _v6();  virtual void _v7();  virtual void _v8();
    virtual void _v9();  virtual void _v10();
    virtual void GetListFor(uint32_t instance, uint32_t id, PropertyList** ppOut);  // +0x2c
};

struct cSpaceInventoryItem {
    virtual void _v0();                                                   // +0x00
    virtual int AddRef();                                                 // +0x04
    virtual int Release();                                                // +0x08
    virtual void _v3();                                                   // +0x0c
    virtual uint32_t GetType();                                           // +0x10
    uint32_t mnRefCount;                                                  // +0x04
    uint32_t pad08[2];
    int mItemCount;                                                       // +0x10
};

struct cSpaceInventory {
    virtual void _v0();  virtual void _v1();  virtual void _v2();         // +0x00..
    virtual void _v3();  virtual void _v4();  virtual void _v5();
    virtual void _v6();  virtual void _v7();  virtual void _v8();
    virtual void _v9();  virtual void _v10(); virtual void _v11();
    virtual void _v12(); virtual void _v13(); virtual void _v14();
    virtual void _v15(); virtual void _v16(); virtual void _v17();
    virtual void _v18(); virtual void _v19(); virtual void _v20();
    virtual void _v21(); virtual void _v22(); virtual void _v23();
    virtual cSpaceInventoryItem* GetItemEntry(int index);                 // +0x60
    virtual int IndexOf(uint32_t type, const ResourceKey& id);            // +0x64
};

struct cSpaceGame {
    cSpaceInventory* GetPlayerInventory();                                // 0x00a1ad60
};

struct cEmpire {
    uint32_t pad00[0x58 / 4];
    int mnCommodityType;                                                  // +0x58
};

template <class T> struct intrusive_ptr {
    T* mpObject;
    intrusive_ptr() : mpObject(0) {}
    intrusive_ptr(const intrusive_ptr& o) : mpObject(o.mpObject) { if (mpObject) mpObject->AddRef(); }
    ~intrusive_ptr() { if (mpObject) mpObject->Release(); }
    intrusive_ptr& operator=(T* p)
    {
        if (mpObject != p) {
            T* pOld = mpObject;
            mpObject = 0;
            if (pOld) pOld->Release();
            mpObject = p;
            if (mpObject) mpObject->AddRef();
        }
        return *this;
    }
    T* operator->() const { return mpObject; }
};
typedef intrusive_ptr<cSpaceInventoryItem> cSpaceInventoryItemPtr;

template <class T> struct vector {
    T* mpBegin;
    T* mpEnd;
    T* mpCapacity;
    uint32_t mAllocator;
    void push_back(const T& value)
    {
        if (mpEnd < mpCapacity)
            ::new (mpEnd++) T(value);
        else
            DoInsertValue(mpEnd, value);
    }
    void DoInsertValue(T* position, const T& value);                      // 0x00f14270
};

class cSPSpaceTradingX {
public:
    uint32_t pad00[0x88 / 4];                                             // +0x00
    vector<cSpaceInventoryItemPtr> mNPCItems;                             // +0x88
    uint32_t pad98;
    int mLastNPCStoreIndex;                                               // +0x9c

    void CreateCommodityEntry_0103d720(cEmpire* empire, ResourceKey key);
    void CreateCommodityFromID(cSpaceInventoryItemPtr& dst, const ResourceKey& id,
                              int count, float cost);                    // 0x0103a480
};

IPropertyManager* __cdecl PropertyManager();                              // 0x0067de30
cSpaceGame* __cdecl SpaceGameGet();                                       // 0x01002bd0
void __cdecl GetFloatProperty(PropertyList* pList, uint32_t id, float* pOut);  // 0x0040cf10

// @ 0x0103d720
void cSPSpaceTradingX::CreateCommodityEntry_0103d720(cEmpire* empire, ResourceKey key)
{
    PropertyList* pList = 0;
    float priceMultiplier;

    // The manager lookup is preceded by an explicit null reset of the property
    // list: retail stores 0 again and releases the old pointer (0x103d734).
    IPropertyManager* pm = PropertyManager();
    if (pList != 0) {
        PropertyList* pOld = pList;
        pList = 0;
        pOld->Release();
    }
    pm->GetListFor(key.instance, 0x34d97fa, &pList);

    cSpaceInventory* pInventory = SpaceGameGet()->GetPlayerInventory();
    int index = pInventory->IndexOf(4, key);
    if (index != -1) {
        cSpaceInventoryItem* pEntry = pInventory->GetItemEntry(index);
        float unitValue = (float)pEntry->mItemCount;
        if (unitValue != 0.0f) {
            if (pList != 0) {
                Property* pProp;
                if (pList->GetProperty(0x34f1a4a, pProp) && pProp->mnType == 13)
                    priceMultiplier = *pProp->GetFloat();
            }

            // Retail leaves the multiplier uninitialised when the property is
            // absent and multiplies by it regardless; kept as written there.
            // A block of its own: retail reuses the property pointer's slot for
            // the scale once the fetch scope has closed.
            {
                float scale = 1.0f;
                // The labels must be written in strict numeric order: the case bodies are
                // emitted in source order, and cl's stack packer alternates the scratch
                // register (edx/eax) it uses for `lea reg,[esp+8]` per emitted block.
                // With 9,11,10 the two property-0x3b blocks get the opposite registers
                // from retail (edx/eax swapped); 0..9,10,11,... reproduces retail exactly
                // (0x5b80b3c -> eax, 0x3b -> edx, 0x3d -> edx, 0x3b -> eax, 0x3c -> edx).
                switch (empire->mnCommodityType) {
                    case 0:  GetFloatProperty(pList, 0x5b80b32, &scale); break;
                    case 1:  GetFloatProperty(pList, 0x5b80b33, &scale); break;
                    case 2:  GetFloatProperty(pList, 0x5b80b34, &scale); break;
                    case 3:  GetFloatProperty(pList, 0x5b80b35, &scale); break;
                    case 4:  GetFloatProperty(pList, 0x5b80b36, &scale); break;
                    case 5:  GetFloatProperty(pList, 0x5b80b37, &scale); break;
                    case 6:  GetFloatProperty(pList, 0x5b80b38, &scale); break;
                    case 7:  GetFloatProperty(pList, 0x5b80b39, &scale); break;
                    case 8:  GetFloatProperty(pList, 0x5b80b3a, &scale); break;
                    case 9:  GetFloatProperty(pList, 0x5b80b3c, &scale); break;
                    case 10: GetFloatProperty(pList, 0x5b80b3b, &scale); break;
                    case 11: GetFloatProperty(pList, 0x5b80b3c, &scale); break;
                    case 12:
                    case 16: GetFloatProperty(pList, 0x5b80b3d, &scale); break;
                    case 13: GetFloatProperty(pList, 0x5b80b3d, &scale); break;
                    case 15: GetFloatProperty(pList, 0x5b80b3b, &scale); break;
                    case 17: GetFloatProperty(pList, 0x5b80b3b, &scale); break;
                    case 14:
                    case 18: GetFloatProperty(pList, 0x5b80b3c, &scale); break;
                    default: break;
                }

                cSpaceInventoryItemPtr item;
                CreateCommodityFromID(item, key, 0, scale * priceMultiplier);
                mNPCItems.push_back(item);
            }
        }
    }
    if (pList != 0)
        pList->Release();
}

}  // namespace sp0103d720
