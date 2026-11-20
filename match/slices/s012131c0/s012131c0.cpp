// @ 0x012131c0
// Exception cleanup entries. Retail supplies the enclosing frame in EBP;
// explicit frame arguments express the same cleanup operations in C++.
void __fastcall FUN_0072a180(int* self);

struct AtomicReference012131c0 {
    void* object;
    ~AtomicReference012131c0(); // 0x007a9610
};

struct CleanupFrame012131c0 {
    AtomicReference012131c0 reference; // parent EBP - 0x1458
    char gap1454[0x0c];
    void* allocation;                 // parent EBP - 0x1448
    char gap1444[0x14];
    int span[5];                      // parent EBP - 0x1430
};

void operator delete(void*, const char*, int, unsigned int, const char*, int);

void __cdecl CleanupSpan012131c0(CleanupFrame012131c0* frame)
{
    FUN_0072a180(frame->span);
}

// Alternate entry at 0x012131cb.
void __cdecl CleanupReference012131c0(CleanupFrame012131c0* frame)
{
    frame->reference.~AtomicReference012131c0();
}

// Alternate entry at 0x012131d6: matching placement delete on unwind.
void __cdecl CleanupAllocation012131c0(CleanupFrame012131c0* frame)
{
    operator delete(frame->allocation, "RenderAsset", 0, 0, 0, 0);
}

// The entry at 0x012131f3 passes FuncInfo 0x015010c0 to the CRT EH
// handler in EAX. Generating this thunk requires the enclosing function's
// exception lifetimes; an ordinary C++ helper cannot express that ABI.
