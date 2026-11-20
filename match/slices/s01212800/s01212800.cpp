// @ 0x01212800
// The retail group contains independent EH cleanup entries. Their enclosing
// frame is explicit here; the original receives it implicitly in EBP.
struct MemberCleanup01212800 {
    void Destroy(); // 0x005725f0
};
struct InstanceCleanup01212800 {
    void Destroy(); // 0x007a9610
};
struct AtomicCleanup01212800 {
    void Destroy(); // 0x0060d210
};

void __cdecl CleanupMember01212800(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x14);
    reinterpret_cast<MemberCleanup01212800*>(object + 4)->Destroy();
}

// Alternate entry 0x0121280b.
void __cdecl CleanupInstance01212800(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x14);
    reinterpret_cast<InstanceCleanup01212800*>(object + 0xc)->Destroy();
}

// Alternate entry 0x01212820.
void __cdecl CleanupAtomic01212800(char* parentFrame)
{
    reinterpret_cast<AtomicCleanup01212800*>(parentFrame - 0x24)->Destroy();
}

void __cdecl operator delete(void*, const char*, int, int, int, int);

// Alternate entry 0x01212828: placement construction failure cleanup.
void __cdecl CleanupAllocation01212800(char* parentFrame)
{
    void* allocation = *reinterpret_cast<void**>(parentFrame + 4);
    operator delete(allocation, "Graphics", 0, 0, 0, 0);
}

// Entries 0x01212816 and 0x01212842 dispatch to __CxxFrameHandler
// with the enclosing functions' FuncInfo. Those thunks must be generated
// from their parents' exception lifetimes to preserve the inherited ABI.

// 0x012138a0 -- clear bit 0 of the global flag word.
//
// Body (14 bytes) is byte-exact: load the word, AND ~1, store it back.
// The retail SYMBOL is 24 bytes because the original had a C++ EH rethrow
// funclet after the ret (mov eax, [type]; jmp __CxxThrowException): the source
// that produces it needs a try block cl cannot prove non-throwing, which a
// volatile load/AND/store never is.  See nonmatching.txt.
#include "types.h"

extern volatile uint32_t g_16f33b78;   // 0x1633b78 (retail address)

// @ 0x012138a0
void __cdecl ClearFlagBit0()
{
    uint32_t v = g_16f33b78;
    g_16f33b78 = v & 0xfffffffeu;
}
