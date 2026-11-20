// @ 0x00568f70
// /Od /Ob1 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

inline void* operator new(unsigned int, void* p) { return p; }

namespace eastl {
struct Pod8 { int first, second; };

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

    void DoPushBack(const Pod8& value); // 0x00569dc0
    void push_back(const Pod8& value);
};

void deque2::push_back(const Pod8& value)
{
    if (mItEnd.mpCurrent + 1 != mItEnd.mpEnd) {
        new (mItEnd.mpCurrent++) Pod8(value);
    } else {
        DoPushBack(value);
    }
}
}
