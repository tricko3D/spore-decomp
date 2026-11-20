// slice s00e24ba0 -- cSPUIPosse::find_item: an eastl lower_bound over the posse-item list
// (the vector at +0xa4 holds 8-byte entries of two refcounted pointers) keyed by the entry's
// key pointer, then a raw identity test on that key.  The lower_bound helper and its comparator
// are the file-local 0x00e23e30 / 0x00e23c00 (defined in the SPUIPosse slices).  The search
// entry takes a reference on both of its pointers and drops them on every exit path.
#include "types.h"

typedef void (__thiscall* FnVoid0)(void*);

// A refcounted, polymorphic UI item: vtable +0x00 AddRef, +0x04 Release.
struct NamedItem {
  virtual void AddRef();
  virtual void Release();
};

// The vector's 8-byte value type: the key item and the item it stands for.
struct ItemEntry {
  NamedItem* mField00;                                      // +0x00 key
  NamedItem* mField04;                                      // +0x04 item
};

// The item list at +0xa4: begin/end of the entry array.
struct ItemList {
  ItemEntry* mpBegin;                                       // +0x00
  ItemEntry* mpEnd;                                         // +0x04

  ItemEntry* begin() const { return mpBegin; }
  ItemEntry* end() const { return mpEnd; }
  bool       empty() const { return mpBegin == mpEnd; }
};

struct cSPUIPosse {
  uint8_t     mPad000[0xa4];                                // +0x00 .. +0xa3
  ItemList    mItems;                                       // +0xa4
  ItemEntry*  find_item(NamedItem* pKey);
};

// 0x00e23e30 eastl::lower_bound over 8-byte entries; 0x00e23c00 its comparator.
void* __cdecl FUN_00e23e30(void* first, void* last, void* val,
                            uint8_t (__cdecl* pred)(void*, void*));
uint32_t __cdecl FUN_00e23c00(void** a, void** b);

// One owning reference on a list entry pointer (vtable slot +0x00 / +0x04).
static void Acquire(NamedItem* p)
{
  if (p)
    ((FnVoid0)((void**)*(void**)p)[0])(p);
}

static void Release(NamedItem* p)
{
  if (p)
    ((FnVoid0)((void**)*(void**)p)[1])(p);
}

// @ 0x00e24ba0
ItemEntry* cSPUIPosse::find_item(NamedItem* pKey)
{
  if (mItems.empty())
    return mItems.end();

  // Search key: the entry's copy of the key plus the item the first entry stands for.
  ItemEntry value;
  Acquire(pKey);
  value.mField00 = pKey;
  ItemEntry* e = mItems.begin();
  Acquire(pKey);
  value.mField04 = e->mField04;
  Acquire(value.mField04);
  Release(pKey);

  ItemEntry* it = (ItemEntry*)FUN_00e23e30(mItems.begin(), mItems.end(), &value,
                                           (uint8_t (__cdecl*)(void*, void*))FUN_00e23c00);
  ItemEntry* end = mItems.end();
  if (it != end && it->mField00 == pKey) {
    Release(value.mField04);
    Release(value.mField00);
    return it;
  }
  Release(value.mField04);
  Release(value.mField00);
  return end;
}
