// Slice s01058790 -- 0x01058790  SP::cSPDefaultToggleTool::ActivateTool (446 bytes).
//
// Toggle tool activation. After the base cSPToolStrategy::ActivateTool gate and the
// space-game / universe-context guards, it creates a noun of type 0x244fb08 through
// the noun manager, casts it to the territory interface (0x2450023), marks it active,
// sets its bounds flag and scale, hands its bounds sub-object to the terrain cursor
// and finally copies the tool's key-instance property list (property 0x4e3eca1) into
// a fresh property list published to the model world (property 0x49b48ee).
//
// Flags: /O2 /MD /Gy /TP
//
// Residual (checker, pass 4: 446 bytes = retail, 150 = 150 instructions, same
// calls/stores/branches):
//  - callee-saved role swap: retail keeps the shared null constant in ebx and
//    the property manager's vtable in ebp; this build has the opposite
//    (constant ebp, vtable ebx), which also swaps the push/pop order
//    (`push ebp;push esi` vs retail `push ebx;push esi`) and cascades into two
//    scheduler differences (pm vtable/argument load order and the Variant
//    zero-init register). Verified levers: the compound guard + trailing
//    `return false` place the failure island at the end, the noun ternary
//    places the `obj = 0` island at the end, `nToolId` must stay unspilled so
//    it is re-read from [esp+4], and the explicit pmVtbl local keeps the
//    property manager's vtable across the tool call (retail: ebp).
#include "types.h"

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

#define VSLOT(o, off) ((*(void***)(o))[(off) / 4])

typedef void  (__thiscall* Vfn0)(void*);
typedef void  (__thiscall* Vfn1)(void*, int);
typedef void  (__thiscall* VfnV)(void*, void*);
typedef void  (__thiscall* VfnVi)(void*, void*, int);
typedef void  (__thiscall* VfnF)(void*, float);
typedef void* (__thiscall* VfnP)(void*);
typedef void* (__thiscall* VfnPI)(void*, u32);
typedef void* (__thiscall* VfnPU)(void*, u32, void**);

extern const float g_fTen;                     // 0x01473c70  10.0f

// ---- engine entry points -------------------------------------------------
void* __cdecl SP_SpaceGameGet();               // 0x01002bd0
void* __cdecl SP_GetUniverseContext();         // 0x01021080
void* __cdecl SP_NounManager();                // 0x00b3d300
void* __cdecl SP_GetGameTerrainCursor();       // 0x00b30d70
void* __cdecl SP_GonzagoModelWorld();          // 0x00b3d520
void* __cdecl PropertyManager();               // 0x0067de30

bool __cdecl GetPropertyAsKeyInstance(void* list, u32 key, u32* out);  // 0x006a12a0

struct NounMgr {
    void* CreateNoun(u32 nounType);            // 0x00b20c60 (thiscall, pops its argument)
};

// ---- noun / territory objects (same view as cSPPlaceColonyToolStrategy) ---
struct Sub34 {
    void** mpVtbl;
};

struct Obj {
    void** mpVtbl;                             // +0x00
    u8 pad04[0x30];
    Sub34 mSub34;                              // +0x34
    u8 pad38[0xa6 - 0x38];
    u8 mByteA6;                                // +0xa6

    void Update();                             // 0x00bebb10
};

// ---- property system ------------------------------------------------------
struct Key {
    u32 mInstance;
    u32 mGroup;
    u32 mType;
};

struct Variant {
    u32 m00, m04, m08, m0C;                    // +0x00
    u16 mFlags;                                // +0x10
    u16 mTypeId;                               // +0x12

    Variant() : mFlags(0), mTypeId(0) {}
    ~Variant() {
        if (mFlags & 4)
            Destruct(0);
    }

    void Set(int type, int a, const void* src, int n, int m);  // 0x0093dd80
    void Destruct(int a);                                      // 0x0093db80
};

struct cPropertyList {
    // 0x006a11b0: resource base (vptr, refcount, key) + embedded vector_map.
    u32 m00, m04, m08, m0c, m10, m14, m18, m1c;
    u32 m20, m24, m28, m2c, m30, m34;

    cPropertyList();                           // 0x006a1c40
    ~cPropertyList();                          // 0x00686320
    void SetProperty(u32 key, Variant* value); // 0x006a2e20
};

// Reference-counted temporary for the tool's property list.
struct Ref {
    void* p;

    Ref() : p(0) {}

    void reset() {
        void* o = p;
        if (o) {
            p = 0;
            ((Vfn0)VSLOT(o, 4))(o);
        }
    }
};

// ---- tool strategy --------------------------------------------------------
class cSPToolStrategy {
public:
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();

    bool ActivateTool(int nToolId);            // 0x010537e0
};

class cSPDefaultToggleTool : public cSPToolStrategy {
public:
    bool ActivateTool(int nToolId);            // 0x01058790
};

// @ 0x01058790
bool cSPDefaultToggleTool::ActivateTool(int nToolId)
{
    if (cSPToolStrategy::ActivateTool(nToolId) &&
        SP_SpaceGameGet() &&
        SP_GetUniverseContext() == 0)
    {
        Obj* noun = (Obj*)((NounMgr*)SP_NounManager())->CreateNoun(0x244fb08u);
        Obj* obj = noun ? (Obj*)((VfnPI)VSLOT(noun, 0x0c))(noun, 0x2450023u) : 0;
        if (obj)
            ((Vfn0)VSLOT(obj, 0x00))(obj);

        Sub34* sub = &obj->mSub34;
        obj->mByteA6 = 1;
        ((Vfn1)VSLOT(sub, 0x54))(sub, 1);
        ((VfnF)VSLOT(obj, 0x60))(obj, g_fTen);
        obj->Update();

        Obj* cursor = (Obj*)SP_GetGameTerrainCursor();
        ((VfnV)VSLOT(cursor, 0xd0))(cursor, sub);

        Ref propList;
        void* pm = PropertyManager();
        propList.reset();

        // The property manager's vtable stays in a callee-saved register across
        // the tool call (retail: ebp).
        void** pmVtbl = *(void***)pm;
        void* list = ((VfnPU)VSLOT((void*)nToolId, 0x1c))((void*)nToolId, 0x30608f0bu, &propList.p);
        ((VfnV)pmVtbl[0x2c / 4])(pm, list);

        u32 keyInstance = 0;
        GetPropertyAsKeyInstance(propList.p, 0x4e3eca1u, &keyInstance);

        if (keyInstance) {
            Key key;
            key.mInstance = keyInstance;
            key.mGroup = 0;
            key.mType = 0;

            cPropertyList props;
            Variant var;

            var.Set(0x20, 0x98, &key, 0xc, 1);
            props.SetProperty(0x49b48eeu, &var);

            void* world = SP_GonzagoModelWorld();
            ((VfnVi)VSLOT(world, 0x18))(world, &props, 4);
        }

        if (propList.p)
            ((Vfn0)VSLOT(propList.p, 4))(propList.p);

        ((Vfn0)VSLOT(obj, 4))(obj);
        return true;
    }
    return false;
}
