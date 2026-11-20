// @ 0x00568f00
// /Od /Ob1 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

inline void* operator new(unsigned int, void* p) { return p; }

namespace eastl {
struct Pod8 { unsigned int a, b; };

struct DequeIterator {
    Pod8* mpCurrent;
    Pod8* mpBegin;
    Pod8* mpEnd;
    Pod8** mpCurrentArrayPtr;
};

class deque2 {
public:
    Pod8** mpPtrArray;
    unsigned int mnPtrArraySize;
    DequeIterator mItBegin;
    DequeIterator mItEnd;
    unsigned int mAllocator;

    void DoPushFront(const Pod8& value); // 0x00569d20
    void push_front(const Pod8& value);
};

void deque2::push_front(const Pod8& value)
{
    if (mItBegin.mpCurrent != mItBegin.mpBegin) {
        --mItBegin.mpCurrent;
        new (mItBegin.mpCurrent) Pod8(value);
    } else {
        DoPushFront(value);
    }
}
}
