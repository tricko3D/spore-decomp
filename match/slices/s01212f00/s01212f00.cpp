// This address starts three separate MSVC exception cleanup funclets.
// Retail receives the enclosing frame in EBP; represent that frame explicitly
// here until the enclosing function's C++ exception lifetime is reconstructed.
struct CleanupObject01212f00 {
    void Destroy(); // 0x00a16a20
};
struct LocalObject01212f00 {
    void Destroy(); // 0x006c0fb0
};
struct ParentFrame01212f00 {
    CleanupObject01212f00* object; // parent EBP - 0x30
    LocalObject01212f00 local;    // parent EBP - 0x2c
};

// @ 0x01212f00
void __cdecl Cleanup01212f00(ParentFrame01212f00* frame)
{
    frame->object->Destroy();
}

// Second unwind entry, 0x01212f08: destroy the embedded object at +0x1c.
void __cdecl CleanupMember01212f00(ParentFrame01212f00* frame)
{
    reinterpret_cast<CleanupObject01212f00*>(
        reinterpret_cast<char*>(frame->object) + 0x1c)->Destroy();
}

// Third unwind entry, 0x01212f13: destroy the local in the parent frame.
void __cdecl CleanupLocal01212f00(ParentFrame01212f00* frame)
{
    frame->local.Destroy();
}
// The final entry at 0x01212f1b is the compiler's EH handler thunk.
// It requires the enclosing function's generated FuncInfo and cannot be
// expressed as a standalone ordinary C++ function with the retail ABI.
