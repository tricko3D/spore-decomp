// @ 0x01213150
// Independent construction-unwind entries. The retail entries inherit EBP
// from the enclosing constructor; parentFrame makes that dependency explicit.
struct Base01213150 {
    void Destroy(); // 0x00a16a20
};
struct Member01213150 {
    void Destroy(); // 0x005725f0
};
struct Resource01213150 {
    void Release(); // 0x0069d350
};
struct AtomicReference01213150 {
    void Release(); // 0x007a9610
};
struct C68200 {
    void Release(); // 0x00768200
};

void __cdecl CleanupBase01213150(char* parentFrame)
{
    Base01213150* object = *reinterpret_cast<Base01213150**>(parentFrame - 0x10);
    object->Destroy();
}

// Alternate unwind entry 0x01213158.
void __cdecl CleanupMember01213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<Member01213150*>(object + 4)->Destroy();
}

// Alternate unwind entry 0x01213163.
void __cdecl CleanupResource0c01213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<Resource01213150*>(object + 0x0c)->Release();
}

// Alternate unwind entry 0x0121316e.
void __cdecl CleanupResource1001213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<Resource01213150*>(object + 0x10)->Release();
}

// Alternate unwind entry 0x01213179.
void __cdecl CleanupReference1401213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<AtomicReference01213150*>(object + 0x14)->Release();
}

// Alternate unwind entry 0x01213184.
void __cdecl CleanupResource2801213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<Resource01213150*>(object + 0x28)->Release();
}

// Alternate unwind entry 0x0121318f.
void __cdecl CleanupMember2c01213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<C68200*>(object + 0x2c)->Release();
}

// Alternate unwind entry 0x0121319a.
void __cdecl CleanupReference10001213150(char* parentFrame)
{
    char* object = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<AtomicReference01213150*>(object + 0x100)->Release();
}

// Entry 0x012131a8 dispatches FuncInfo 0x01501044 to __CxxFrameHandler
// through EAX. That compiler-generated thunk requires the enclosing
// constructor's exception lifetimes and cannot be expressed by these helpers.
