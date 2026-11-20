// @ 0x012133a0
// Placement-allocation unwind actions. Retail inherits the parent EBP;
// the explicit frame below identifies the allocation at parent EBP - 0x10.
struct AllocationFrame012133a0 {
    void* allocation;
};

// Tagged placement delete, retail 0x00f47410.
void __cdecl FreeTagged012133a0(void*, const char*, int, int, int, int);

void __cdecl CleanupAllocation012133a0(AllocationFrame012133a0* frame)
{
    FreeTagged012133a0(frame->allocation,
        reinterpret_cast<const char*>(0x013eb8a4), 0, 0, 0, 0);
}

// Alternate unwind entry 0x012133ba.
void __cdecl CleanupAllocation012133ba(AllocationFrame012133a0* frame)
{
    FreeTagged012133a0(frame->allocation,
        reinterpret_cast<const char*>(0x013eb8a4), 0, 0, 0, 0);
}

// Alternate unwind entry 0x012133d4.
void __cdecl CleanupAllocation012133d4(AllocationFrame012133a0* frame)
{
    FreeTagged012133a0(frame->allocation,
        reinterpret_cast<const char*>(0x013eb8a4), 0, 0, 0, 0);
}

// Entry 0x012133ee dispatches to __CxxFrameHandler3 with FuncInfo
// 0x015012b4 in EAX. The compiler must generate that entry from the
// enclosing function's exception lifetimes; it has no C++ call interface.
