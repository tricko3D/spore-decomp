// Cleanup entries for an enclosing function's exception-unwind table.
// Retail passes the enclosing frame implicitly in EBP. These declarations
// make that dependency explicit; they cannot reproduce the funclet ABI.
struct TObjVecVec {
    ~TObjVecVec(); // 0x00432de0
};
struct SplitRecord {
    ~SplitRecord(); // 0x0079aeb0
};
namespace SP {
    class AtomicRef {
    public:
        ~AtomicRef(); // 0x0060d210
    };
}
struct VectorBase {
    ~VectorBase(); // 0x0050ea50
};

// Offsets below are measured from the enclosing function's EBP.
// The same storage has vector and base-vector lifetimes at different states.
// @ 0x01213d10
void __cdecl CleanupObjectVectors01213d10(char* parentFrame)
{
    reinterpret_cast<TObjVecVec*>(parentFrame - 0x110)->~TObjVecVec();
}

void __cdecl CleanupRecord01213d1b(char* parentFrame)
{
    reinterpret_cast<SplitRecord*>(parentFrame - 0x80)->~SplitRecord();
}
void __cdecl CleanupRecord01213d23(char* parentFrame)
{
    reinterpret_cast<SplitRecord*>(parentFrame - 0xf0)->~SplitRecord();
}
void __cdecl CleanupReference01213d2e(char* parentFrame)
{
    reinterpret_cast<SP::AtomicRef*>(parentFrame - 0x118)->~AtomicRef();
}
void __cdecl CleanupReference01213d39(char* parentFrame)
{
    reinterpret_cast<SP::AtomicRef*>(parentFrame - 0xf8)->~AtomicRef();
}
void __cdecl CleanupReference01213d44(char* parentFrame)
{
    reinterpret_cast<SP::AtomicRef*>(parentFrame - 0x128)->~AtomicRef();
}
void __cdecl CleanupReference01213d4f(char* parentFrame)
{
    reinterpret_cast<SP::AtomicRef*>(parentFrame - 0x120)->~AtomicRef();
}
void __cdecl CleanupVectorStorage01213d5a(char* parentFrame)
{
    reinterpret_cast<VectorBase*>(parentFrame - 0x110)->~VectorBase();
}
// The final entry at 0x01213d65 is the compiler-generated EH dispatcher.
// Recreating it requires the enclosing function and its lifetime metadata.
