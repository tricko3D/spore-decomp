#include "types.h"

// ---------------------------------------------------------------------------
// Shared views over the retail SP::cPlayer object and the containers it owns.
// Every field used below is named; no raw-offset accesses.
// ---------------------------------------------------------------------------

// Element of the eastl::map<unsigned, unsigned> flag map at +0x11d8.
// The lower-bound helper returns a pointer straight at this pair.
struct cFlagPair {
    uint32_t mKey;         // +0x00
    uint8_t  mValue;       // +0x04
};

// Node of an eastl::map<unsigned, X> tree (left/right/parent then the pair).
struct cMapNode {
    cMapNode*  mLeft;      // +0x00
    cMapNode*  mRight;     // +0x04
    cMapNode*  mParent;    // +0x08
    uint8_t    mPad0c[0x04]; // +0x0c
    uint32_t   mKey;       // +0x10
    uint32_t   mValue;     // +0x14
};

// eastl::rbtree header as consumed by this slice: end node at +4, root at +0x0c.
struct cTree {
    uint8_t    mPad00[0x04]; // +0x00
    cMapNode*  mEnd;       // +0x04
    uint8_t    mPad08[0x04]; // +0x08
    cMapNode*  mRoot;      // +0x0c

    // 0x00c7a8f0
    uint32_t* MapEntry(const uint32_t& key);
    // 0x00abff10  the insert-or-assign helper is a __thiscall method of the tree itself
    // (`ret 0x10`: it pops its four arguments), so it is a member and not a free function.
    uint32_t* InsertOrAssign(uint32_t* hintOut, cMapNode* hint, uint32_t* query, uint32_t flag);
    // 0x00e5c780
    cMapNode** Find(cMapNode** out, const uint32_t* key);
};

struct cTimerElem {                    // eastl::pair<unsigned, SP::cGonzagoTimer>, stride 0x28
    uint32_t mKey;                     // +0x00
    uint32_t mPad04;                   // +0x04
    uint32_t mTimerVtable;             // +0x08
    uint32_t mRefCount;                // +0x0c
    uint32_t mElapsedLo;               // +0x10
    uint32_t mElapsedHi;               // +0x14
    uint32_t mBaseLo;                  // +0x18
    uint32_t mBaseHi;                  // +0x1c
    uint8_t  mbRunning;                // +0x20
    uint8_t  mPad21[0x03];             // +0x21
    uint32_t mFn;                      // +0x24

    // cGonzagoTimer copy constructor (constructs this from src); __thiscall.
    void Construct(const cTimerElem* src);                              // 0x00c76f10
};

struct cNatElem {                      // 0x14 bytes
    uint32_t mId;                      // +0x00
    uint8_t  mPad04[0x10];             // +0x04
};

// ---------------------------------------------------------------------------
// External helpers (retail addresses in comments; not defined here).
// ---------------------------------------------------------------------------
uint8_t* lowerBound(cFlagPair* begin, cFlagPair* end, const uint32_t* key, uint8_t flag); // 0x00d01260
uint8_t* eastlCopyImpl(void* dst, void* src, void* last);                                 // 0x00c771e0
struct cGonzagoTimer { void Dtor(); };                                                     // 0x00b638b0
// Both helpers are __thiscall on the caller's second argument (`arg`): retail sets `ecx` to it
// before the call (0x00db5b30 reads `[esp+8]` as the source and `esi`/`ecx` as the target; 0x00b48e40
// reads `edi`/`ecx` as the target and both stack slots as the range), so model them as members.
struct cNatTarget {
    void AssignElement(void* elem);        // 0x00db5b30
    void ErasePair(void* begin, void* end); // 0x00b48e40
};
struct cStarRecordRefView {                                                              // SP::cStarRecordRef
    int32_t  GetCommContext();                                                             // 0x00ce6950
    void*    SortBanners();                                                                // 0x00b8de30
    uint8_t  mPad08[0x68];                                                                 // +0x08
    uint32_t mId;                                                                          // +0x70
};
struct cAvatarHost { void* GetAvatar(); };                                                 // 0x00b1fdb0

