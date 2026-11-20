// Slice s00b215a0: cGameNounManager::BuildClassMap (0x00b215a0).
//
// Rebuilds the noun class map (an EASTL hash map at this+0x20) from the property
// system's "noun" object list, then from the world's intrusive noun hash, where
// every hashed noun becomes a property object carrying class/owner/world/name.
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE.
#include "types.h"

typedef unsigned int uint;
typedef unsigned __int64 uint64;
typedef unsigned short uint16;
typedef unsigned char uint8;

struct NameVector;
struct PropWorld;
struct World;

// --------------------------------------------------------------------------
// helper functions of the retail binary
extern "C" PropWorld* __cdecl PropertyManager();                    // 0x67de30
extern "C" World*     __cdecl GetWorld();                           // 0x401090
extern "C" void*      __cdecl SporeNew(uint size, const char* name, int a, int b,
                                       const char* file, int line);  // 0xf473a0
extern "C" void       __cdecl EA_Deallocate(void* p);               // 0xf47380
extern "C" void       __cdecl AssignNameVector(NameVector* dst, const NameVector* src);  // 0x93c570

extern uint gDefaultUInt;    // 0x015d1164, the default when a value is not an integer
extern uint gSomeDefault;    // 0x0164dc58

// --------------------------------------------------------------------------
// property-system interfaces
struct IRefObj {
    virtual void AddRef();                                          // +0x00
    virtual void Release();                                         // +0x04
};

// 0x14-byte property value: a 16-byte payload, the value flags and the value type.
struct PropValue {
    union {
        void*  mpData;
        uint   muValue;
        uint8  mBytes[0x10];
    };                                                              // +0x00
    uint16 mFlags;                                                  // +0x10
    uint16 mType;                                                   // +0x12

    // Payload pointer of an integer-typed value; the installed default otherwise.
    uint* PayloadPtr() {
        if (mType == 0xa || mType == 0x10) {
            if (mFlags & 0x30)
                return *(uint**)this;
            return mType ? (uint*)this : 0;
        }
        return &gSomeDefault;
    }
    uint  GetUInt() { return *PayloadPtr(); }                        // inlined at both uses
    uint* GetUIntPtr();                                             // 0x41ea00
    void  FreeString(int flag);                                     // 0x93db80 (ret 4)
    void  SetString(uint a, int n, NameVector* s, uint b, int c);    // 0x93dd80 (ret 0x14)
};

struct PropObject : IRefObj {
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void SetValue(uint name, const PropValue* value);        // +0x14
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual bool GetValue(uint name, PropValue** out);              // +0x24
};

// "noun" object list returned by the property world.
struct IdVector {
    uint* mpBegin;                                                  // +0x00
    uint* mpEnd;                                                    // +0x04
    uint* mpCap;                                                    // +0x08
};

struct PropWorld {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28();
    virtual bool Lookup(uint id, uint name, PropObject** out);      // +0x2c
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44();
    virtual void GetObjects(uint name, IdVector* out);              // +0x48
};

// eastl::vector<uint16> used for a hashed noun's name.
struct NameVector {
    uint16* mpBegin;                                                // +0x00
    uint16* mpEnd;                                                  // +0x04
    uint16* mpCap;                                                  // +0x08
};

// --------------------------------------------------------------------------
// verbatim 16-byte hash-map value: the owning reference plus two ids and the
// world stamp (0xb18490 fills it, 0xa21bf0 releases it).
struct ClassKey {
    uint     mClassId;                                              // +0x00
    uint     mObjectId;                                             // +0x04
    uint     mWorldId;                                              // +0x08
    IRefObj* mpOwner;                                               // +0x0c

    void CopyFrom(const ClassKey& o) {                               // inlined copy ctor
        mClassId = o.mClassId;
        mObjectId = o.mObjectId;
        mWorldId = o.mWorldId;
        mpOwner = o.mpOwner;
        if (mpOwner)
            mpOwner->AddRef();
    }
    void Destroy();                                                 // 0xa21bf0
    ClassKey* Ctor(uint classId, uint objectId, IRefObj* owner);    // 0xb18490
};

// eastl hash-map value_type: a 64-bit key plus the 16-byte ClassKey.
struct ClassEntry {
    uint64   mKey;                                                  // +0x00
    ClassKey mValue;                                                // +0x08
};

struct HashNode;

// pair<iterator, bool> returned by the map's insert.
struct InsertResult {
    void* mpNode;                                                   // +0x00
    void* mpBucket;                                                 // +0x04
    bool  mbInserted;                                               // +0x08
};

struct TrueType {};

struct ClassMap {
    uint       mFunctors;                                           // +0x00
    HashNode** mpBuckets;                                           // +0x04
    uint       mBucketCount;                                        // +0x08
    uint       mElementCount;                                       // +0x0c

    void FreeNodes(HashNode** buckets, uint count);                 // 0xb20a50 (ret 8)
    void Insert(InsertResult* out, const ClassEntry& value, TrueType t);  // 0xb20860 (ret 0xc)
};

// --------------------------------------------------------------------------
// world noun hash (intrusive; the node's chain link sits at +0x444).
struct NounNode {
    uint       mId;                                                 // +0x00
    NameVector mName;                                               // +0x04
    char       mPad[0x444 - 0x10];                                  // +0x10
    NounNode*  mpNext;                                              // +0x444
};

struct NounHash {
    NounNode*  mpFirst;                                             // +0x00
    NounNode** mpBuckets;                                           // +0x04
    uint       mBucketCount;                                        // +0x08
};

struct World {
    char     mPad[0xb0];                                            // +0x00
    NounHash mNounHash;                                             // +0xb0
};

