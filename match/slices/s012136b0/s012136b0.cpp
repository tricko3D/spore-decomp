// Retail exception-unwind entries inherit the enclosing function's EBP.
// These C++ entry points express the parent frame explicitly.
struct SpanBuffer012136b0 {
    void Destroy(); // 0x0072a180: release storage unless it is inline storage
};

void __cdecl FreeTagged012136b0(void*, const char*, int, int, int, int);
// Tagged placement delete, 0x00f47410.

// Frame offsets are relative to the enclosing function's EBP, rather than
// to a new cleanup function's stack frame.
struct ParentFrame012136b0 {
    char* base;
    SpanBuffer012136b0* BufferAt(int offset) const
    {
        return reinterpret_cast<SpanBuffer012136b0*>(base + offset);
    }
};

// @ 0x012136b0
void __cdecl CleanupFirstBuffer012136b0(ParentFrame012136b0* frame)
{
    frame->BufferAt(-0x2bc)->Destroy();
}

// Alternate cleanup entry 0x012136bb.
void __cdecl CleanupSecondBuffer012136b0(ParentFrame012136b0* frame)
{
    frame->BufferAt(-0x164)->Destroy();
}

// Alternate cleanup entry 0x012136d0.
void __cdecl CleanupLargeBuffer012136b0(ParentFrame012136b0* frame)
{
    frame->BufferAt(-0xa030)->Destroy();
}

// Alternate cleanup entry 0x012136db.
void __cdecl CleanupAllocation012136b0(ParentFrame012136b0* frame)
{
    void* allocation = *reinterpret_cast<void**>(frame->base - 0xa06c);
    FreeTagged012136b0(allocation,
        reinterpret_cast<const char*>(0x013eb8a4), 0, 0, 0, 0);
}

// Entries 0x012136c6 and 0x012136f8 are compiler-generated dispatchers
// for FuncInfo 0x01501580 and 0x015015b4. Their inherited frame ABI requires
// the enclosing functions' exception lifetimes to reproduce in C++.
