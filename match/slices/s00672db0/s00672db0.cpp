// @ 0x00672db0
// Move-uninitialized helper for cBuildXHTMLDetokenizer::cVar::cAction: copy-constructs
// [first, last) into the storage at destination.  The inlined cAction copy constructor
// matches the one in s00672cf0 (fixed_string copy ctor + mX).
#include "game/types.h"
#include <intrin.h>

inline void* operator new(unsigned int, void* storage) { return storage; }

namespace eastl {
template <class T, int N, bool Overflow> class fixed_string {
public:
    T* mpBegin;
    T* mpEnd;
    T* mpCapacity;
    struct Allocator {
        int mPadding;
        T* mpBegin;
        T mBuffer[N];
    } mAllocator;

    void assign(const T* first, const T* last); // 0x00672750

    __forceinline fixed_string(const fixed_string& source)
    {
        T* buffer = mAllocator.mBuffer;
        mAllocator.mpBegin = buffer;
        _ReadWriteBarrier();
        mpCapacity = buffer + N;
        mpEnd = buffer;
        mpBegin = buffer;
        *buffer = 0;
        assign(source.mpBegin, source.mpEnd);
    }
};
}

namespace {
class cBuildXHTMLDetokenizer {
public:
    class cVar {
    public:
        struct cAction {
            eastl::fixed_string<wchar_t, 32, true> mString;
            int mX;

            __forceinline cAction(const cAction& source)
                : mString(source.mString), mX(source.mX) {}
        };
    };
};
}

typedef cBuildXHTMLDetokenizer::cVar::cAction DetokenizerAction;

// @ 0x00672db0
DetokenizerAction* MoveUninitializedDetokenizerActions(
    DetokenizerAction* first, DetokenizerAction* last,
    DetokenizerAction* destination)
{
    for (; first != last; ++first, ++destination) {
        if (destination)
            new (static_cast<void*>(destination)) DetokenizerAction(*first);
    }
    return destination;
}
