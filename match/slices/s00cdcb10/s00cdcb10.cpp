// Slice s00cdcb10 -- cCityInputStrategy::TransitionToSpaceMode (retail 0x00cdcb10, dev 0x007f5630).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
//
// The player's mode object has three independent pending transitions, each armed by a one-shot
// byte at +0x28 / +0x29 (and a third, unarmed, fall-through path).  Every path hides the HUD for
// the mode's noun (id 0x4bf38a6) and then tears down the polymorphic space-transition object at
// +0x2f8 through its virtual releasing destructor (+0x08).  The first path refreshes a set of
// data objects (+0x290) and releases the temporary pointer array; the second marks the world
// (+0x364) and closes the space HUD; the third moves every unit of the player's selected list
// onto the city display.
//
// Stand-in types/offsets are named at their retail positions, matching the cMode layout already
// used by slice s00cde2f0 (this object is the same mode object; +0x28 -> sub-object +0x1c etc.).

#include "types.h"
#include <math.h>

#pragma warning(disable : 4100)

typedef unsigned char      u8;
typedef unsigned int       u32;
typedef signed int         i32;

// ---------------------------------------------------------------------------------------------
//  external callees
// ---------------------------------------------------------------------------------------------

void* FUN_00b3d280();
void* FUN_00b3d300();                                          // game-data manager
void* FUN_00b3d320();
void* FUN_00b3d380();                                          // noun manager
void* FUN_00b3d410();
void  FUN_00f47380(void* p);                                  // ??3 operator delete
void* FUN_00401090();                                          // species/editor manager

struct cNounMgr  { void FUN_00b32250(int id); };               // 0x00b32250
struct cDataMgr
{
    void* FUN_00b21340(int a, int b, int c, int d, int e);      // 0x00b21340 GetGameDataVector
    void* FUN_00bfc5f0();                                       // 0x00bfc5f0 GetPlayerTribe
    void* FUN_00f67d90();                                       // 0x00f67d90 GetCurrentTerrainSphere
};
struct cHudOwner  { void FUN_00e3e350(int a, int b, int c); };  // 0x00e3e350
struct cNounSlot  { void FUN_00b1e410(int id); };               // 0x00b1e410
struct cSub290    { void FUN_00ce0a20(); };                    // 0x00ce0a20
struct cLandmark  { void FUN_004dfb50(void* key, void* pos); };// 0x004dfb50
struct cSphere    { void FUN_00c79f00(int id, u32 value); };   // 0x00c79f00
struct cIconMgr   { void FUN_00cca880(void* unit, u32 value); };// 0x00cca880
struct cHandle
{
    int  FUN_00bca0c0(int a, int b, void* unit, int c);         // 0x00bca0c0
    void FUN_00bca040(int id);                                  // 0x00bca040
};

// ---------------------------------------------------------------------------------------------
//  element types and vtable views
// ---------------------------------------------------------------------------------------------

struct cRange { u32* mpBegin; u32* mpEnd; };

// A ref-counted object released through virtual slot +0x04; the first transition also talks to
// the sub-object at +0x290.
struct cRefObj
{
    virtual void v00() {}
    virtual void Release() {}                                   // +0x04
    u8  pad04[0x290 - 0x04];
    void* mp290;                                                // +0x290
};

// Temporary pointer array built by 0x00ba95a0 from a game-data row.
struct cPtrVec
{
    cRefObj** mpBegin;                                          // +0x00
    cRefObj** mpEnd;                                            // +0x04

    void Init(void* src);                                       // 0x00ba95a0
};

struct Vec3 { float x, y, z; };

// Polymorphic sub-object at cUnit+0xc0; slot +0x40 receives the unit's heading offset.
struct cUnitSub
{
    virtual void s00() {}
    virtual void s04() {}
    virtual void s08() {}
    virtual void s0c() {}
    virtual void s10() {}
    virtual void s14() {}
    virtual void s18() {}
    virtual void s1c() {}
    virtual void s20() {}
    virtual void s24() {}
    virtual void s28() {}
    virtual void s2c() {}
    virtual void s30() {}
    virtual void s34() {}
    virtual void s38() {}
    virtual void s3c() {}
    virtual void s40(float value) {}
};