struct cSPTimerView { uint64_t GetElapsedTime(); };                                       // 0x00bc3190
void*    GetSpaceCombatTuning();                                                          // 0x01029940
struct cTuningView { int32_t GetCaptureDecayTimeS(); };                                   // 0x01029c80
void*    StarManager(uint32_t handle);                                                    // 0x00b3d2a0
int32_t  BannerListFun(void* bannerList, uint32_t handle);                                // 0x00ba6d80
void     FUN_00c8c9d0(int32_t value);                                                     // 0x00c8c9d0

// ---------------------------------------------------------------------------
// SP::cPlayer
// ---------------------------------------------------------------------------
namespace SP {

struct cPlayer {
    uint8_t    mPad0000[0x113c];       // +0x0000
    cNatElem*  mNatTable;              // +0x113c  (0x14-stride element table)

    uint8_t    mPad1140[0x11d8 - 0x1140]; // +0x1140
    cFlagPair* mFlagMapRoot;           // +0x11d8
    cFlagPair* mFlagMapEnd;            // +0x11dc
    uint8_t    mPad11e0[0x11ec - 0x11e0]; // +0x11e0
    uint8_t    mFlagCompareFlag;       // +0x11ec

    uint8_t    mPad11ed[0x1214 - 0x11ed]; // +0x11ed
    cTimerElem* mCaptureBegin;         // +0x1214
    cTimerElem* mCaptureEnd;           // +0x1218

    uint8_t    mPad121c[0x1228 - 0x121c]; // +0x121c
    cTree      mCMap;                  // +0x1228

    // 0x00c7a6c0  map flag bit 0
    bool GetFlag0(uint32_t key);
    // 0x00c7a710  map flag bit 1
    bool GetFlag1(uint32_t key);
    // 0x00c7a770  map flag bit 2
    bool GetFlag2(uint32_t key);
    // 0x00c7a7d0  map flag bit 3
    bool GetFlag3(uint32_t key);
    // 0x00c7a830  map flag bit 4
    bool GetFlag4(uint32_t key);
    // 0x00c7a890  map flag bit 5
    bool GetFlag5(uint32_t key);
    // 0x00c7ad00  nat-table dispatch
    void NatOp(uint32_t index, void* arg);
    // 0x00c7ad40  remove a star from the capture progress list
    void RemoveStarFromPlayerCaptureProgressList(cStarRecordRefView* star);
    // 0x00c7ae80  ensure the embassy entry for a star
    void FUN_00c7ae80(cStarRecordRefView* star);
    // 0x00c7ada0  decay the capture progress list
    void UpdatePlayerCaptureProgressDecay();
};

// 0x00c7a6c0
bool cPlayer::GetFlag0(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return ((cFlagPair*)it)->mValue & 1;
}

// 0x00c7a710
bool cPlayer::GetFlag1(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return (((cFlagPair*)it)->mValue >> 1) & 1;
}

// 0x00c7a770
bool cPlayer::GetFlag2(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return (((cFlagPair*)it)->mValue >> 2) & 1;
}

// 0x00c7a7d0
bool cPlayer::GetFlag3(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return (((cFlagPair*)it)->mValue >> 3) & 1;
}

// 0x00c7a830
bool cPlayer::GetFlag4(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return (((cFlagPair*)it)->mValue >> 4) & 1;
}

// 0x00c7a890
bool cPlayer::GetFlag5(uint32_t key)
{
    cFlagPair* end = mFlagMapEnd;
    uint8_t* it = lowerBound(mFlagMapRoot, end, &key, mFlagCompareFlag);
    if (it == (uint8_t*)end || key < ((cFlagPair*)it)->mKey || it + 8 == it) {
        it = (uint8_t*)end;
    }
    if (it == (uint8_t*)end) {
        return 0;
    }
    return (((cFlagPair*)it)->mValue >> 5) & 1;
}

// @ 0x00c7ad00  nat-table element dispatch
void cPlayer::NatOp(uint32_t index, void* arg)
{
    cNatTarget* target = (cNatTarget*)arg;
    if (index <= 9) {
        target->AssignElement(&mNatTable[index]);
        return;
    }
    target->ErasePair(*(void**)arg, *(void**)((char*)arg + 4));
}

// 0x00c7ad40  remove a star from the capture progress list
void cPlayer::RemoveStarFromPlayerCaptureProgressList(cStarRecordRefView* star)
{
    cTimerElem* begin = mCaptureBegin;
    cTimerElem* end   = mCaptureEnd;
    if (begin == end) {
        return;
    }
    uint32_t id = *(uint32_t*)((char*)star + 0x70);
    do {
        if (begin->mKey == id) {
            goto found;
        }
        begin = (cTimerElem*)((char*)begin + 0x28);
    } while (begin != end);
    return;
found:
    if ((cTimerElem*)((char*)begin + 0x28) < mCaptureEnd) {
        eastlCopyImpl((char*)begin + 0x28, mCaptureEnd, begin);
    }
    mCaptureEnd = (cTimerElem*)((char*)mCaptureEnd - 0x28);
    ((cGonzagoTimer*)((char*)mCaptureEnd + 8))->Dtor();
}

// @ 0x00c7ae80  ensure the embassy entry for a star
void cPlayer::FUN_00c7ae80(cStarRecordRefView* star)
{
    uint32_t ctx = (uint32_t)star->GetCommContext();
    cMapNode* probe;
    cMapNode** iter = mCMap.Find(&probe, &ctx);
    if ((char*)*iter == (char*)&mCMap.mEnd) {
        uint32_t key2 = (uint32_t)star->GetCommContext();
        uint32_t* entry = mCMap.MapEntry(key2);
        entry[1] = 0;
        *entry = (uint32_t)((cAvatarHost*)star->SortBanners())->GetAvatar();
    }
}

} // namespace SP

