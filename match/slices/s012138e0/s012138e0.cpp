// @ 0x012138e0
// Exception cleanup actions for a partially constructed viewer owner.
// Retail invokes each entry with the enclosing function's EBP. The explicit
// frame argument below describes the same cleanup state in ordinary C++.
struct Object012138e0 {
    void Destroy(); // 0x00a16a20
    char storage[4];
};

struct RefCount012138e0 {
    void Destroy(); // 0x005725f0
    char storage[4];
};

struct Viewer012138e0 {
    void Destroy(); // 0x007c4000
    char storage[0x174];
};

struct ViewerOwner012138e0 {
    Object012138e0 object;
    RefCount012138e0 references;
    char padding[4];
    Viewer012138e0 viewers[2];
    char paddingBeforeLastViewer[4];
    Viewer012138e0 lastViewer;
};

struct ParentFrame012138e0 {
    ViewerOwner012138e0* owner; // parent EBP - 0x20
};

void __stdcall DestroyArray012138e0(void*, unsigned int, unsigned int,
                                  void (__thiscall*)(Viewer012138e0*));
// MSVC exception array destructor iterator, 0x011e0b22.

void __cdecl CleanupObject012138e0(ParentFrame012138e0* frame)
{
    frame->owner->object.Destroy();
}

void __cdecl CleanupReferences012138e0(ParentFrame012138e0* frame)
{
    frame->owner->references.Destroy();
}

void __cdecl CleanupViewerArray012138e0(ParentFrame012138e0* frame)
{
    DestroyArray012138e0(frame->owner->viewers, sizeof(Viewer012138e0), 2,
        reinterpret_cast<void (__thiscall*)(Viewer012138e0*)>(0x007c4000));
}

void __cdecl CleanupLastViewer012138e0(ParentFrame012138e0* frame)
{
    frame->owner->lastViewer.Destroy();
}

// The final retail entry is the compiler's exception dispatcher for FuncInfo
// 0x015017f8; reproducing that entry requires the enclosing C++ lifetimes.
