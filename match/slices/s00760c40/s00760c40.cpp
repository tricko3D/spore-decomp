// @ 0x00760c40
#include "game/types.h"
#include <intrin.h>

namespace SP {
struct RefCountedAt8 {
    virtual void r0();
    int r1;
    long mRefCount;
};

void __cdecl move_ptrs(RefCountedAt8** first, RefCountedAt8** last,
                      RefCountedAt8** destination); // @ 0x006f43e0

struct PtrVec {
    RefCountedAt8** mpBegin;
    RefCountedAt8** mpEnd;
    RefCountedAt8** mpCapacity;
    RefCountedAt8** erase(RefCountedAt8** position);
};

RefCountedAt8** PtrVec::erase(RefCountedAt8** position)
{
    if (position + 1 < mpEnd)
        move_ptrs(position + 1, mpEnd, position);
    --mpEnd;
    RefCountedAt8* object = *mpEnd;
    if (object) {
        volatile long* count = &object->mRefCount;
        _InterlockedExchangeAdd(count, -1);
        if (_InterlockedExchangeAdd(count, 0) < 1) {
            _InterlockedExchangeAdd(count, 1);
            return position;
        }
        _InterlockedExchangeAdd(count, 0);
    }
    return position;
}
}
