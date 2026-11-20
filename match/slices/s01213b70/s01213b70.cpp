// Exception cleanup for the vector pointer at parent EH frame offset +4.
// The retail entry receives the parent frame implicitly in EBP. This C++
// spelling makes that frame explicit; the adjacent EH handler and its metadata
// require reconstruction of the enclosing function to reproduce automatically.
struct RawVec8 {
    char* begin;
    char* end;
    char* cap;
    void dealloc(); // 0x0045daf0
};

struct VectorCleanupFrame {
    unsigned int savedFrameWord;
    RawVec8* vector;
};

// @ 0x01213b70
void __cdecl CleanupVector01213b70(VectorCleanupFrame* parentFrame)
{
    parentFrame->vector->dealloc();
}
