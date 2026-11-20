// Slice s00b848f0 -- hash record table insert-or-find (retail 0x00b848f0).
//
// Modelled from the retail disassembly: a bucket table of 0x24-byte nodes
// (12-byte key at +0x00, 20-byte record at +0x0c, next pointer at +0x20).
// The owning container keeps the bucket array at +0x04, the bucket count at
// +0x08, the element count at +0x0c and a rehash policy at +0x10.
// The dev PDB layout differs, so retail offsets are modelled locally.

#include <intrin.h>

typedef unsigned int  u32;
typedef unsigned char u8;

struct MapKey {
    u32 m0;                 // +0x00
    u32 m4;                 // +0x04
    u32 m8;                 // +0x08
};

struct MapNode {
    MapKey    key;          // +0x00
    u32       rec[5];       // +0x0c
    MapNode  *pNext;        // +0x20
};

struct MapSlot {
    MapNode  *pNode;        // +0x00
    MapNode **ppSlot;       // +0x04
    u8        bInserted;    // +0x08
};

struct GrowInfo {
    u8  bGrow;              // +0x00
    u32 newSize;            // +0x04
};

struct HashPolicy {
    u8 pad[0x10];

    char Check(GrowInfo *pInfo, u32 size, u32 count, int flag);  // 0x00921440
};

struct KeyHashTable {
    u32       m00;          // +0x00
    MapNode **mpBuckets;    // +0x04
    u32       mSize;        // +0x08 bucket count
    u32       mCount;       // +0x0c element count
    HashPolicy mPolicy;     // +0x10 rehash policy subobject

    MapSlot *Insert(MapSlot *pOut, const MapKey *pKey, void *unused);
    MapNode *NewNode(const MapKey *pKey);   // 0x00b83b70
    void     Rehash(u32 newSize);           // 0x00b7f510
};

// @ 0x00b848f0
MapSlot *KeyHashTable::Insert(MapSlot *pOut, const MapKey *pKey, void *unused)
{
    // Retail starts the m0*0x25 chain in eax, then the m8*0xfd chain in edi, and
    // combines them (m0*0x25 + m4) + m8*0xfd.  cl otherwise schedules the wider
    // 0xfd product first and swaps the two chains' registers, so the m0 read is
    // volatile to pin its chain ahead of the m8 chain (no code is added).
    // Residual (both scheduler tie-breaks of the same instruction multiset):
    // cl adds m4 into the edi chain (add edi, ecx) where retail adds into eax,
    // and it emits pOut->pNode = pNew before the final mpBuckets reload/pops.
    u32 a = *(volatile u32 *)&pKey->m0 * 0x25;
    u32 hash = (a + pKey->m4) + pKey->m8 * 0xfd;
    u32 idx  = hash % mSize;
    MapNode  *pNode = mpBuckets[idx];
    MapNode **ppSlot = &mpBuckets[idx];

    while (pNode != 0) {
        if (pKey->m0 == pNode->key.m0 && pKey->m4 == pNode->key.m4 &&
            pKey->m8 == pNode->key.m8) {
            pOut->pNode     = pNode;
            pOut->bInserted = 0;
            pOut->ppSlot    = ppSlot;
            return pOut;
        }
        pNode = pNode->pNext;
    }

    GrowInfo info;
    mPolicy.Check(&info, mSize, mCount, 1);

    MapNode *pNew = NewNode(pKey);
    if (info.bGrow) {
        u32 size = info.newSize;
        idx = hash % size;
        Rehash(size);
    }

    pNew->pNext  = mpBuckets[idx];
    mpBuckets[idx] = pNew;
    mCount++;
    pOut->pNode  = pNew;
    _ReadWriteBarrier();
    pOut->ppSlot = &mpBuckets[idx];
    pOut->bInserted = 1;
    return pOut;
}
