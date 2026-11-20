// @ 0x007764c0
#include "game/types.h"

struct BoneVector {
    float x, y, z, w;
    BoneVector(const float* v, float weight)
        : x(v[0]), y(v[1]), z(v[2]), w(weight) {}
};

struct cRuntimeModelBuilder {
    char padding[0x120];
    BoneVector mBoneA;
    void setBoneA(int unused, const float* v, float weight, int unused2);
};

void cRuntimeModelBuilder::setBoneA(int unused, const float* v, float weight, int unused2) {
    mBoneA = BoneVector(v, weight);
}
