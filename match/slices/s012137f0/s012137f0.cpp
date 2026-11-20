// The retail entries are independent unwind actions sharing the parent's EBP.
// Ordinary C++ makes that inherited frame an explicit parameter.
struct RefVector { ~RefVector(); }; // 0x0041eb80
struct PropertyListPtr { ~PropertyListPtr(); }; // 0x004a9b10
struct RefHolderB { ~RefHolderB(); }; // 0x00472540
struct InstancePtr { ~InstancePtr(); }; // 0x007a9610
struct Elem7c { ~Elem7c(); }; // 0x0041f2d0

void __cdecl operator delete(void*, const char*, int, int, int, int);

// @ 0x012137f0
void __cdecl CleanupReferences012137f0(char* parentFrame)
{
    reinterpret_cast<RefVector*>(parentFrame - 0xd4)->~RefVector();
}

// Alternate unwind entry 0x012137fb.
void __cdecl CleanupProperties012137f0(char* parentFrame)
{
    reinterpret_cast<PropertyListPtr*>(parentFrame - 0xe4)->~PropertyListPtr();
}

// Alternate unwind entry 0x01213806.
void __cdecl CleanupHolder012137f0(char* parentFrame)
{
    reinterpret_cast<RefHolderB*>(parentFrame - 0xe0)->~RefHolderB();
}

// Alternate unwind entry 0x01213811: tagged placement-new failure.
void __cdecl CleanupAllocation012137f0(char* parentFrame)
{
    void* allocation = *reinterpret_cast<void**>(parentFrame - 0xdc);
    operator delete(allocation, reinterpret_cast<const char*>(0x013eb8a4),
                    0, 0, 0, 0);
}

// Alternate unwind entry 0x0121382e.
void __cdecl CleanupInstance012137f0(char* parentFrame)
{
    reinterpret_cast<InstancePtr*>(parentFrame - 0xdc)->~InstancePtr();
}

// Alternate unwind entry 0x01213839.
void __cdecl CleanupElement012137f0(char* parentFrame)
{
    reinterpret_cast<Elem7c*>(parentFrame - 0x88)->~Elem7c();
}

// The final entry dispatches FuncInfo 0x015016bc through __CxxFrameHandler.
// It must be emitted from the enclosing function's exception lifetimes.
