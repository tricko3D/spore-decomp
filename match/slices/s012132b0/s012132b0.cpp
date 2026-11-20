// Retail contains independent unwind entries sharing the enclosing EBP frame.
// The explicit frame argument below expresses the objects each entry cleans up.
void __fastcall FUN_006ae020(void** resource);
void __fastcall FUN_006ae110(void** job);

struct CleanupFrame012132b0 {
    void* job28;
    void* job24;
    void* job20;
    void* job1c;
    void* job18;
    void* job14;
    void* resource10;
};

// @ 0x012132b0
void __cdecl CleanupResource012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae020(&frame->resource10);
}

// Alternate entry 0x012132b8.
void __cdecl CleanupJob1c012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job1c);
}

// Alternate entry 0x012132c0.
void __cdecl CleanupJob20012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job20);
}

// Alternate entry 0x012132c8.
void __cdecl CleanupJob24012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job24);
}

// Alternate entry 0x012132d0.
void __cdecl CleanupJob28012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job28);
}

// Alternate entry 0x012132d8.
void __cdecl CleanupJob18012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job18);
}

// Alternate entry 0x012132e0.
void __cdecl CleanupJob14012132b0(CleanupFrame012132b0* frame)
{
    FUN_006ae110(&frame->job14);
}

// The final entry is the compiler-generated exception handler using FuncInfo
// 0x01501198. Reproducing it requires the enclosing function's EH lifetimes.
