// Exception cleanup entries for the enclosing constructor. Retail passes the
// enclosing frame in EBP; an explicit frame describes its live objects in C++.
struct CleanupObject01212e30 {
    void Destroy(); // 0x006abf10
};

struct CleanupFrame01212e30 {
    CleanupObject01212e30* object; // parent EBP - 0x14
    void* allocation;             // parent EBP - 0x10
};

void operator delete(void* pointer, const char* name, int flags,
                     unsigned int debugFlags, const char* file, int line);

// @ 0x01212e30
void __cdecl CleanupObject01212e30Entry(CleanupFrame01212e30* frame)
{
    frame->object->Destroy();
}

// Alternate cleanup entry 0x01212e38 releases storage if construction throws.
void __cdecl CleanupAllocation01212e30(CleanupFrame01212e30* frame)
{
    operator delete(frame->allocation, "Graphics", 0, 0, 0, 0);
}

// The trailing entry is a compiler-generated __CxxFrameHandler thunk. Its
// FuncInfo and EBP calling convention require reconstruction of the enclosing
// function's exception lifetimes to reproduce with ordinary C++.
