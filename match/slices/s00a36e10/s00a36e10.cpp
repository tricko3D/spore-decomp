// @ 0x00a36e10
#include "game/types.h"

// Retail view used by the adjacent s00a36d60 slice. The generated
// PrimitiveAggregate header describes a different layout and vtable.
struct IPrimitive;
struct IPrimVec {
    IPrimitive** mpBegin;
    IPrimitive** mpEnd;
    void* mpCapacity;
    void* m0c;
    void* m10;
    void* m14;
    unsigned char mBuf[0x28];
    void erase(IPrimitive** first, IPrimitive** last); // 0x00e25bd0
};
struct PAggX {
    void* vtbl0;
    void* vtbl1;
    void* vtbl2;
    int mRefCount;
    IPrimVec mVec;
    bool UpdatePrimitives();
};

// @ 0x00a36e10
bool PAggX::UpdatePrimitives()
{
    IPrimVec& primitives = mVec;
    IPrimitive** end = primitives.mpEnd;
    IPrimitive** it = primitives.mpBegin;
    for (; it != end; ++it) {
        IPrimitive* element = *it;
        int elementSource = ((int (__thiscall*)(void*))(*(void***)element)[12])(element);
        int aggregateSource = ((int (__thiscall*)(void*))(*(void***)this)[12])(this);
        if (elementSource == aggregateSource)
            ((void (__thiscall*)(void*))(*(void***)element)[5])(element);
    }
    primitives.erase(primitives.mpBegin, primitives.mpEnd);
    return true;
}
