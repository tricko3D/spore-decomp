// @ 0x01212dc0
#include "SP/cGraphicsResourceFactory.h"

// Retail unwind entries inherit EBP from ReadResourceArena and BuildArena.
// Explicit object pointers express their cleanup actions without assembly.
struct ArenaHeader01212dc0 {
    ~ArenaHeader01212dc0(); // 0x007635b0
};
struct ResourceDescriptor01212dc0 {
    ~ResourceDescriptor01212dc0(); // 0x006c0fb0
};

void __cdecl CleanupArenaHeader01212dc0(ArenaHeader01212dc0* arena)
{
    arena->~ArenaHeader01212dc0();
}

// Alternate entry 0x01212dcb: local descriptor at enclosing EBP - 0xc4.
void __cdecl CleanupResourceDescriptor01212dc0(ResourceDescriptor01212dc0* descriptor)
{
    descriptor->~ResourceDescriptor01212dc0();
}

// Alternate entry 0x01212de0: load state at enclosing EBP - 0x18.
void __cdecl CleanupArenaLoadState01212dc0(
    SP::cGraphicsResourceFactory::cArenaLoadState* state)
{
    state->~cArenaLoadState();
}

// The entries at 0x01212dd6 and 0x01212de8 are compiler-generated
// __CxxFrameHandler dispatch thunks for the two enclosing functions.
// Their FuncInfo and inherited frame require those functions' EH lifetimes.
