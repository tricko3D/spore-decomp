// Slice s00b214e0: global string -> int map lookup (clsid-string lookup).
// Flags: /O2 /MD /Gy /TP /arch:SSE
#include <string.h>
#include "types.h"

// ---------------------------------------------------------------- allocator
void* __cdecl EAAllocate(unsigned size, const char* name, int flags, int debugFlags,
                         const char* file, int line);   // 0x00f473a0
void  __cdecl opdel(void* p);                           // 0x00f47380
void  __cdecl DoInsertValueC(void* dst, const void* src, unsigned n);  // 0x011e0744

// ---------------------------------------------------------------- eastl string (char)
struct eString8 {
    char* mpBegin;                                      // +0x00
    char* mpEnd;                                        // +0x04
    char* mpCapacity;                                   // +0x08
    int   mAllocator;                                   // +0x0c  (eastl::allocator, stateless)

    // eastl::basic_string<char>::basic_string(const char*) / assign(const char*)
    __forceinline eString8(const char* p)
    {
        const char* pEnd = p + strlen(p);
        unsigned    n    = (unsigned)(pEnd - p);
        unsigned    size = n + 1;

        char* b;
        char* cap;
        if (size > 1)
        {
            b   = (char*)EAAllocate(size, (const char*)0x013f09b4, 0, 0, (const char*)0x013ebb38, 0xd1);
            cap = b + size;
        }
        else
        {
            b   = (char*)0x01667bac;                    // eastl empty string
            cap = (char*)(0x01667bac + 1);
        }
        mpBegin    = b;
        mpCapacity = cap;

        DoInsertValueC(b, p, n);
        char* e = b + (pEnd - p);
        mpEnd = e;
        *e = 0;
    }

    ~eString8()
    {
        if (mpCapacity - mpBegin > 1 && mpBegin != 0)
            opdel(mpBegin);
    }
};

// ---------------------------------------------------------------- game side
// The global string->int map (eastl rbtree: anchor node at this+4, which is also
// the value the find returns when the key is absent).
struct MapStrInt {
    void Find(void** out, const void* key);             // 0x00b20930
};

// @ 0x00b214e0
int __cdecl FUN_00b214e0(const char* name)
{
    eString8 key(name);
    // MSVC reuses the dead parameter's stack slot for this local (docs/codegen.md).
    void*& node = (void*&)name;
    ((MapStrInt*)0x01568530)->Find(&node, &key);

    return node == (void*)0x01568534 ? -1 : *(int*)((char*)node + 0x20);
}
