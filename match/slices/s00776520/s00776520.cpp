// @ 0x00776520
#include "game/types.h"

struct BoneVector {
    float x, y, z, w;
    BoneVector(const float* v, float weight)
        : x(v[0]), y(v[1]), z(v[2]), w(weight) {}
};

struct cRuntimeModelBuilder {
    char padding[0x12c];
    BoneVector mBoneB;
    void setBoneB(int unused, const float* v, float weight, int unused2);
};

void cRuntimeModelBuilder::setBoneB(int unused, const float* v, float weight, int unused2) {
    mBoneB = BoneVector(v, weight);
}
