// @ 0x012129d0
// Unwind cleanup for the PNG thumbnail exporter. The retail entries obtain
// this frame through the enclosing function's EBP, rather than an argument.
struct PngWriterCleanup012129d0 {
    void Destroy(); // 0x0087d260
};

struct PngExportFrame012129d0 {
    PngWriterCleanup012129d0 writer; // enclosing EBP - 0x2a94
    char writerStorage[0x13];
    void* allocation;              // enclosing EBP - 0x2a80
};

void operator delete(void* pointer, const char* name, int flags,
                     unsigned int debugFlags, const char* file, int line);

void __cdecl CleanupPngAllocation012129d0(PngExportFrame012129d0* frame)
{
    operator delete(frame->allocation, "Graphics/PNGUtils", 0, 0, 0, 0);
}

// Alternate unwind entry at 0x012129ed.
void __cdecl CleanupPngWriter012129d0(PngExportFrame012129d0* frame)
{
    frame->writer.Destroy();
}

// The final entry at 0x012129f8 is the compiler-generated exception handler
// thunk for FuncInfo 0x01500784 and requires the enclosing function's metadata.
