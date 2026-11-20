// @ 0x00569290
// /Od /Ob1 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

struct Pair {
    int first;
    int second;
};

struct DequeBase {
    void Deallocate(void* p); // 0x00569260
    void FreeSubarrays(Pair** first, Pair** last);
};

void DequeBase::FreeSubarrays(Pair** first, Pair** last)
{
    while (first < last) {
        Pair* p = *first;
        Deallocate(p);
        ++first;
    }
}
