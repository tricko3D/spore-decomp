// Unwind cleanup entries for the enclosing function. Retail supplies its
// frame through EBP; ordinary C++ expresses that frame as an argument.
struct CleanupObject01213280 {
    void Destroy(); // 0x0069d350
};

struct CleanupFrame01213280 {
    CleanupObject01213280 local; // parent EBP - 0x10
    char remainingFrame[0x13];
    void* allocation;            // parent EBP + 4
};

void __cdecl FreeAllocation01213280(void*, const char*, int, int, int, int); // 0x00f47410

// @ 0x01213280
void __cdecl CleanupLocal01213280(CleanupFrame01213280* frame)
{
    frame->local.Destroy();
}

// Alternate unwind entry at 0x01213288: release the allocation made with
// the tagged placement allocator.
void __cdecl CleanupAllocation01213280(CleanupFrame01213280* frame)
{
    FreeAllocation01213280(frame->allocation,
        reinterpret_cast<const char*>(0x013eb8a4), 0, 0, 0, 0);
}

// The final entry at 0x012132a2 is the generated exception handler for
// FuncInfo 0x01501174. Its frame convention and metadata require the
// enclosing function's C++ exception lifetimes to reproduce.
