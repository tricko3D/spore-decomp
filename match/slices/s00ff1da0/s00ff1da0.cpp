// @ 0x00ff1da0
//
// Reconstructed from the retail disassembly (517 bytes, two stack arguments,
// ret 8).  The owning class and the entry type are not identified yet, so the
// names below are stand-ins; the layout is exactly what the code touches:
//
//   +0x24  int      count
//   +0x28  uint8    flags          (its population count is the "special" tally)
//   +0x7c  EntryVec vec
//
// Each entry is 0x38 bytes and carries a four byte tag at +0x34.
#include "types.h"
#include "sse_helpers.h"

typedef unsigned char uchar;

// Population count; VC++ 2008 inlines this from the game's own bit helper.
// Defined after the target so the reconstructed function is the first (and
// only) symbol the slice's object exposes.
static __forceinline int CountBits(uint32_t x);

struct Entry {
    int      pad00[13];
    uint32_t type;          // +0x34
};

struct EntryVec {
    Entry* begin;           // +0x00
    Entry* end;             // +0x04
    Entry* cap;             // +0x08

    Entry* erase(Entry* it);            // 0x00ff1600, returns the next entry
};

struct Splitter;

extern void __cdecl Finish(int mode, Splitter* self, void* out);       // 0x00be2440

struct Splitter {
    char     pad00[0x24];
    int      count;         // +0x24
    uchar    flags;         // +0x28
    char     pad29[0x7c - 0x29];
    EntryVec vec;           // +0x7c

    void trim();                        // 0x00ff07e0, drops one special entry
};

// 0x00ff1da0
void __fastcall SplitterApply(Splitter* self, int edx, int index, void* out)
{
    if (index >= self->count)
        return;

    // Split index into a "special" part and an "ordinary" part, then nudge the
    // two parts until they add up to index exactly.
    int a = SseRoundToInt((float)(CountBits(self->flags) * index) / (float)self->count);

    int remaining = self->count - CountBits(self->flags);
    int b = SseRoundToInt((float)(remaining * index) / (float)self->count);

    while (a + b < index) {
        if (CountBits(self->flags) > 0 && a < b)
            ++a;
        else
            ++b;
    }
    while (a + b > index) {
        if (b > a)
            --b;
        else
            --a;
    }

    // Drop ordinary entries until at most `remaining` of them are kept.
    for (Entry* it = self->vec.begin; it != self->vec.end; ) {
        if (it->type != 0x18ea1eb && b < remaining) {
            it = self->vec.erase(it);
            --remaining;
        } else {
            ++it;
        }
    }

    // Trim the special side down to `a`.
    while (a < CountBits(self->flags))
        self->trim();

    self->count = index;
    Finish(0, self, out);
}

static __forceinline int CountBits(uint32_t x)
{
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0f0f0f0fu;
    return (int)((x * 0x01010101u) >> 24);
}