// Polymorphic sub-object at cUnit+0x120; slot +0x2c yields the object's world position.
struct cUnitAxis
{
    virtual void s00() {}
    virtual void s04() {}
    virtual void s08() {}
    virtual void s0c() {}
    virtual void s10() {}
    virtual void s14() {}
    virtual void s18() {}
    virtual void s1c() {}
    virtual void s20() {}
    virtual void s24() {}
    virtual void s28() {}
    virtual Vec3* s2c() { return 0; }
};

// Player-side and unit-side view of the objects returned by GetPlayerTribe() and by the
// player's unit-range accessor (slot +0x90).
struct cUnit
{
    virtual void v00() {}
    virtual void v04() {}
    virtual void v08() {}
    virtual void v0c() {}
    virtual void v10() {}
    virtual void v14() {}
    virtual void v18() {}
    virtual void v1c() {}
    virtual void v20() {}
    virtual void v24() {}
    virtual void v28() {}
    virtual void v2c() {}
    virtual void v30() {}
    virtual void v34() {}
    virtual void v38() {}
    virtual void v3c() {}
    virtual void v40() {}
    virtual void v44() {}
    virtual void v48() {}
    virtual void v4c() {}
    virtual void v50() {}
    virtual void v54() {}
    virtual void v58() {}
    virtual void v5c() {}
    virtual void v60() {}
    virtual void v64() {}
    virtual void v68() {}
    virtual void v6c() {}
    virtual void v70() {}
    virtual void v74() {}
    virtual void v78() {}
    virtual void v7c() {}
    virtual float v80(int which) { return 0.0f; }               // +0x80
    virtual void v84() {}
    virtual void v88() {}
    virtual void v8c() {}
    virtual cRange* v90() { return 0; }                         // +0x90

    u8  pad04[0xc0 - 0x04];
    cUnitSub mHeading;                                          // +0xc0
    u8  padc4[0x120 - 0xc4];
    cUnitAxis mAxis;                                            // +0x120
    u8  pad124[0xb24 - 0x124];
    u32 m_b24;                                                  // +0xb24
    u8  padb28[0xb48 - 0xb28];
    void* mpb48;                                                // +0xb48

    void* FUN_00c8e820(int a);                                  // 0x00c8e820
    void  FUN_00c99550(float distance);                         // 0x00c99550
    void  FUN_00c21bf0(int a, int b);                           // 0x00c21bf0
    void  FUN_00c0bbe0();                                       // 0x00c0bbe0
    u32   FUN_00c22dc0();                                       // 0x00c22dc0
    u32   FUN_00c22820(u32 a);                                  // 0x00c22820 (one int arg)
    u32   FUN_00c0f9b0(u32 a);                                  // 0x00c0f9b0
    void  FUN_00c14070(u32 a);                                  // 0x00c14070
};

// Polymorphic helper at cCityInputStrategy+0x2f8; the mode owns it and releases it through
// virtual slot +0x08.
struct cSpaceTarget
{
    virtual void v00() {}
    virtual void v04() {}
    virtual void v08() {}                                       // +0x08 releasing destructor
    u8  pad04[0x18 - 0x04];
    u32 m18;                                                    // +0x18
    u8  pad1c[0x34 - 0x1c];
    u32 m34;                                                    // +0x34
};

struct cCityLandmark { void FUN_00ccef10(int id); };            // 0x00ccef10

// ---------------------------------------------------------------------------------------------
//  the mode object
// ---------------------------------------------------------------------------------------------

struct cCityInputStrategy
{
    u8  pad00[0x28];
    u8  m28;                                                    // +0x028 first pending transition
    u8  m29;                                                    // +0x029 second pending transition
    u8  pad2a[0x23c - 0x2a];
    cCityLandmark* mp23c;                                       // +0x23c
    cIconMgr*      mp240;                                       // +0x240
    u8  pad244[0x2f8 - 0x244];
    cSpaceTarget*  mp2f8;                                       // +0x2f8
    u8  pad2fc[0x38c - 0x2fc];
    i32 m38c;                                                   // +0x38c
    u8  pad390[0x3a1 - 0x390];
    u8  m3a1;                                                   // +0x3a1

    void FUN_00cd43e0(void* target);                            // 0x00cd43e0
    void FUN_00cdcb10();
};

