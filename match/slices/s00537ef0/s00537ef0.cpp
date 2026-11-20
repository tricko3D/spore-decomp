// @ 0x00537ef0
#include "cSPTransform.h"

// The transform assignment helper accepts an untyped source at retail.
struct cSPTransformOps {
    cSPTransformOps* Assign(const void* source); // 0x00537dc0
};

struct Host537ef0 {
    char mHeader[0x0c];
    cSPTransform mTransform;
    void Set(const cSPTransform& parent, const cSPTransform& local);
};

// @ 0x00537ef0
void Host537ef0::Set(const cSPTransform& parent, const cSPTransform& local)
{
    cSPTransform composed(local);
    composed.PreTransformBy(parent);
    cSPTransform result(composed);
    reinterpret_cast<cSPTransformOps*>(&mTransform)->Assign(&result);
}