// 0x00c7ada0  decay the capture progress list
void SP::cPlayer::UpdatePlayerCaptureProgressDecay()
{
    cTimerElem* begin = mCaptureBegin;
    if (begin == mCaptureEnd) {
        return;
    }
    do {
        cSPTimerView* timer = (cSPTimerView*)((char*)begin + 8);
        uint64_t elapsed = timer->GetElapsedTime();
        cTuningView* tuning = (cTuningView*)GetSpaceCombatTuning();
        int32_t  decayTime = tuning->GetCaptureDecayTimeS();
        float    seconds = (float)(int64_t)elapsed * 0.001f;
        if (seconds <= (float)decayTime) {
            return;
        }
        uint32_t handle = begin->mKey;
        int32_t  value  = BannerListFun(StarManager(handle), handle);
        FUN_00c8c9d0(value);
        if ((cTimerElem*)((char*)begin + 0x28) < mCaptureEnd) {
            eastlCopyImpl((char*)begin + 0x28, mCaptureEnd, begin);
        }
        mCaptureEnd = (cTimerElem*)((char*)mCaptureEnd - 0x28);
        ((cGonzagoTimer*)((char*)mCaptureEnd + 8))->Dtor();
    } while (begin != mCaptureEnd);
}

// @ 0x00c7a8f0  eastl::map::operator[]-style find-or-insert (value at node+0x14)
uint32_t* cTree::MapEntry(const uint32_t& key)
{
    cMapNode* node  = mRoot;
    cMapNode* end   = (cMapNode*)&mEnd;   // the end iterator is the anchor node's address
    cMapNode* found = end;
    while (node != 0) {
        if (node->mKey < key) {
            node = node->mLeft;
        } else {
            found = node;
            node = node->mRight;
        }
    }
    if (found != end) {
        if (found->mKey == key)
            return &found->mValue;
    }
    uint32_t query[3];
    query[0] = key;
    query[1] = 0;
    query[2] = 0;
    uint32_t flag = 0;
    InsertOrAssign((uint32_t*)&found, found, query, flag);
    return (uint32_t*)((char*)found + 0x14);
}

// ---------------------------------------------------------------------------
// Stream-backed vector readers (EASTL vector instantiations)
// ---------------------------------------------------------------------------

// Serialiser read primitive (also declared below for the quad-vector reader).
void ReadInt32(void* reader, int* out, int count, int flags);          // 0x0093a780

