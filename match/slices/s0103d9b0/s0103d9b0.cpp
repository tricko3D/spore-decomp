// Slice s0103d9b0: SP::cSPSpaceTrading::ConsiderSellingSpice
// (planet spice key / NPC-store price decision; creates the commodity and
// appends it to the trading inventory vector at +0x88).
// Flags: /O2 /MD /Gy /TP /arch:SSE
#include "types.h"
#include <new>

typedef unsigned int u32;
typedef unsigned char u8;

// ---------------------------------------------------------------------------
// Resource key (three words, passed by value; the function returns with ret 0x10
// for the planet pointer plus this key).
// ---------------------------------------------------------------------------
struct ResourceKey_s0103d9b0 {
    u32 instance;    // +0x00
    u32 type;        // +0x04
    u32 group;       // +0x08
};

// ---------------------------------------------------------------------------
// Property list (only the reference-count interface this function touches)
// ---------------------------------------------------------------------------
struct PropertyList_s0103d9b0 {
    virtual int AddRef();       // +0x00
    virtual int Release();      // +0x04
};

struct PropertyManager_s0103d9b0 {
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
                                 PropertyList_s0103d9b0** ppOut);   // +0x2c
};

PropertyManager_s0103d9b0* PropertyManager_s0103d9b0_Get();          // 0x0067de30

// 0x0040cf10 (__cdecl): read property `id` of `pList` as a float.
void GetFloatProperty_s0103d9b0(PropertyList_s0103d9b0* pList, u32 id,
                                float* pOut);

// ---------------------------------------------------------------------------
// Intrusive reference-counted pointer (AsOutParam releases the previous object
// before handing out the slot), same shape the original uses.
// ---------------------------------------------------------------------------
template <class T>
struct AutoRefCount_s0103d9b0 {
    T* mpObject;
    AutoRefCount_s0103d9b0() : mpObject(0) {}
    // The pointer is tested as the constructor argument, not re-read through
    // mpObject, so the inlined call site keeps it in one register.
    AutoRefCount_s0103d9b0(T* p) : mpObject(p) { if (p) p->AddRef(); }
    AutoRefCount_s0103d9b0(const AutoRefCount_s0103d9b0& o) : mpObject(o.mpObject)
    {
        if (mpObject) mpObject->AddRef();
    }
    ~AutoRefCount_s0103d9b0() { if (mpObject) mpObject->Release(); }
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

template <class T>
T** AsOutParam_s0103d9b0(T*& p)
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
struct Item_s0103d9b0 {
    virtual void v00();
    virtual int  AddRef();      // +0x04
    virtual int  Release();     // +0x08
};
typedef AutoRefCount_s0103d9b0<Item_s0103d9b0> ItemPtr_s0103d9b0;

// Vector of item references living at cSPSpaceTrading+0x88.
struct ItemVec_s0103d9b0 {
    ItemPtr_s0103d9b0* mpBegin;     // +0x00 (object +0x88)
    ItemPtr_s0103d9b0* mpEnd;       // +0x04 (object +0x8c)
    ItemPtr_s0103d9b0* mpCapacity;  // +0x08 (object +0x90)

    void DoInsertValue(ItemPtr_s0103d9b0* position,
                       Item_s0103d9b0* const& value);            // 0x00f14270

    void push_back(Item_s0103d9b0* const& value)
    {
        if (mpEnd < mpCapacity)
            ::new (mpEnd++) ItemPtr_s0103d9b0(value);
        else
            DoInsertValue(mpEnd, value);
    }
};

// ---------------------------------------------------------------------------
// NPC store (only the fields this function reads)
// ---------------------------------------------------------------------------
struct NPCStore_s0103d9b0 {
    u8   mPad00[0x30];
    int  mSpiceCount30;                // +0x30
    u8   mPad34[0x60 - 0x34];
    ResourceKey_s0103d9b0* mpSpiceID;  // +0x60 (vector begin)
    u8   mPad64[0x9c - 0x64];
    float* mpSpiceCost;                // +0x9c (vector begin)
};

// ---------------------------------------------------------------------------
// Space economy tuning (0x0102f810 / 0x0102fde0)
// ---------------------------------------------------------------------------
struct SpaceEconomyTuning_s0103d9b0 {
    int GetTradingMaxSpiceToBuy() const;    // 0x0102fde0
};
SpaceEconomyTuning_s0103d9b0* SpaceEconomyTuning_s0103d9b0_Get();   // 0x0102f810

// ---------------------------------------------------------------------------
// Planet reference: the planet's current spice key lives at +0x198.
// ---------------------------------------------------------------------------
struct Planet_s0103d9b0 {
    ResourceKey_s0103d9b0* GetSpiceKey();   // 0x00b8dad0
};

// ---------------------------------------------------------------------------
// Player inventory (slots +0x60 / +0x64 of its vtable)
// ---------------------------------------------------------------------------
struct InventoryEntry_s0103d9b0 {
    u8  mPad00[0x10];
    int mItemCount;                 // +0x10
};

struct Inventory_s0103d9b0 {
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
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual InventoryEntry_s0103d9b0* GetEntry(int index);         // +0x60
    virtual int FindEntry(int category, ResourceKey_s0103d9b0* pKey);  // +0x64
};

struct SpaceGame_s0103d9b0 {
    Inventory_s0103d9b0* GetPlayerInventory();   // 0x00a1ad60
};
SpaceGame_s0103d9b0* SpaceGameGet_s0103d9b0();   // 0x01002bd0

// ---------------------------------------------------------------------------
// Terrain measure chain used for the fallback spice price (0x00b3d300 /
// 0x00f67d90 / 0x00c75c30).
// ---------------------------------------------------------------------------
struct TerrainSphere_s0103d9b0 {
    // Retail multiplies this result with `fmul dword ptr` twice, so the value
    // and the 0.05f scale below are single precision.
    float Measure();        // 0x00c75c30
};
struct NounManager_s0103d9b0 {
    TerrainSphere_s0103d9b0* GetCurrentTerrainSphere();   // 0x00f67d90
};
NounManager_s0103d9b0* __stdcall NounManager_s0103d9b0_Get(u32 key);   // 0x00b3d300 (callee cleans)

// Shared price-scale constant (0x013ec478 == 0.05f), multiplied as a dword.
extern const float kSpicePriceScale_s0103d9b0;

// ---------------------------------------------------------------------------
// cSPSpaceTrading (only the layout this function needs)
// ---------------------------------------------------------------------------
struct cSPSpaceTrading_s0103d9b0 {
    u8  mPad00[0x48];
    NPCStore_s0103d9b0** mppStores;    // +0x48
    u8  mPad4c[0x88 - 0x4c];
    ItemVec_s0103d9b0 mSelling;        // +0x88
    u8  mPad94[0x9c - 0x94];
    int mStoreIndex;                   // +0x9c

