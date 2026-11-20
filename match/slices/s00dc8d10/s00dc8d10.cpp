// Slice s00dc8d10 -- EASTL vector<T>::insert(position, n, value) instantiation for a
// 4-byte intrusive reference-counted handle: copying an element AddRefs the pointee
// through vtable slot 0x60, destroying one Releases through vtable slot 0x64.
// Flags: /O2 /MD /Gy /EHsc /TP
#include "types.h"

typedef unsigned int u32;

#define VFN(p, off) ((*(void***)(p))[(off) / 4])

typedef void (__thiscall* RefCall)(void*);

static inline void RefAddRef(void* p)  { ((RefCall)VFN(p, 0x60))(p); }
static inline void RefRelease(void* p) { ((RefCall)VFN(p, 0x64))(p); }

// Element type: a single intrusive ref-counted handle.
struct Ref { void* mpObj; };

// The uninitialised-copy helper (0x00dc6730) reports its end cursor through an
// explicit out parameter, and takes one further trailing pointer the body never
// reads; both branches below pass the insertion position there.
void __cdecl UninitCopy(Ref** ppCursor, Ref* first, Ref* last, Ref* dest, Ref* position);
void __cdecl CopyBackward(Ref* first, Ref* last, Ref* dest);                                // 0x00ae68d0
void __cdecl Fill(Ref* first, Ref* last, const Ref* value);                                 // 0x00dc6780
void __cdecl FillN(Ref* dest, u32 n, const Ref* value, Ref* position);                       // 0x00b0ce40
void* __cdecl EAAlloc(u32 size, const char* name, int a, int b, const char* file, int line); // 0x00f473a0
void  __cdecl EAFree(void* p);                                                              // 0x00f47380
void* __cdecl BlockCopy(void* dst, const void* src, u32 bytes);                             // memmove, 0x011e0744

extern char c_013ebb38[];   // 0x13ebb38 path string handed to the allocator
extern char c_013f09b4[];   // 0x13f09b4 allocator name string

struct RefVec {
    Ref* mpBegin;       // +0x00
    Ref* mpEnd;         // +0x04
    Ref* mpCapacity;    // +0x08

    void insert(Ref* position, u32 n, const Ref& value);   // 0x00dc8d10
};

// @ 0x00dc8d10
void RefVec::insert(Ref* position, u32 n, const Ref& value)
{
    if (n <= (u32)(mpCapacity - mpEnd))
    {
        if (n > 0)
        {
            // Keep a private copy so a value that lives inside this vector stays valid.
            const Ref temp = value;
            if (temp.mpObj != 0)
                RefAddRef(temp.mpObj);

            const u32 nExtra = (u32)(mpEnd - position);

            if (n < nExtra)
            {
                // Fewer copies than elements after the position: grow the tail into the
                // uninitialised slots, shift the rest right, then fill the hole.
                // The pre-increment end is spelled from mpEnd so the original's register
                // allocation (n in ebx, position in edi) comes out the same.
                Ref* pCursor;
                UninitCopy(&pCursor, mpEnd - n, mpEnd, mpEnd, position);
                mpEnd += n;
                CopyBackward(position, mpEnd - n - n, mpEnd - n);
                Fill(position, position + n, &temp);
            }
            else
            {
                // At least as many copies as elements after the position: append the
                // surplus, move the tail past it, then fill the hole.
                Ref* const pEnd = mpEnd;
                FillN(pEnd, n - nExtra, &temp, position);
                mpEnd += n - nExtra;
                Ref* pCursor;
                UninitCopy(&pCursor, position, pEnd, mpEnd, position);
                mpEnd += nExtra;
                Fill(position, pEnd, &temp);
            }

            if (temp.mpObj != 0)
                RefRelease(temp.mpObj);
        }
    }
    else
    {
        // Not enough capacity: build a bigger block and splice the three runs in.
        const u32 nPrevSize = (u32)(mpEnd - mpBegin);
        const u32 nGrowSize = (nPrevSize > 0) ? nPrevSize * 2 : 1;
        const u32 nNewSize  = (nGrowSize > (nPrevSize + n)) ? nGrowSize : (nPrevSize + n);

        Ref* pNewData = nNewSize
            ? (Ref*)EAAlloc(nNewSize * 4, c_013f09b4, 0, 0, c_013ebb38, 0xd1)
            : 0;

        // The two runs are spliced with memmove; the size is the raw byte distance and the
        // advanced pointer is the element count, exactly the split the original shows.
        Ref* pMid = (Ref*)BlockCopy(pNewData, mpBegin,
                                    (u32)((char*)position - (char*)mpBegin))
                  + (u32)(position - mpBegin);

        FillN(pMid, n, &value, position);

        Ref* pNewEnd = (Ref*)BlockCopy(pMid + n, position,
                                       (u32)((char*)mpEnd - (char*)position))
                     + (u32)(mpEnd - position);

        if (mpBegin != 0 && *(int*)((char*)mpBegin - 4) != 0)
            EAFree(mpBegin);

        mpBegin    = pNewData;
        mpEnd      = pNewEnd;
        mpCapacity = pNewData + nNewSize;
    }
}
