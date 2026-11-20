// @ 0x01213230
// Unwind entries use the enclosing function's EBP. An explicit frame
// describes the objects destroyed at each entry without assembly.
void __fastcall FUN_0072a180(int* self);

struct MeshGroupRef01213230 {
    void* object;
    ~MeshGroupRef01213230(); // 0x00472520
};
struct AtomicRef01213230 {
    void* object;
    ~AtomicRef01213230(); // 0x007a9610
};
struct CleanupFrame01213230 {
    void* allocation;                 // parent EBP - 0xe8
    union {
        MeshGroupRef01213230 mesh;    // parent EBP - 0xe4
        AtomicRef01213230 reference;
    };
    char gap[0x3c];
    int span[5];                      // parent EBP - 0xa4
};
void __cdecl FreeAllocation01213230(void*, const char*, int, int, int, int);

void __cdecl CleanupSpan01213230(CleanupFrame01213230* frame)
{
    FUN_0072a180(frame->span);
}

// Alternate entry 0x0121323b.
void __cdecl CleanupMesh01213230(CleanupFrame01213230* frame)
{
    frame->mesh.~MeshGroupRef01213230();
}

// Alternate entry 0x01213246: tagged placement-allocation cleanup.
void __cdecl CleanupAllocation01213230(CleanupFrame01213230* frame)
{
    FreeAllocation01213230(frame->allocation,
        reinterpret_cast<const char*>(0x0140e154), 0, 0, 0, 0);
}

// Alternate entry 0x01213263.
void __cdecl CleanupReference01213230(CleanupFrame01213230* frame)
{
    frame->reference.~AtomicRef01213230();
}

// The generated handler at 0x0121326e passes FuncInfo 0x01501140
// in EAX to the CRT. Reproducing it requires the enclosing EH lifetimes.
