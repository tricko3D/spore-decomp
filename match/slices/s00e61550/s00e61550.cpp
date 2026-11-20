// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE
// Slice s00e61550.
//
// 0x00e61550 is the factory for the cell-stage "ability" object (236 bytes). The constructor is
// inlined into the factory (retail has no separate ctor body), so the two vtable stores of the
// most-derived class, the reference-counted base's vtable/field, the plain field stores and the
// three sub-object initialisers all appear inside the factory.
#include "types.h"

// ---- external data ----
struct cCellGame {
    char pad0[0x51da];
    char field_51da;                 // +0x51da
};
extern cCellGame* gspCellGame;       // 0x016b3c04

// ---- external callees ----
void* __cdecl operator new(unsigned, const char*, int, int, const char*, int); // 0x00f473a0
void  __cdecl f91ff50(void*);        // 0x0091ff50
void  __cdecl f4fcd0(void*);         // 0x00e4fcd0
void* __cdecl memset_stub(void*, int, unsigned); // 0x011e073e

// ---- the ability interface (primary base, +0x00) ----
struct IAbility {
    virtual void OnCreate();
};

// ---- the reference-counted base (second base subobject, +0x04; refcount at +0x08) ----
struct GmeRefCount {
    virtual int AddRef();
    virtual int Release();
    int mRefCount;
    GmeRefCount() : mRefCount(0) {}
};

struct CellAbility : IAbility, GmeRefCount {
    virtual void Update();           // engine slot owned by the most-derived class
    int  f0c;                        // +0x0c, set by the caller after construction
    int  f10, f14, f18;              // +0x10
    int  f1c, f20, f24, f28;         // +0x1c
    int  f2c;                        // +0x2c
    int  f30;                        // +0x30
    char o34[0x68 - 0x34];           // +0x34
    char b68, b69;                   // +0x68
    char pad6a[2];
    int  f6c, f70, f74;              // +0x6c
    char o78[0x7c - 0x78];           // +0x78
    int  f7c;                        // +0x7c
    char buf80[0x60];                // +0x80
    char b_e0, b_e1, b_e2;           // +0xe0
    char pad_e3;
    int  f_e4;                       // +0xe4
    char b_e8, b_e9;                 // +0xe8
    char pad_ea[0xec - 0xea];

    __forceinline CellAbility()
    {
        f10 = 0;
        f7c = 1;
        b_e2 = 1;
        f14 = 0;
        f18 = 0;
        b68 = 0;
        f1c = 0;
        f20 = 0;
        f24 = 0;
        f28 = 0;
        b69 = 0;
        f70 = 0;
        f6c = 0;
        f_e4 = 0;
        b_e8 = 0;
        f74 = 0;
        b_e0 = 0;
        b_e1 = 0;
        f30 = 10;
        f2c = 0;
        f91ff50(o78);
        f4fcd0(o34);
        memset_stub(buf80, 0, 0x60);
        gspCellGame->field_51da = 0;
        b_e9 = 0;
    }
};

// @ 0x00e61550
__declspec(noinline) CellAbility* FUN_00e61550()
{
    return new ("Simulator", 0, 0, 0, 0) CellAbility();
}
