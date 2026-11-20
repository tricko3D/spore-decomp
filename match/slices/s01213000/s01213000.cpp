// Exception cleanup entries for the enclosing function at 0x00767590.
// Retail receives its parent frame through EBP. The explicit frame argument
// below expresses the cleanup operations in portable C++ instead.
struct AtomicRef01213000 {
    void* object;
    ~AtomicRef01213000(); // 0x007a9610
};
struct ResourceRef01213000 {
    void* object;
    ~ResourceRef01213000(); // 0x006ae110
};
struct CleanupFrame01213000 {
    ResourceRef01213000 resource; // parent EBP - 0x18
    unsigned int local14;
    union {
        void* allocation;
        AtomicRef01213000 reference;
    };                          // parent EBP - 0x10
};

void operator delete(void*, const char*, int, unsigned int, const char*, int);

// @ 0x01213000
void __cdecl CleanupAllocation01213000(CleanupFrame01213000* frame)
{
    operator delete(frame->allocation, "Graphics", 0, 0, 0, 0);
}

// Alternate unwind entry at 0x0121301a.
void __cdecl CleanupReference01213000(CleanupFrame01213000* frame)
{
    frame->reference.~AtomicRef01213000();
}

// Alternate unwind entry at 0x01213022.
void __cdecl CleanupResource01213000(CleanupFrame01213000* frame)
{
    frame->resource.~ResourceRef01213000();
}

// The final entry at 0x0121302a is the compiler-generated exception handler
// for FuncInfo 0x01500f18. Generating that entry requires the enclosing
// function's exception lifetimes and cannot be done by a standalone helper.
