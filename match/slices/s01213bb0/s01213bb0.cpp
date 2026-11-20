// Exception-unwind cleanup for a constructed TObjVec argument.
struct TObjVec {
    ~TObjVec(); // 0x00432e80
};

// Offsets are relative to the enclosing function's EBP, not this helper's.
struct VectorArgumentUnwindFrame {
    unsigned int constructedArguments; // EBP - 0x1c
    unsigned char savedLocals[0x1c];
    TObjVec* argument;                  // EBP + 4
};

// The original obtains the parent frame implicitly through EBP. Making it
// explicit preserves the cleanup's logic in ordinary C++. Its second entry
// destroys the AtomicRef at EBP - 0x14; the final entry is the EH dispatcher.
// Those compiler-generated entries require the enclosing function's EH table.
// @ 0x01213bb0
void __cdecl CleanupVectorArgument01213bb0(VectorArgumentUnwindFrame* frame)
{
    if (frame->constructedArguments & 1) {
        frame->constructedArguments &= ~1u;
        frame->argument->~TObjVec();
    }
}
