// @ 0x00a36e80
#include "game/types.h"
#include "EA/Audio/IPrimitive.h"

// Retail view through the aggregate's second interface, four bytes into
// the complete object. The generated aggregate layout is unverified.
struct PrimitiveRef {
    EA::Audio::IPrimitive* mpObject;
    PrimitiveRef(EA::Audio::IPrimitive* object) : mpObject(object) {
        if (object) object->AddRef();
    }
    ~PrimitiveRef() {
        if (mpObject) mpObject->Release();
    }
};

struct PrimitiveVector {
    unsigned char storage[0x40];
    void push_back(const PrimitiveRef& primitive); // 0x00a36ce0
};

struct PrimitiveAggregateView {
    void* vtable;
    unsigned int interfaceData[2];
    PrimitiveVector mPrimitives;
    bool mbDirty;
    unsigned char padding[7];
    int mModificationCount;
    bool AddPrimitive(EA::Audio::IPrimitive* primitive);
};

// @ 0x00a36e80
bool PrimitiveAggregateView::AddPrimitive(EA::Audio::IPrimitive* primitive)
{
    {
        PrimitiveRef reference(primitive);
        mPrimitives.push_back(reference);
    }
    --mModificationCount;
    mbDirty = true;
    return true;
}
