// These unwind entries receive their enclosing function's frame in EBP.
// The explicit frame parameter below expresses the cleanup in ordinary C++.
struct CleanupObject012129a0 {
    void Destroy(); // 0x006c0fb0
};

struct CleanupFrame012129a0 {
    unsigned int constructed;              // parent EBP - 0x30
    CleanupObject012129a0 local;            // parent EBP - 0x2c
    char remainingLocals[0x2f];
    CleanupObject012129a0* temporary;        // parent EBP + 4
};

// @ 0x012129a0
void __cdecl CleanupLocal012129a0(CleanupFrame012129a0* frame)
{
    frame->local.Destroy();
}

// Alternate unwind entry at 0x012129a8: clear the construction bit before
// destroying the temporary so a subsequent unwind cannot destroy it twice.
void __cdecl CleanupTemporary012129a0(CleanupFrame012129a0* frame)
{
    if (frame->constructed & 1) {
        frame->constructed &= ~1u;
        frame->temporary->Destroy();
    }
}

// The entry at 0x012129c1 is the compiler-generated EH handler, which loads
// FuncInfo and jumps to __CxxFrameHandler. Reproducing it requires the parent
// function's exception metadata and object lifetimes.
