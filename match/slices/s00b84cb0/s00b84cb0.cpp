// @ 0x00b84cb0
//
// vector<12-byte element>::insert(position, first, last) -- the range form of the
// EASTL vector insert (EASTL/vector.h DoInsertFromIterator with a random access
// iterator tag) for a trivially copyable three-dword element.  The fourth stack
// argument is the empty tag; the original only keeps `ret 0x10` for it.
//
// The helpers are EASTL's out-of-line copies: 0xa11310 / 0x898b80 copy into
// uninitialised storage (the latter reports the advanced destination through its
// first argument and takes the source container as a trailing, unread argument),
// 0xabced0 is copy_backward and 0xb84c80 is copy.
#include "game/types.h"

struct Vec12Elem { unsigned x, y, z; };

struct EastlVecBase { Vec12Elem* mpBegin; Vec12Elem* mpEnd; Vec12Elem* mpCap; };

extern "C" void* FUN_00f473a0(unsigned size, const char* tag, int a, int b, const char* file, int line);
extern "C" void  FUN_00f47380(void* p);
extern "C" void* FUN_00a11310(void* first, void* last, void* dest);          // uninitialized copy, T*/T*
extern "C" void  FUN_00898b80(void** out, void* first, void* last, void* dest, void* owner);
extern "C" void  FUN_00abced0(void* first, void* last, void* destEnd);        // copy_backward
extern "C" void  FUN_00b84c80(void* first, void* last, void* dest);          // copy

#define ALLOC_FILE "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h"
#define EASTL_TAG  "EASTL"

struct Vec12 : EastlVecBase
{
    void InsertRange(Vec12Elem* position, const Vec12Elem* first, const Vec12Elem* last, int unused);
};

void Vec12::InsertRange(Vec12Elem* position, const Vec12Elem* first, const Vec12Elem* last, int unused)
{
    if (first == last)
        return;

    unsigned count = (unsigned)(last - first);
    if (count <= (unsigned)(mpCap - mpEnd)) {
        // ---- the elements fit in the slack after position -------------------
        unsigned tail = (unsigned)(mpEnd - position);
        Vec12Elem* oldEnd = mpEnd;

        if (count < tail) {
            // the shifted tail and the source range do not overlap: extend the
            // uninitialised region by `count`, then shift and copy.
            Vec12Elem* cut = oldEnd - count;
            Vec12Elem* sink;
            FUN_00898b80((void**)&sink, cut, oldEnd, oldEnd, (void*)first);
            mpEnd += count;
            FUN_00abced0(position, cut, oldEnd);
            FUN_00b84c80((void*)first, (void*)last, position);
            return;
        }

        // the source range is longer than the tail: only its first `tail`
        // elements go to uninitialised memory, the rest is shifted.
        const Vec12Elem* fiTemp = first + tail;
        FUN_00898b80((void**)&first, (void*)fiTemp, (void*)last, oldEnd, (void*)first);
        mpEnd += count - tail;
        FUN_00898b80((void**)&first, position, oldEnd, mpEnd, (void*)first);
        mpEnd += tail;
        FUN_00abced0((void*)first, (void*)fiTemp, position + tail);
        return;
    }

    {
        // ---- grow ---------------------------------------------------------
        unsigned size = (unsigned)(mpEnd - mpBegin);
        unsigned cap2 = size * 2;
        if (size == 0)
            cap2 = 1;
        unsigned want = count + size;
        if (!(cap2 > want))
            cap2 = want;
        Vec12Elem* mem = cap2 != 0
                                   ? (Vec12Elem*)FUN_00f473a0(cap2 * 12, EASTL_TAG, 0, 0, ALLOC_FILE, 0xd1)
                                   : 0;
        Vec12Elem* cursor = (Vec12Elem*)FUN_00a11310(mpBegin, position, mem);
        FUN_00898b80((void**)&first, (void*)first, (void*)last, cursor, (void*)first);
        Vec12Elem* newEnd = (Vec12Elem*)FUN_00a11310(position, mpEnd, (void*)first);
        if (mpBegin != 0 && *(int*)((char*)mpBegin - 4) != 0)
            FUN_00f47380(mpBegin);
        mpBegin = mem;
        mpEnd = newEnd;
        mpCap = mem + cap2;
    }
}
