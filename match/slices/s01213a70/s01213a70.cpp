#include "game/types.h"

// cShadowWorld destruction unwind actions. The retail entries inherit EBP
// from the destructor and read its saved this pointer at EBP - 0x10.
// The generated header's unverified layout differs from these retail offsets.
struct ShadowRefCountBase01213a70 { void Destroy(); };
struct ShadowObjectBase01213a70 { void Destroy(); };
struct ShadowLightingRef01213a70 { void Destroy(); };
struct ShadowCasterVector01213a70 { void Destroy(); };
struct ShadowStateVector01213a70 { void Destroy(); };
struct ShadowRasterRef01213a70 { void Destroy(); };

void __stdcall DestroyShadowRasterArray01213a70(
    void*, unsigned int, unsigned int, void (__thiscall*)(ShadowRasterRef01213a70*));

// @ 0x01213a70
void __cdecl CleanupShadowRefCount01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<ShadowRefCountBase01213a70*>(owner)->Destroy();
}

void __cdecl CleanupShadowObject01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<ShadowObjectBase01213a70*>(owner + 8)->Destroy();
}

void __cdecl CleanupShadowLighting01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<ShadowLightingRef01213a70*>(owner + 0x18)->Destroy();
}

void __cdecl CleanupShadowCasters01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<ShadowCasterVector01213a70*>(owner + 0x1c)->Destroy();
}

void __cdecl CleanupShadowStates01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    reinterpret_cast<ShadowStateVector01213a70*>(owner + 0x30)->Destroy();
}

void __cdecl CleanupShadowMaps01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    DestroyShadowRasterArray01213a70(owner + 0xf4, 4, 2,
        reinterpret_cast<void (__thiscall*)(ShadowRasterRef01213a70*)>(0x00576620));
}

void __cdecl CleanupShadowBasisMaps01213a70(char* parentFrame)
{
    char* owner = *reinterpret_cast<char**>(parentFrame - 0x10);
    DestroyShadowRasterArray01213a70(owner + 0x10c, 4, 8,
        reinterpret_cast<void (__thiscall*)(ShadowRasterRef01213a70*)>(0x00576620));
}

// The remaining retail entry invokes __CxxFrameHandler3 using FuncInfo
// 0x01501964. C++ must generate that dispatch from the enclosing destructor's
// exception lifetimes; a standalone cleanup cannot express its register ABI.
