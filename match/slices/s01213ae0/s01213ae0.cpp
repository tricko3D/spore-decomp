// Unwind actions for a Graphics allocation and its constructed local reference.
struct AtomicRef {
    void* object;
    ~AtomicRef(); // 0x007a9610
};

struct GraphicsAllocationUnwindFrame {
    void* allocation;       // parent EBP - 0x1c
    AtomicRef reference;    // parent EBP - 0x18
};

void __cdecl FreeGraphicsAllocation(void*, const char*, int, int, int, int);
// FreeGraphicsAllocation is the tagged deallocator at 0x00f47410.

// @ 0x01213ae0
void __cdecl CleanupGraphicsAllocation01213ae0(GraphicsAllocationUnwindFrame* frame)
{
    FreeGraphicsAllocation(frame->allocation, "Graphics", 0, 0, 0, 0);
}

// Alternate unwind entry at 0x01213afa.
void __cdecl CleanupGraphicsReference01213ae0(GraphicsAllocationUnwindFrame* frame)
{
    frame->reference.~AtomicRef();
}

// The final retail entry dispatches through FuncInfo 0x015019d0. The
// compiler generates that metadata and entry from the enclosing function's
// exception lifetimes; the original cleanup entries inherit its EBP.
