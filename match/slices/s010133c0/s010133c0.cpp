// Slice s010133c0: weighted pick of an object from the player's home planet list.
// Helper types are local stand-ins: the function only reads a pointer vector at
// +0x2c of the object reached through SpaceGameGet()->[+0x30].
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-

void* StarManager();                          // 0x00b3d2a0
void* GetPlayerHomePlanet();                  // 0x01021370
void* SpaceGameGet();                         // 0x01002bd0
double FUN_010103f0(void* pObject);           // 0x010103f0
int FUN_00fec010(void* weights);              // 0x00fec010
void* EAAllocate(unsigned int size, const char* name, int flags, int debugFlags,
                 const char* file, int line); // 0x00f473a0
void EARelease(void* pBlock);                 // 0x00f47380

// The allocation site is eastl::allocator::allocate inlined: retail's call passes
// this header's __FILE__ at line 0xd1 (0x13ebb38 in the image).
#define EASTL_ALLOCATOR_FILE \
    "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h"

struct cStarManager {
    void FUN_00ba6490();                      // 0x00ba6490
};

struct PlanetVec {
    void** mpBegin;  // +0x00
    void** mpEnd;    // +0x04
    void** mpCap;    // +0x08
};

struct PlanetHolder {
    char mPad00[0x2c];
    PlanetVec mPlanets;  // +0x2c
};

struct cSpaceGame {
    char mPad00[0x30];
    PlanetHolder* mpPlanetHolder;  // +0x30
};

// A float vector: three pointers, allocated with EA's named array allocator.
// The retail build's local occupies 0x14 bytes (frame layout), so the type has
// two further words that this function never touches.
struct SpVectorF {
    float* mpBegin;
    float* mpEnd;
    float* mpCap;
    unsigned int mPad0;
    unsigned int mPad1;
};

struct IPickTarget {
    virtual void* FUN_00();                       // vtable +0x00
    virtual void* Pick(void* pObject, int arg);   // vtable +0x04
};

struct cPickOwner {
    int mPad00;    // +0x00
    int mPad04;    // +0x04
    int mField08;  // +0x08
    int mField0C;  // +0x0c
    void* Pick(IPickTarget* target, int arg);
};

// @ 0x010133c0
void* cPickOwner::Pick(IPickTarget* target, int arg)
{
    ((cStarManager*)StarManager())->FUN_00ba6490();
    GetPlayerHomePlanet();
    cSpaceGame* game = (cSpaceGame*)SpaceGameGet();
    PlanetVec* vec = &game->mpPlanetHolder->mPlanets;
    int count = (int)(vec->mpEnd - vec->mpBegin);

    SpVectorF weights;
    unsigned int n = (unsigned int)count;
    float* pWeights = n != 0
        ? (float*)EAAllocate(n * 4, "Simulator", 0, 0, EASTL_ALLOCATOR_FILE, 0xd1)
        : 0;
    weights.mpBegin = pWeights;
    weights.mpEnd = weights.mpCap = pWeights + n;
    for (int i = 0; i < count; ++i)
        ((unsigned int*)pWeights)[i] = 0;

    for (int i = 0; i < count; ++i)
        pWeights[i] = (float)FUN_010103f0(vec->mpBegin[i]);

    // The block header is the word before the first element; a zero header
    // marks a block that is not owned.
    int idx = FUN_00fec010(&weights);
    if (idx < count) {
        void* object = vec->mpBegin[idx];
        if (object != 0) {
            mField08 = 0;
            mField0C = 0;
            void* result = target->Pick(object, arg);
            if (pWeights != 0 && ((const int*)pWeights)[-1] != 0)
                EARelease(pWeights);
            return result;
        }
    }
    if (pWeights != 0 && ((const int*)pWeights)[-1] != 0)
        EARelease(pWeights);
    return 0;
}