// Deserialiser node interface (vtable +0x18 reader, +0x1c end-entry, +0x20 sub-node).
class IStreamNode {
public:
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5();
    virtual void* GetReader();           // +0x18
    virtual void EndChunk();             // +0x1c
    virtual IStreamNode* GetSubNode();   // +0x20
};

// eastl::vector<unsigned int> (4-byte elements).
struct U32Vector {
    uint32_t* mpBegin;                   // +0x0
    uint32_t* mpEnd;                     // +0x4
    uint32_t* mpCapacity;                // +0x8

    void Clear();                        // inlined clear()
    void Reserve(uint32_t n);            // 0x00c77b60
    void PushBack(const uint32_t& value); // fast path + slow DoInsertValue
    void DoInsertValue(uint32_t* position, const uint32_t& value);  // 0x00cd0550
};

// eastl's memmove wrapper used by the range algorithms.
void EastlMove(void* dst, const void* src, unsigned bytes);            // 0x011e0744

void U32Vector::Clear()
{
    EastlMove(mpBegin, mpEnd, 0);
    mpEnd -= (mpEnd - mpBegin);
}

void U32Vector::PushBack(const uint32_t& value)
{
    if (mpEnd < mpCapacity) {
        uint32_t* p = mpEnd;
        mpEnd = p + 1;
        if (p)
            *p = value;
    } else {
        DoInsertValue(mpEnd, value);
    }
}

// @ 0x00c7a970
void FUN_00c7a970(IStreamNode* stream, U32Vector* vec)
{
    vec->Clear();

    uint32_t count = 0;
    IStreamNode* node = stream->GetSubNode();
    void* reader = node->GetReader();
    ReadInt32(reader, (int*)&count, 1, 0);
    vec->Reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        IStreamNode* itemNode = stream->GetSubNode();
        void* itemReader = itemNode->GetReader();
        uint32_t value;
        ReadInt32(itemReader, (int*)&value, 1, 0);
        vec->PushBack(value);
    }
    stream->EndChunk();
}

// eastl::vector<eastl::pair<unsigned int, SP::cGonzagoTimer>> (0x28-byte elements).
// The pair's copy assignment only touches the timer's state fields, not its
// vtable/refcount words, so the copy helpers below do the same.

// Range copy-construct [first,last) into dst; returns the new end.
cTimerElem* TimerConstructRange(cTimerElem* first, cTimerElem* last, cTimerElem* dst); // 0x00c77160
// Range copy-assign [first,last) backwards ending at dst; returns new begin.
cTimerElem* TimerCopyBackward(cTimerElem* first, cTimerElem* last, cTimerElem* dst);  // 0x00c77250
// Range destroy [first,last) (dst only carries the return value).
void TimerDestroyRange(cTimerElem* first, cTimerElem* last, cTimerElem* dst);         // 0x0100daa0
void* EastlAllocate(unsigned size, const char* name, int flags, unsigned debugFlags,
                    const char* file, int line);                        // 0x00f473a0
void  EastlFree(void* p);                                               // 0x00f47380

struct cTimerVector {
    cTimerElem* mpBegin;                 // +0x0
    cTimerElem* mpEnd;                   // +0x4
    cTimerElem* mpCapacity;              // +0x8

    void Insert(cTimerElem* position, const cTimerElem& value);          // 0x00c7aa40
};

