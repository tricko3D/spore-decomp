// @ 0x01213930
#include "SP/cViewer.h"

// The retail cleanup entries inherit the constructor's EBP. This interface
// makes the object saved at parent EBP - 0x10 explicit.
struct ObjectBase01213930 {
    void Destroy(); // 0x00a16a20
    char storage[4];
};

struct ReferenceBase01213930 {
    void Destroy(); // 0x005725f0
    char storage[4];
};

struct ViewerOwner01213930 {
    ObjectBase01213930 object;
    ReferenceBase01213930 references;
    char padding[4];
    SP::cViewer viewers[2];
    char paddingBeforeLastViewer[4];
    SP::cViewer lastViewer;
};

struct ConstructorFrame01213930 {
    ViewerOwner01213930* owner;
};

void __stdcall DestroyViewerArray01213930(void*, unsigned int, unsigned int,
                                        void (__thiscall*)(SP::cViewer*));
// MSVC exception array destructor iterator, 0x011e0b22.

void __cdecl CleanupObject01213930(ConstructorFrame01213930* frame)
{
    frame->owner->object.Destroy();
}

void __cdecl CleanupReferences01213930(ConstructorFrame01213930* frame)
{
    frame->owner->references.Destroy();
}

void __cdecl CleanupViewerArray01213930(ConstructorFrame01213930* frame)
{
    DestroyViewerArray01213930(frame->owner->viewers, sizeof(SP::cViewer), 2,
        reinterpret_cast<void (__thiscall*)(SP::cViewer*)>(0x007c4000));
}

void __cdecl CleanupLastViewer01213930(ConstructorFrame01213930* frame)
{
    frame->owner->lastViewer.~cViewer();
}

// The final retail entry dispatches with FuncInfo 0x0150183c through
// __CxxFrameHandler3. Generation requires the enclosing constructor's
// exception lifetimes, which are outside this cleanup symbol.
