// These unwind entries share the enclosing function's EBP frame.
// An explicit frame describes their cleanup actions in ordinary C++.
struct RefSlot01213340 {
    void Destroy(); // 0x007a9610
    void* value;
};
struct Resource01213340 {
    void Destroy(); // 0x006ae020
    void* value;
};
struct Job01213340 {
    void Destroy(); // 0x006ae110
    void* value;
};
struct CleanupFrame01213340 {
    RefSlot01213340 slot28;
    RefSlot01213340 slot24;
    Job01213340 job20;
    Job01213340 job1c;
    unsigned int reserved18;
    RefSlot01213340 slot14;
    Resource01213340 resource10;
};
void __cdecl FreeGraphics01213340(void*, const char*, int, int, int, int);

// @ 0x01213340
void __cdecl CleanupSlot2801213340(CleanupFrame01213340* frame)
{
    frame->slot28.Destroy();
}

// Alternate entry 0x01213348: tagged placement deallocation.
void __cdecl CleanupAllocation01213340(CleanupFrame01213340* frame)
{
    FreeGraphics01213340(frame->job20.value, "Graphics", 0, 0, 0, 0);
}

// Alternate entry 0x01213362.
void __cdecl CleanupSlot1401213340(CleanupFrame01213340* frame)
{
    frame->slot14.Destroy();
}

// Alternate entry 0x0121336a.
void __cdecl CleanupResource01213340(CleanupFrame01213340* frame)
{
    frame->resource10.Destroy();
}

// Alternate entry 0x01213372.
void __cdecl CleanupJob2001213340(CleanupFrame01213340* frame)
{
    frame->job20.Destroy();
}

// Alternate entry 0x0121337a.
void __cdecl CleanupJob1c01213340(CleanupFrame01213340* frame)
{
    frame->job1c.Destroy();
}

// Alternate entry 0x01213382.
void __cdecl CleanupSlot2401213340(CleanupFrame01213340* frame)
{
    frame->slot24.Destroy();
}
// Entry 0x0121338a is the compiler's EH dispatcher for FuncInfo
// 0x01501240. Its implicit EAX metadata argument and inherited EBP
// require the enclosing function's exception lifetimes to reproduce.