// @ 0x00c7aa40
void cTimerVector::Insert(cTimerElem* position, const cTimerElem& value)
{
    if (mpEnd != mpCapacity) {
        const cTimerElem* source = &value;
        // Self-insertion fixup (EASTL adjusts a value that lives in the vector).
        if ((const char*)source >= (const char*)position && source < mpEnd)
            source = (const cTimerElem*)((const char*)source + 0x28);

        if (mpEnd != 0)
            mpEnd->Construct(mpEnd - 1);
        TimerCopyBackward(position, mpEnd - 1, mpEnd);

        position->mKey       = source->mKey;
        position->mElapsedLo = source->mElapsedLo;
        position->mElapsedHi = source->mElapsedHi;
        position->mBaseLo    = source->mBaseLo;
        position->mBaseHi    = source->mBaseHi;
        position->mbRunning  = source->mbRunning;
        position->mFn        = source->mFn;
        mpEnd = mpEnd + 1;
        return;
    }

    int count = (int)(mpEnd - mpBegin);
    int newCapacity;
    cTimerElem* buffer;
    if (count == 0) {
        newCapacity = 1;
    } else {
        newCapacity = count * 2;
        if (newCapacity == 0) {
            buffer = 0;
            goto haveBuffer;
        }
    }
    buffer = (cTimerElem*)EastlAllocate(newCapacity * 0x28, "Simulator", 0, 0,
        "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h",
        0xd1);
haveBuffer:
    cTimerElem* mid = TimerConstructRange(mpBegin, position, buffer);
    TimerDestroyRange(mpBegin, position, buffer);
    if (mid)
        mid->Construct(&value);

    cTimerElem* newEnd = TimerConstructRange(position, mpEnd, mid + 1);
    TimerDestroyRange(position, mpEnd, mid + 1);

    if (mpBegin != 0 && ((int*)mpBegin)[-1] != 0)
        EastlFree(mpBegin);

    mpEnd = newEnd;
    mpBegin = buffer;
    mpCapacity = (cTimerElem*)((char*)buffer + newCapacity * 0x28);
}

// ===========================================================================
// 0x00c7a1f0  per-source key bookkeeping on the player's capture data
// ===========================================================================
// Retail counts every distinct 64-bit id of the caller's 0x1d8-stride record
// list into a local map, looks each distinct id up in the active object
// manager, reads eight named integer properties (and their text forms) and
// then, `count` times per id, re-inserts two key vectors into the player's key
// pool and randomly erases keys from the vectors named by the properties.
//
// The property-name block at 0x52c6cc0, the two list names at 0x46bf76c /
// 0x4973416, the distribution table at 0x1601760 and the float at 0x1470f1c
// are read by retail but are not part of the analysis image, so they are
// declared as external data with their retail addresses recorded.
namespace {

typedef uint32_t u32;
typedef uint64_t u64;

struct cKey {                     // 8-byte key, halves ordered like the record
    u32 mPart0;                   // +0x00  (built from the record's +0x04 word)
    u32 mPart1;                   // +0x04  (built from the record's +0x00 word)
};

struct cKeyNode {                 // node of the local (key -> count) map
    uint64_t  mKey;               // +0x00  (retail compares it as one qword)
    u32       mCount;             // +0x08
    u32       mPad0c;             // +0x0c
    cKeyNode* mpNext;             // +0x10
};

struct cPoolKeyRecord {           // 0xc-stride record of the conversion vectors
    u32 mLo;                      // +0x00
    u32 mPad04;                   // +0x04
    u32 mHi;                      // +0x08
};

struct cSourceRecord {            // 0x1d8-stride record of the caller's list
    u32     mLo;                  // +0x00
    u32     mHi;                  // +0x04
    u32     mPad08;               // +0x08
    u32     mLimit;               // +0x0c
    uint8_t mPad10[0x1d8 - 0x10];
};

struct cConversionObject {        // ref-counted object from the active manager
    virtual void v0();
    virtual void Release();                                    // +0x04
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual bool GetProperty(const char* name, void** out);     // +0x24
};

struct cObjectManager {           // active object manager (0x0067de30)
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual bool Find(u32 lo, u32 hi, cConversionObject** out); // +0x2c
};

// RAII handle used for the manager result: the shipped code constructs it
// (pointer cleared), re-checks it around the lookup and releases it when the
// per-key block ends.
struct cObjectRef {
    cConversionObject* mpObject;                               // +0x00

