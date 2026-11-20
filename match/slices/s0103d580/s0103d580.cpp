// Slice s0103d580: SP::cSPSpaceTrading::ConsiderBuyingRareGroup.
// Flags: /O2 /MD /Gy /TP /arch:SSE
#include "types.h"
#include <new>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

// ---------------------------------------------------------------------------
// Resource key (three words, passed by value: the function returns with ret 0xc)
// ---------------------------------------------------------------------------
struct ResourceKey_s0103d580 {
    u32 instance;    // +0x00
    u32 type;        // +0x04
    u32 group;       // +0x08
};

// ---------------------------------------------------------------------------
// Property / property list (only the members this function touches)
// ---------------------------------------------------------------------------
struct Property_s0103d580 {
    u8  mPad00[0x12];
    u16 mType;                    // +0x12
    int*   GetIntPtr();           // 0x0041e990
    float* GetFloatPtr();         // 0x0041ea70
};

struct PropertyList_s0103d580 {
    virtual int  AddRef();                                          // +0x00
    virtual int  Release();                                         // +0x04
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual bool GetProperty(u32 id, Property_s0103d580** ppOut);    // +0x24
};

struct PropertyManager_s0103d580 {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void GetPropertyList(u32 instance, u32 group,
                                 PropertyList_s0103d580** ppOut);    // +0x2c
};

PropertyManager_s0103d580* PropertyManager_Get_s0103d580();          // 0x0067de30

// Intrusive reference-counted pointer, exactly the shape the original uses
// (AsOutParam releases the previous object before handing out the slot).
template <class T>
struct AutoRefCount_s0103d580 {
    T* mpObject;
    AutoRefCount_s0103d580() : mpObject(0) {}
    AutoRefCount_s0103d580(T* p) : mpObject(p) { if (mpObject) mpObject->AddRef(); }
    AutoRefCount_s0103d580(const AutoRefCount_s0103d580& o) : mpObject(o.mpObject)
    {
        if (mpObject) mpObject->AddRef();
    }
    ~AutoRefCount_s0103d580() { if (mpObject) mpObject->Release(); }
    T** AsOutParam()
    {
        if (mpObject) {
            T* pOld = mpObject;
            mpObject = 0;
            pOld->Release();
        }
        return &mpObject;
    }
    T* get() const { return mpObject; }
    T* operator->() const { return mpObject; }
};


// Hand-rolled AsOutParam for a raw reference-counted pointer (same shape as the
// AutoRefCount member: release the old object, then hand out the slot).
template <class T>
T** AsOutParam_s0103d580(T*& p)
{
    if (p) {
        T* pOld = p;
        p = 0;
        pOld->Release();
    }
    return &p;
}

// ---------------------------------------------------------------------------
// Inventory item reference interface (AddRef at +0x04, Release at +0x08)
// ---------------------------------------------------------------------------
struct Item_s0103d580 {
    virtual void v00();
    virtual int  AddRef();        // +0x04
    virtual int  Release();       // +0x08
};
typedef AutoRefCount_s0103d580<Item_s0103d580> ItemPtr_s0103d580;

// Vector of item references living at cSPSpaceTrading+0x88.
struct ItemVec_s0103d580 {
    ItemPtr_s0103d580* mpBegin;     // +0x00 (object +0x88)
    ItemPtr_s0103d580* mpEnd;       // +0x04 (object +0x8c)
    ItemPtr_s0103d580* mpCapacity;  // +0x08 (object +0x90)

    void DoInsertValue(ItemPtr_s0103d580* position,
                       Item_s0103d580* const& value);          // 0x00f14270

    void push_back(Item_s0103d580* const& value)
    {
        if (mpEnd < mpCapacity)
            ::new (mpEnd++) ItemPtr_s0103d580(value);
        else
            DoInsertValue(mpEnd, value);
    }
};

// ---------------------------------------------------------------------------
// Rare-commodity group table: FindGroup maps a group key to its table index.
// ---------------------------------------------------------------------------
struct GroupTable_s0103d580 {
    u32 FindGroup(u32 id);         // 0x0103a760
};

void* GroupTableGlobal_s0103d580();                                // 0x00b3d3d0

// ---------------------------------------------------------------------------
// cSPSpaceTrading (only the layout this function needs)
// ---------------------------------------------------------------------------
struct cSPSpaceTrading_s0103d580 {
    u8  mPad00[0x88];
    ItemVec_s0103d580 mVec88;      // +0x88

    // 0x0103a480: build a commodity inventory item from a resource key.
    void CreateCommodityFromID(Item_s0103d580*& dst,
                               const ResourceKey_s0103d580& key,
                               int count, float cost);
    // 0x0103ac70: is every member commodity of the group in the inventory?
    bool IsRareGroupComplete(u32 group, bool needInventory);

    void ConsiderBuyingRareGroup(ResourceKey_s0103d580 key);
};

// ---------------------------------------------------------------------------
// @ 0x0103d580  SP::cSPSpaceTrading::ConsiderBuyingRareGroup
// ---------------------------------------------------------------------------
void cSPSpaceTrading_s0103d580::ConsiderBuyingRareGroup(ResourceKey_s0103d580 key)
{
    // The original allocates this slot first and only assigns it when the
    // cost property is present; there is no zero-initialisation on the
    // missing-property path.
    float cost;

    // The original stages each property result in the same stack slot that the
    // later property-list / item pointer occupies, so these two locals double as
    // the GetProperty out-parameters before their real use.
    PropertyList_s0103d580* props = 0;
    PropertyList_s0103d580* props2;   // also the group-property scratch
    Item_s0103d580* item;             // also the cost-property scratch
    PropertyManager_Get_s0103d580()->GetPropertyList(key.instance, 0x34d97fa,
                                                     AsOutParam_s0103d580(props));

    // The group key is the instance itself unless the property list names one.
    {
        u32 group;
        if (props != 0 &&
            props->GetProperty(0x3c72dc6, (Property_s0103d580**)&props2) &&
            ((Property_s0103d580*)props2)->mType == 9)
            group = (u32)((Property_s0103d580*)props2)->GetIntPtr()[0];
        else
            group = key.instance;

        u32 groupIndex =
            ((GroupTable_s0103d580*)GroupTableGlobal_s0103d580())->FindGroup(group);
        if (!IsRareGroupComplete(groupIndex, true))
            return;
    }

    props2 = 0;
    PropertyManager_Get_s0103d580()->GetPropertyList(key.instance, 0x34d97fa,
                                                     AsOutParam_s0103d580(props2));

    if (props2 != 0 &&
        props2->GetProperty(0x34f1a4a, (Property_s0103d580**)&item) &&
        ((Property_s0103d580*)item)->mType == 0xd)
        cost = ((Property_s0103d580*)item)->GetFloatPtr()[0];

    item = 0;
    CreateCommodityFromID(item, key, 0, cost);
    mVec88.push_back(item);
    if (item != 0)
        item->Release();
    if (props2 != 0)
        props2->Release();
    if (props != 0)
        props->Release();
}
