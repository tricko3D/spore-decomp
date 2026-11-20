// Compares two clsid -> count trees and reports, on a formatted line, every
// entry that is missing from the other tree or whose counts disagree.
// Reconstructed from the shipped binary; helper addresses noted per call.
#include <string.h>
#include "game/types.h"
#include "eastl/map.h"
#include "eastl/allocator.h"

typedef eastl::map<int, int>                      CountMap;
typedef eastl::rbtree_node<eastl::pair<const int, int> >        CountNode;
typedef eastl::map<int, const char*>              NameMap;
typedef eastl::rbtree_node<eastl::pair<const int, const char*> > NameNode;

extern NameMap g_clsidMap;                               // 0x0156854c
extern const char* g_clsidNameSlot;                      // 0x01568584
extern char gEmptyStringData[];                          // 0x01667bac (empty string byte)

eastl::rbtree_node_base* RBTreeIncrement(eastl::rbtree_node_base* node);  // 0x00921580
void FUN_00b21060(char* dst, const char* fmt, int value);                 // 0x00b21060
void OperatorDelete(void* p);                                            // 0x00f47380

// eastl::basic_string<char, eastl::allocator>: begin / end / capacity + the (empty) allocator,
// 0x10 bytes as in the shipped image.  Its constructor points at the shared empty string and its
// destructor releases a heap buffer only when the capacity says the buffer was allocated.
struct EaString8 {
  char* mpBegin;
  char* mpEnd;
  char* mpCapacity;
  eastl::allocator mAllocator;

  EaString8() {
    mpBegin = mpEnd = gEmptyStringData;
    mpCapacity = gEmptyStringData + 1;
  }
  ~EaString8() {
    if (mpCapacity - mpBegin > 1 && mpBegin)
      OperatorDelete(mpBegin);
  }
  const char* c_str() const { return mpBegin; }

  void assign(const char* first, const char* last);   // 0x00454cb0 (library instantiation)
  void append(const char* first, const char* last);   // 0x00455d60
  int  sprintf(const char* format, ...);              // 0x00472fe0

  char* erase(char* first, char* last) {
    if (first != last) {
      unsigned n = (unsigned)(mpEnd - last);
      memmove(first, last, n + 1);
      mpEnd = first + n;
    }
    return first;
  }
};

// The shipped build inlined the library assign(first, last) at its second call site.
static inline void AssignStringInlined(EaString8* pString, const char* first, const char* last)
{
  unsigned n    = (unsigned)(last - first);
  unsigned size = (unsigned)(pString->mpEnd - pString->mpBegin);
  if (n <= size) {                                    // fits the buffer: copy, then drop the tail
    memcpy(pString->mpBegin, first, n);
    pString->erase(pString->mpBegin + n, pString->mpEnd);
  } else {                                            // too long: fill the buffer, then append the rest
    memcpy(pString->mpBegin, first, size);
    pString->append(first + size, last);
  }
}

// clsid -> display name, falling back to "clsid 0x%08x" for unknown classes.
static inline const char* LookupClsidName(int clsid)
{
  NameNode* node  = (NameNode*)g_clsidMap.mAnchor.mpNodeParent;
  NameNode* where = (NameNode*)&g_clsidMap.mAnchor;
  while (node) {
    if (node->mValue.first >= clsid) {
      where = node;
      node = (NameNode*)node->mpNodeLeft;
    } else {
      node = (NameNode*)node->mpNodeRight;
    }
  }
  if (where != (NameNode*)&g_clsidMap.mAnchor && clsid >= where->mValue.first)
    return where->mValue.second;
  FUN_00b21060((char*)&g_clsidNameSlot, "clsid 0x%08x", clsid);
  return g_clsidNameSlot;
}

// Finds key in a clsid -> count tree; the header node doubles as end().  The clsid keys are
// unsigned, so the tree walk compares them unsigned.
static inline CountNode* FindCountNode(CountMap* pMap, unsigned clsid)
{
  CountNode* node  = (CountNode*)pMap->mAnchor.mpNodeParent;
  CountNode* where = (CountNode*)&pMap->mAnchor;
  while (node) {
    if ((unsigned)node->mValue.first >= clsid) {
      where = node;
      node = (CountNode*)node->mpNodeLeft;
    } else {
      node = (CountNode*)node->mpNodeRight;
    }
  }
  if (where != (CountNode*)&pMap->mAnchor && clsid >= (unsigned)where->mValue.first)
    return where;
  return (CountNode*)&pMap->mAnchor;
}

// @ 0x00b21a90
bool CompareClsidMaps(CountMap* pMap1, CountMap* pMap2)
{
  bool ok = true;
  EaString8 line;
  EaString8 names;

  for (eastl::rbtree_node_base* it = pMap1->mAnchor.mpNodeLeft;
       it != &pMap1->mAnchor; it = RBTreeIncrement(it)) {
    CountNode* node = (CountNode*)it;
    const char* name = LookupClsidName(node->mValue.first);
    names.assign(name, name + strlen(name));
    line.sprintf(" Class: %s Count: %d\n", names.c_str(), node->mValue.second);

    CountNode* other = FindCountNode(pMap2, (unsigned)node->mValue.first);
    if (other == (CountNode*)&pMap2->mAnchor) {
      line.sprintf("Map1 class ID not found in Map2: 0x%08x (count: %d)\n",
                   node->mValue.first, node->mValue.second);
      ok = false;
    } else if (node->mValue.first != other->mValue.first) {
      line.sprintf("Class count mismatch, classID = 0x%08x, count 1 = %d count2 = %d\n",
                   node->mValue.first, node->mValue.second, other->mValue.second);
      ok = false;
    }
  }

  eastl::rbtree_node_base* end2 = &pMap2->mAnchor;
  for (eastl::rbtree_node_base* it = pMap2->mAnchor.mpNodeLeft;
       it != end2; it = RBTreeIncrement(it)) {
    CountNode* node = (CountNode*)it;
    const char* name = LookupClsidName(node->mValue.first);
    AssignStringInlined(&names, name, name + strlen(name));
    line.sprintf(" Class: %s Count: %d\n", names.c_str(), node->mValue.second);

    if (FindCountNode(pMap1, (unsigned)node->mValue.first) == (CountNode*)&pMap1->mAnchor) {
      line.sprintf("Map2 class ID not found in Map1: 0x%08x (count: %d)\n",
                   node->mValue.first, node->mValue.second);
      ok = false;
    }
  }

  return ok;
}