// @ 0x00cdcb10
void cCityInputStrategy::FUN_00cdcb10()
{
    // --- first pending transition: refresh the space-mode data objects ------------------------
    if (m28 == 1)
    {
        m28 = 0;
        cNounMgr* nouns = (cNounMgr*)FUN_00b3d380();
        nouns->FUN_00b32250(0x4bf38a6);

        if (!mp2f8)
            return;

        void* row = ((cDataMgr*)FUN_00b3d300())->FUN_00b21340(
                        0xcd7d10, 0xd3d420, 0xaccbb0, 0xb1e500, 0x18c6d19);

        cPtrVec vec;
        vec.Init((u8*)row + 4);

        cRefObj** begin = vec.mpBegin;
        cRefObj** end   = vec.mpEnd;
        cRefObj** it;

        for (it = begin; it != end; ++it)
        {
            cRefObj* obj = *it;
            if (obj->mp290)
                ((cSub290*)obj->mp290)->FUN_00ce0a20();
        }

        FUN_00cd43e0(&mp2f8->m18);

        cSpaceTarget* target = mp2f8;
        if (target)
        {
            mp2f8 = 0;
            target->v08();
        }

        for (it = begin; it < end; ++it)
        {
            if (*it)
                (*it)->Release();
        }

        if (begin && ((u32*)begin)[-1] != 0)
            FUN_00f47380(begin);

        return;
    }

    // --- second pending transition: leave the space HUD ---------------------------------------
    if (m29 == 1)
    {
        m29 = 0;
        cNounMgr* nouns = (cNounMgr*)FUN_00b3d380();
        nouns->FUN_00b32250(0x4bf38a6);

        if (!mp2f8)
            return;

        u8* world = (u8*)FUN_00b3d280();
        if (world)
            world[0x364] = 1;

        if (m3a1 == 0)
        {
            ((cHudOwner*)FUN_00b3d410())->FUN_00e3e350(1, 0x1654c04, 1);
            ((cNounSlot*)FUN_00b3d320())->FUN_00b1e410(0x1654c04);
        }

        cSpaceTarget* target = mp2f8;
        if (target)
        {
            mp2f8 = 0;
            target->v08();
        }
        return;
    }

    // --- third transition: hand every selected unit to the city display -----------------------
    mp23c->FUN_00ccef10(m38c);

    bool hadTarget = mp2f8 != 0;
    m38c = -1;
    if (!hadTarget)
        return;

    cUnit* player = (cUnit*)((cDataMgr*)FUN_00b3d300())->FUN_00bfc5f0();
    void*  key    = player->FUN_00c8e820(0);
    ((cLandmark*)FUN_00401090())->FUN_004dfb50(key, &mp2f8->m18);

    Vec3*  pos     = player->mAxis.s2c();
    float  radius  = sqrtf(pos->x * pos->x + pos->y * pos->y + pos->z * pos->z);
    player->FUN_00c99550(radius);

    cSphere* sphere = (cSphere*)((cDataMgr*)FUN_00b3d300())->FUN_00f67d90();
    sphere->FUN_00c79f00(0x55d6335, mp2f8->m34);

    cRange* range = player->v90();
    u32* it = range->mpBegin;
    if (it != range->mpEnd)
    {
        do
        {
            cUnit* unit = (cUnit*)*it;

            unit->m_b24 = *(u32*)((u8*)key + 0x53c);

            unit->mHeading.s40(unit->v80(1));

            unit->FUN_00c21bf0(1, 1);
            unit->FUN_00c0bbe0();
            cIconMgr* icons = mp240;
            u32 value = unit->FUN_00c22dc0();
            icons->FUN_00cca880(unit, value);

            int id = ((cHandle*)unit->mpb48)->FUN_00bca0c0(5, 0, unit, 0);
            if (id != -1)
                ((cHandle*)unit->mpb48)->FUN_00bca040(id);

            u32 a = unit->FUN_00c22820(0);
            u32 b = unit->FUN_00c0f9b0(a);
            unit->FUN_00c14070(b);

            ++it;
        } while (it != range->mpEnd);
    }

    cSpaceTarget* target = mp2f8;
    if (target)
    {
        mp2f8 = 0;
        target->v08();
    }
}
