// @ 0x01212d70
#include "game/types.h"
#include "EA/Allocator/ZoneObject.h"
#include "EA/IO/SharedPointer.h"

// Cleanup entries for Loader::Load820. Retail supplies the enclosing frame
// through EBP; this view makes the two live stack slots explicit.
struct LoadBufferFrame01212d70 {
    EA::IO::SharedPointer* sharedBuffer; // enclosing EBP - 0x14
    void* requestAllocation;           // enclosing EBP - 0x10
};

void operator delete(void*, const char*, int, unsigned int, const char*, int);

void __cdecl CleanupBufferAllocation01212d70(LoadBufferFrame01212d70* frame)
{
    EA::Allocator::ZoneObject::operator delete(
        frame->sharedBuffer, "Graphics", 0, 0, 0, 0);
}

// Alternate entry 0x01212d8a destroys the shared-buffer handle.
struct SharedBufferHandle01212d70 {
    EA::IO::SharedPointer* pointer;
    ~SharedBufferHandle01212d70(); // 0x006855f0
};

void __cdecl CleanupBufferHandle01212d70(SharedBufferHandle01212d70* handle)
{
    handle->~SharedBufferHandle01212d70();
}

// Alternate entry 0x01212d92 releases the async request if construction throws.
void __cdecl CleanupRequestAllocation01212d70(LoadBufferFrame01212d70* frame)
{
    operator delete(frame->requestAllocation,
                    "Graphics/GraphicsFactoryAsyncRequest", 0, 0, 0, 0);
}

// Entry 0x01212dac dispatches to __CxxFrameHandler with the enclosing
// function's FuncInfo. Ordinary C++ generates this thunk from EH lifetimes;
// it cannot supply the inherited EBP ABI as a standalone function.
