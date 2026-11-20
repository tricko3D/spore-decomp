// @ 0x00b1a350
#include "game/types.h"

struct Elem38 {
    char* mpNameBegin;
    char* mpNameEnd;
    char* mpNameCapacity;
    int pad0c;
    int f10, f14, f18, f1c, f20;
    char f24;
    char pad25[3];
    int f28, f2c, f30, f34;
};

struct Elem38Vector {
    Elem38* mpBegin;
    Elem38* mpEnd;
    Elem38* mpCapacity;
    int mAllocator;
    void __thiscall DoAllocate(int count, const int* allocator);
    Elem38Vector& __thiscall CopyConstruct(const Elem38Vector* source);
};

void __cdecl Elem38_UninitCopy5(Elem38** result, Elem38* first,
    Elem38* last, Elem38* destination, const void* allocator);

Elem38Vector& Elem38Vector::CopyConstruct(const Elem38Vector* source) {
    const Elem38Vector* original = source;
    DoAllocate(original->mpEnd - original->mpBegin, &original->mAllocator);
    // The input argument is no longer needed once the copy has been started.
    Elem38_UninitCopy5(reinterpret_cast<Elem38**>(&source),
        original->mpBegin, original->mpEnd, mpBegin, source);
    mpEnd = reinterpret_cast<Elem38*>(const_cast<Elem38Vector*>(source));
    return *this;
}
