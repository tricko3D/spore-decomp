// @ 0x00b1a540
#include "game/types.h"

void* operator new(unsigned int size, const char* name, int flags,
    unsigned debugFlags, const char* file, int line);

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

void __cdecl Elem38_UninitCopy5(void* result, Elem38* first,
    Elem38* last, Elem38* destination, const void* allocator);

struct Elem38Vector {
    Elem38* mpBegin;
    Elem38* mpEnd;
    Elem38* mpCapacity;
    Elem38* __thiscall DoAllocateAndCopy(unsigned n, Elem38* first, Elem38* last);
};

Elem38* Elem38Vector::DoAllocateAndCopy(unsigned n, Elem38* first, Elem38* last) {
    Elem38* destination = n ? static_cast<Elem38*>(operator new(
        n * sizeof(Elem38), "Simulator", 0, 0,
        "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h",
        0xd1)) : 0;
    Elem38_UninitCopy5(&n, first, last, destination,
        reinterpret_cast<const void*>(n));
    return destination;
}