// The 0x38-byte property object built for every hashed noun.
struct NounEntry {
    char mBytes[0x38];
    NounEntry* Init();                                              // 0x6a1c40
};

// --------------------------------------------------------------------------
// reference-counted pointer (assignment adds the new reference, then drops the
// old one; 0 == null is handled first so a stale slot is cleared before use).
template <class T>
struct RefPtr {
    T* mp;
    RefPtr() : mp(0) {}
    void Reset() { if (mp) mp->Release(); }
    RefPtr& operator=(T* p) {
        if (p != mp) {
            if (p)
                p->AddRef();
            T* old = mp;
            mp = p;
            if (old)
                old->Release();
        }
        return *this;
    }
    T* operator->() const { return mp; }
};

// --------------------------------------------------------------------------
// Property name hashes the original passes to the property system.
enum {
    kNounName  = 0x2e9c426,
    kClassName = 0x2e9c0cb,
    kOwnerName = 0x2e9eda0,
    kWorldName = 0x2e9c0d2,
    kEntryName = 0x2e9c159
};

// Default class id given to every noun rebuilt from the world hash.
enum { kDefaultClassId = 0x1be418e };

struct cGameNounManager_s00b215a0 {
    char     mPad00[0x20];                                          // +0x00
    ClassMap mClassMap;                                             // +0x20
    void __thiscall BuildClassMap();
};

// @ 0x00b215a0
void __thiscall cGameNounManager_s00b215a0::BuildClassMap()
{
    PropWorld* pm = PropertyManager();
    ClassMap* map = &mClassMap;

    map->FreeNodes(map->mpBuckets, map->mBucketCount);
    map->mElementCount = 0;

    RefPtr<PropObject> cur;

    {
        IdVector ids;
        ids.mpBegin = 0;
        ids.mpEnd = 0;
        ids.mpCap = 0;
        PropertyManager()->GetObjects(kNounName, &ids);

        for (uint* it = ids.mpBegin; it != ids.mpEnd; ++it) {
            cur = 0;
            uint id = *it;
            if (!pm->Lookup(id, kNounName, &cur.mp))
                continue;

            uint classId = 0;
            PropValue* pvClass;
            if (cur.mp && cur->GetValue(kClassName, &pvClass))
                classId = pvClass->GetUInt();

            uint objectId = id;
            PropValue* pvOwner;
            if (cur.mp && cur->GetValue(kOwnerName, &pvOwner)) {
                uint v = pvOwner->GetUInt();
                if (v)
                    objectId = v;
            }
            if (!classId || !objectId)
                continue;

            ClassKey key;
            key.Ctor(classId, objectId, cur.mp);

            PropValue* pvWorld;
            if (cur.mp && cur->GetValue(kWorldName, &pvWorld) && pvWorld->mType == 0xa)
                key.mWorldId = *pvWorld->GetUIntPtr();

            ClassEntry entry;
            entry.mKey = ((uint64)(int)classId << 32) | (uint64)objectId;
            entry.mValue.CopyFrom(key);
            InsertResult res;
            map->Insert(&res, entry, TrueType());
            entry.mValue.Destroy();
            key.Destroy();
        }

        if (ids.mpBegin && ((int*)ids.mpBegin)[-1])
            EA_Deallocate(ids.mpBegin);
    }

    World* world = GetWorld();
    NounHash* hash = &world->mNounHash;
    NounNode** buckets = hash->mpBuckets;
    NounNode* node = *buckets;
    NounNode** bucket = buckets;
    if (!node) {
        ++bucket;
        while (!*bucket)
            ++bucket;
        node = *bucket;
    }
    NounNode* end = buckets[hash->mBucketCount];

    while (node != end) {
        uint id = node->mId;
        NameVector name;
        AssignNameVector(&name, &node->mName);

        NounEntry* ne = (NounEntry*)SporeNew(0x38, "Simulator/NounEntry", 0, 0, 0, 0);
        if (ne)
            ne = ne->Init();
        PropObject* noun = (PropObject*)ne;
        cur = noun;

        PropValue v;
        v.muValue = id;
        v.mFlags = 0;
        v.mType = 0xa;
        noun->SetValue(kOwnerName, &v);
        if (v.mFlags & 4)
            v.FreeString(0);

        v.muValue = kDefaultClassId;
        v.mFlags = 0;
        v.mType = 0xa;
        noun->SetValue(kClassName, &v);
        if (v.mFlags & 4)
            v.FreeString(0);

        v.muValue = id;
        v.mFlags = 0;
        v.mType = 0xa;
        noun->SetValue(kWorldName, &v);
        if (v.mFlags & 4)
            v.FreeString(0);

        v.mFlags = 0;
        v.mType = 0;
        v.SetString(0x12, 9, &name, 0x10, 1);
        noun->SetValue(kEntryName, &v);
        if (v.mFlags & 4)
            v.FreeString(0);

        ClassKey key;
        key.Ctor(kDefaultClassId, id, noun);
        key.mWorldId = id;

        ClassEntry entry;
        entry.mKey = ((uint64)(int)kDefaultClassId << 32) | (uint64)id;
        entry.mValue.CopyFrom(key);
        InsertResult res;
        map->Insert(&res, entry, TrueType());
        entry.mValue.Destroy();
        key.Destroy();

        if ((char*)name.mpEnd - (char*)name.mpBegin > 1 && name.mpBegin)
            EA_Deallocate(name.mpBegin);

        node = node->mpNext;
        while (!node) {
            ++bucket;
            node = *bucket;
        }
    }

    cur.Reset();
}