    cObjectRef() : mpObject(0) {}
    cConversionObject** operator&() { return &mpObject; }
    cConversionObject* operator->() const { return mpObject; }
    operator cConversionObject*() const { return mpObject; }
    // Releases the held object and drops the reference.
    void Reset()
    {
        cConversionObject* pObject = mpObject;
        mpObject = 0;
        if (pObject != 0)
            pObject->Release();
    }
};

struct cKeyPool {                 // hashtable of 64-bit keys at cPlayer+0x10e8
    void Insert(u32 lo, u32 hi, int extra);   // 0x00596da0
    bool Contains(u32 lo, u32 hi);            // 0x00595110
    void Erase(u32 lo, u32 hi, int extra);    // 0x005973a0
};

// ---------------------------------------------------------------------------
// The local (64-bit key -> count) map.  Its layout is the game's fixed-size
// eastl hash map: the hashtable header, the inline bucket buffer, the inline
// node pool, and further inline storage the slice does not name.
// ---------------------------------------------------------------------------
struct cHashFunctor { uint8_t mDummy; };

struct cFixedHashtable {          // 0x00 .. 0x34
    uint8_t   mFunctors[0x04];    // +0x00  empty hash/equality keys
    cKeyNode** mpBucketArray;     // +0x04
    u32       mnBucketCount;      // +0x08
    u32       mnElementCount;     // +0x0c
    float     mfMaxLoadFactor;    // +0x10
    float     mfGrowthFactor;     // +0x14
    u32       mnNextResize;       // +0x18
    void*     mpFreeListHead;     // +0x1c  (inline allocator)
    void*     mpPoolNext;         // +0x20
    void*     mpPoolBegin;        // +0x24
    void*     mpPoolCapacity;     // +0x28
    u32       mnNodeSize;         // +0x2c
    void*     mpBucketBuffer;     // +0x30

    // 0x00c78860  find-or-insert of the node's count word
    u32& operator[](const cKey& key);
    // 0x005941f0  release every node of the bucket array
    void FreeNodes(cKeyNode** buckets, u32 count);

    // fixed_hashtable_allocator::deallocate, inlined by the shipped build
    void DeallocateBucketArray(void* pBlock)
    {
        if (pBlock != mpBucketBuffer) {
            if (pBlock >= mpPoolBegin && pBlock < mpPoolCapacity)
                *(void**)pBlock = mpFreeListHead;
            else
                EastlFree(pBlock);
        }
    }
    // The shipped build tears the container down with an explicit call at the
    // end of the function (no exception-unwind registration is emitted).
    void Destroy()
    {
        FreeNodes(mpBucketArray, mnBucketCount);
        mnElementCount = 0;
        if (mnBucketCount > 1)
            DeallocateBucketArray(mpBucketArray);
    }
};

struct cLocalMap : cFixedHashtable {   // 0x45c on the stack
    // 0x00c78790  fixed_hash_map(hash, equal)
    void Construct(const cHashFunctor& hash, const cHashFunctor& equal);

    uint8_t mBucketBuffer[0x88];  // +0x34  inline bucket array (34 slots)
    uint8_t mNodeBuffer[0x300];   // +0xbc  inline 0x18-stride node pool
    uint8_t mInlineTail[0x9C];    // +0x3bc further inline container storage
};

// external helpers (retail addresses; their bodies live in the game)
cObjectManager* ActiveObjectManager();                       // 0x0067de30
void ConvertNamedValue(cConversionObject* object, const char* name,
                       void* outA, void* outB);              // 0x006a0ae0

// 0x00a68fb0: cRandomGlobal::RandomUint32Uniform, __thiscall on 0x01601760.
struct cRandomGlobal {
    u32 RandomUint32Uniform(u32 limit);
};

// external data (retail addresses in the comments)
extern char        g_propertyKeys[];   // 0x0052c6cc0  the eight property names
extern char        g_propertyText[];   // 0x0052c6cda  their text forms
extern char        g_listNameA[];      // 0x0046bf76c
extern char        g_listNameB[];      // 0x004973416
extern const float g_captureRatio;     // 0x01470f1c

struct cCaptureOwner {            // the player view the pool handle belongs to
    uint8_t   mPad0000[0x10e8];
    cKeyPool* mpKeyPool;          // +0x10e8

    void Update(void* source);
};

} // namespace

