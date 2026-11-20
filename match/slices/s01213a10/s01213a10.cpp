// @ 0x01213a10
#include "game/types.h"

// Constructor-unwind entries for cShadowWorld. Retail shares the parent's
// EBP and loads the partially constructed object from EBP - 0x10.
// These entry points expose that saved object as an ordinary C++ argument.
struct ShadowReferenceBase {
    void Destroy(); // 0x005725f0
    char storage[8];
};
struct ShadowObjectBase {
    void Destroy(); // 0x00a16a20
    char storage[0x10];
};
struct ShadowLightingReference {
    void Destroy(); // 0x007a9610
    void* object;
};
struct ShadowCasterVector {
    void Destroy(); // 0x00781b10
    char storage[0x14];
};
struct ShadowStateVector {
    void Destroy(); // 0x00781cf0
    char storage[0x14];
};
struct ShadowRasterReference {
    void Destroy(); // 0x00576620
    void* object;
};
struct ShadowWorldConstruction {
    ShadowReferenceBase references;             // +0x00
    ShadowObjectBase object;                    // +0x08
    ShadowLightingReference mLightingWorld;     // +0x18
    ShadowCasterVector mShadowCasters;          // +0x1c
    ShadowStateVector mShadowStates;            // +0x30
    char shaderAndMapData[0xf4 - 0x44];
    ShadowRasterReference mShadowMaps[2];       // +0xf4
};
struct ShadowWorldUnwindFrame {
    ShadowWorldConstruction* owner; // parent's EBP - 0x10
};

void __cdecl CleanupShadowReferenceBase(ShadowWorldUnwindFrame* frame)
{
    frame->owner->references.Destroy();
}
void __cdecl CleanupShadowObjectBase(ShadowWorldUnwindFrame* frame)
{
    frame->owner->object.Destroy();
}
void __cdecl CleanupShadowLightingWorld(ShadowWorldUnwindFrame* frame)
{
    frame->owner->mLightingWorld.Destroy();
}
void __cdecl CleanupShadowCasters(ShadowWorldUnwindFrame* frame)
{
    frame->owner->mShadowCasters.Destroy();
}
void __cdecl CleanupShadowStates(ShadowWorldUnwindFrame* frame)
{
    frame->owner->mShadowStates.Destroy();
}
// The runtime array iterator destroys the two four-byte elements in reverse.
void __stdcall DestroyShadowRasterArray(void*, unsigned int, unsigned int,
                                       void (__thiscall*)(ShadowRasterReference*));
void __cdecl CleanupShadowMaps(ShadowWorldUnwindFrame* frame)
{
    DestroyShadowRasterArray(frame->owner->mShadowMaps,
        sizeof(ShadowRasterReference), 2,
        reinterpret_cast<void (__thiscall*)(ShadowRasterReference*)>(0x00576620));
}
// The final retail entry is __CxxFrameHandler3 with FuncInfo 0x01501910.
// Its implicit register ABI and compiler-owned metadata require the enclosing
// constructor's exception lifetimes to generate in C++.