    // 0x0103a480: build a commodity inventory item from a resource key.
    void CreateCommodityFromID(Item_s0103d9b0*& dst,
                               const ResourceKey_s0103d9b0& key,
                               int count, float cost);

    void ConsiderSellingSpice(Planet_s0103d9b0* pPlanet,
                              ResourceKey_s0103d9b0 key);
};

// ---------------------------------------------------------------------------
// @ 0x0103d9b0  SP::cSPSpaceTrading::ConsiderSellingSpice
// ---------------------------------------------------------------------------
void cSPSpaceTrading_s0103d9b0::ConsiderSellingSpice(
    Planet_s0103d9b0* pPlanet, ResourceKey_s0103d9b0 key)
{
    bool bOwn = false;

    // Does the planet already trade this spice, or does its store already hold
    // the maximum amount of spice it will buy?
    ResourceKey_s0103d9b0* pPlanetKey = pPlanet->GetSpiceKey();
    if (key.instance == pPlanetKey->instance &&
        key.type == pPlanetKey->type &&
        key.group == pPlanetKey->group)
        bOwn = true;
    else
    {
        // The store is picked before the tuning calls, so it survives them in
        // a callee-saved register.
        NPCStore_s0103d9b0* pStore = mppStores[mStoreIndex];
        if (pStore->mSpiceCount30 >=
            SpaceEconomyTuning_s0103d9b0_Get()->GetTradingMaxSpiceToBuy())
            bOwn = true;
    }

    int nOwned = 0;
    {
        SpaceGame_s0103d9b0* pGame = SpaceGameGet_s0103d9b0();
        Inventory_s0103d9b0* pInventory = pGame->GetPlayerInventory();
        int index = pInventory->FindEntry(4, &key);
        if (index != -1)
            nOwned = pInventory->GetEntry(index)->mItemCount;
    }

    PropertyList_s0103d9b0* pProps = 0;
    PropertyManager_s0103d9b0_Get()->GetPropertyList(
        key.instance, 0x34d97fa, AsOutParam_s0103d9b0(pProps));

    if (nOwned)
    {
        // `cost` is assigned on every path below (the bOwn path first, the
        // store branches after it), so it has no initialiser.
        float cost;
        // The item reference: the original places it in the lower of the two
        // frame slots; handing its member to CreateCommodityFromID as the
        // out-parameter resets it to null right before the call.
        ItemPtr_s0103d9b0 item;

        if (bOwn)
            cost = 0.0f;

        NPCStore_s0103d9b0* pStore = mppStores[mStoreIndex];
        ResourceKey_s0103d9b0* pIDs = pStore->mpSpiceID;

        if (key.instance == pIDs[0].instance &&
            key.type == pIDs[0].type &&
            key.group == pIDs[0].group)
            cost = pStore->mpSpiceCost[0];
        else if (key.instance == pIDs[1].instance &&
                 key.type == pIDs[1].type &&
                 key.group == pIDs[1].group)
            cost = pStore->mpSpiceCost[1];
        else if (key.instance == pIDs[2].instance &&
                 key.type == pIDs[2].type &&
                 key.group == pIDs[2].group)
            cost = pStore->mpSpiceCost[2];
        else
        {
            GetFloatProperty_s0103d9b0(pProps, 0x34f1a4a, &cost);
            cost = NounManager_s0103d9b0_Get(0x1c2e590d)
                       ->GetCurrentTerrainSphere()
                       ->Measure() * cost * kSpicePriceScale_s0103d9b0;
        }

        item.mpObject = 0;
        CreateCommodityFromID(item.mpObject, key, 0, cost);
        mSelling.push_back(item.mpObject);
    }

    if (pProps != 0)
        pProps->Release();
}
