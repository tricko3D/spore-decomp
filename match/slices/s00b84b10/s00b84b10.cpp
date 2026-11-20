// @ 0x00b84b10
//
// Copy constructor of a vector of 24-byte elements.  The element layout is
// taken from the out-of-line uninitialized-copy helper at 0x00b7f5c0, which
// copies 0x18-byte records field by field:
//   +0x00 float, +0x04 float, +0x08 float, +0x0c float,
//   +0x10 u8, +0x11 u8, +0x14 pointer.
// The allocation goes through the six-argument operator new (size, name,
// flags, debug flags, file, line 209) used by the EASTL allocator.
//
// Retail hands the helper's out-parameter as the address of its own incoming
// source-vector argument (the parameter is dead once its begin/end have been
// read), and passes that same source vector again as the allocator-like fifth
// argument.

typedef unsigned int size_t;

void* operator new(size_t size, const char* pName, int flags,
                   unsigned int debugFlags, const char* pFile, int line);

struct Elem {
    float f00;
    float f04;
    float f08;
    float f0c;
    unsigned char b10;
    unsigned char b11;
    unsigned char pad12[2];
    void* p14;
};

class ElemVector;

// 0x00b7f5c0: copies [first, last) into the uninitialised range at dest and
// reports the final destination pointer through its first argument.  The fifth
// argument is the source container (debug allocator context); this
// specialisation does not read it.
void uninitialized_copy(Elem** out, const Elem* first, const Elem* last,
                        Elem* dest, const ElemVector* owner);

class ElemVector {
public:
    Elem* mpBegin;
    Elem* mpEnd;
    Elem* mpCapacityEnd;

    ElemVector* CopyCtor(const ElemVector* pSrc);
};

// @ 0x00b84b10
ElemVector* ElemVector::CopyCtor(const ElemVector* pSrc)
{
    const int n = (int)(pSrc->mpEnd - pSrc->mpBegin);

    Elem* p = n ? (Elem*)operator new((size_t)(n * (int)sizeof(Elem)),
                                      "EASTLAllocator", 0, 0, "vector.h", 209)
                : 0;

    mpBegin = p;
    mpEnd = p;
    mpCapacityEnd = p + n;

    Elem* pEnd;
    uninitialized_copy(&pEnd, pSrc->mpBegin, pSrc->mpEnd, p, pSrc);
    mpEnd = pEnd;
    return this;
}
