// eastl::vector<S18>::insert(position, n, value) -- 0x18-byte element type
// (same element layout as the copy/fill helpers at 0xb7f5c0/0xb7f630/0xb7f860).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-   (no /EHsc)
#include "types.h"

#define ALLOC_FILE \
    "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h"

// 0x18-byte element: two chars at +0x10/+0x11 leave +0x12/+0x13 implicit padding,
// which the original copies memberwise and never touches.
struct S18 {
    int   a;        // +0x00
    float b;        // +0x04
    float c;        // +0x08
    float d;        // +0x0c
    char  e;        // +0x10
    char  f;        // +0x11
    int   g;        // +0x14
    S18() {}
    S18(const S18& o) : a(o.a), b(o.b), c(o.c), d(o.d), e(o.e), f(o.f), g(o.g) {}
};

extern "C" void* F_f473a0(unsigned size, const char* name, int a, int b,
                          const char* file, int line);   // 0xf473a0 allocate
extern "C" void  F_f47380(void* p);                      // 0xf47380 deallocate

// copy(first, last, dst)
extern "C" S18*  F_00b7f630(const S18* first, const S18* last, S18* dst);

// uninitialized_copy(first, last, dst, owner): copies [first, last) into the
// uninitialised range at dst and reports the final destination pointer through
// its first (out) argument.  The fifth argument is the owning container (debug
// allocator context); this specialisation does not read it.
extern "C" S18** F_00b7f5c0(S18** out, const S18* first, const S18* last,
                            S18* dst, const S18* owner);

// fill_n(dst, count, value, owner)
extern "C" void  F_00b7f860(S18* dst, unsigned count, const S18* value,
                            const S18* owner);

// copy_backward(first, last, dstEnd)
extern "C" void  F_cc8b50(S18* first, S18* last, S18* dstEnd);

// fill(first, last, value)
extern "C" void  F_7144b0(S18* first, S18* last, const S18* value);

struct S18Vec {
    S18* mpBegin;   // +0x00
    S18* mpEnd;     // +0x04
    S18* mpCap;     // +0x08
    void insert(S18* pos, unsigned n, const S18* val);
};

// @ 0x00b84e90
void S18Vec::insert(S18* pos, unsigned n, const S18* val)
{
    if (n <= (unsigned)(mpCap - mpEnd)) {
        if (n > 0) {
            S18 value = *val;
            unsigned tail = (unsigned)(mpEnd - pos);
            S18* pOldEnd = mpEnd;
            if (n < tail) {
                S18* pSplit = pOldEnd - n;
                F_00b7f5c0((S18**)&val, pSplit, pOldEnd, pOldEnd, pos);
                mpEnd += n;
                F_cc8b50(pos, pSplit, pOldEnd);
                F_7144b0(pos, pos + n, &value);
                return;
            }
            unsigned fillNew = n - tail;
            F_00b7f860(pOldEnd, fillNew, &value, pos);
            mpEnd += fillNew;
            S18* p = pos;
            F_00b7f5c0((S18**)&pos, p, pOldEnd, mpEnd, p);
            mpEnd += tail;
            F_7144b0(p, pOldEnd, &value);
        }
        return;
    }

    unsigned size = (unsigned)(mpEnd - mpBegin);
    unsigned newCap = size * 2;
    if (size == 0)
        newCap = 1;
    unsigned need = size + n;
    if (need > newCap)
        newCap = need;
    S18* pNew;
    if (newCap != 0)
        pNew = (S18*)F_f473a0(newCap * 0x18, "Simulator", 0, 0, ALLOC_FILE, 0xd1);
    else
        pNew = 0;
    S18* pMid = F_00b7f630(mpBegin, pos, pNew);
    F_00b7f860(pMid, n, val, pos);
    S18* pNewEnd = F_00b7f630(pos, mpEnd, pMid + n);
    if (mpBegin != 0 && ((int*)mpBegin)[-1] != 0)
        F_f47380(mpBegin);
    mpBegin = pNew;
    mpEnd = pNewEnd;
    mpCap = pNew + newCap;
}