// @ 0x00c7a1f0
void cCaptureOwner::Update(void* source)
{
    if (mpKeyPool == 0)
        return;
    if (source == 0)
        return;

    cHashFunctor functor;
    cLocalMap    map;
    map.Construct(functor, functor);

    // Count every distinct id of the caller's 0x1d8-stride record list.
    cSourceRecord* begin = *(cSourceRecord**)((uint8_t*)source + 0x98);
    cSourceRecord* end   = *(cSourceRecord**)((uint8_t*)source + 0x9c);
    int count = end - begin;
    for (unsigned i = 0; i < (unsigned)count; ++i) {
        cSourceRecord* record = begin + i;
        if (record->mLimit < i)
            continue;
        cKey key;
        key.mPart0 = record->mHi;
        key.mPart1 = record->mLo;
        ++map[key];
    }

    // Walk the map through its bucket array; the end iterator is the bucket
    // slot at index `mnBucketCount`.
    cKeyNode** bucket = map.mpBucketArray;
    cKeyNode*  node   = *bucket;
    if (node == 0) {
        do {
            ++bucket;
        } while (*bucket == 0);
        node = *bucket;
    }

    while (node != map.mpBucketArray[map.mnBucketCount]) {
        u64 key     = node->mKey;
        u32 keyLo   = (u32)key;
        // Retail shifts the node key by a 32-bit count and calls __aullshr
        // (0x011e08d0) for it; keep the count in a variable to get that form.
        int hiShift = 32;
        u32 keyHi   = (u32)(key >> hiShift);
        u32 repeats = node->mCount;

        {
        cObjectRef object;
        cObjectManager* manager = ActiveObjectManager();
        if (object != 0)
            object.Reset();
        if (manager->Find(keyLo, keyHi, &object)) {
            u32   countA = 0;
            u32   countB = 0;
            void* baseA  = 0;
            void* baseB  = 0;
            ConvertNamedValue(object, g_listNameA, &countA, &baseA);
            ConvertNamedValue(object, g_listNameB, &countB, &baseB);

            u32   byName[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
            void* vectors[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            u32   byText[8]  = {0, 0, 0, 0, 0, 0, 0, 0};

            unsigned i = 0;
            for (int n = 8; n != 0; --n, ++i) {
                void* property;
                if (object != 0 && object->GetProperty(&g_propertyKeys[i], &property)) {
                    uint8_t* entry = (uint8_t*)property;
                    if (*(unsigned short*)(entry + 0x12) == 9) {
                        if (entry[0x10] & 0x30)
                            entry = *(uint8_t**)entry;
                        byName[i] = *(u32*)entry;
                    }
                }
                byText[i] = 0;
                ConvertNamedValue(object, &g_propertyText[i], &byText[i], &vectors[i]);
                if ((int)byName[i] >= (int)byText[i])
                    byName[i] = byText[i];
            }

            for (u32 n = repeats; n > 0; --n) {
                for (u32 j = 0; j < countA; ++j) {
                    cPoolKeyRecord* record = (cPoolKeyRecord*)((uint8_t*)baseA + j * 0xc);
                    mpKeyPool->Insert(record->mLo, record->mHi, 0);
                }
                for (u32 j = 0; j < countB; ++j) {
                    cPoolKeyRecord* record = (cPoolKeyRecord*)((uint8_t*)baseB + j * 0xc);
                    mpKeyPool->Insert(record->mLo, record->mHi, 0);
                }
                for (u32 j = 0; j < 8; ++j) {
                    cPoolKeyRecord* records = (cPoolKeyRecord*)vectors[j];
                    int limit = (int)byName[j];
                    for (int k = 0; k < limit; ++k) {
                        cPoolKeyRecord* pick;
                        u32 attempt = 0;
                        do {
                            u32 index = ((cRandomGlobal*)0x01601760)->RandomUint32Uniform(byText[j]);
                            pick = records + index;
                            if (!mpKeyPool->Contains(pick->mLo, pick->mHi))
                                break;
                            ++attempt;
                        } while ((float)(int)byText[j] * g_captureRatio > (float)attempt);
                        mpKeyPool->Erase(pick->mLo, pick->mHi, 0);
                    }
                }
            }
        }
        if (object != 0)
            object->Release();
        }

        node = node->mpNext;
        if (node == 0) {
            do {
                ++bucket;
            } while (*bucket == 0);
            node = *bucket;
        }
    }

    map.Destroy();
}
