// Slice s00ca6630: gameplay/object helpers.
//
// Targets:
//   00ca6db0  update a cVehicle-like object's combat/status flags
//   00ca6f20  lazily initialised global object list A
//   00ca6f70  block-list pop-front
//   00ca6ff0  cVehicle::UpdateCombatStatus (timed messages / UI effects)
//   00ca71d0  lazily initialised global object list B
//   00ca7250  assign a C string into an eastl::basic_string at +0xd14
//   00ca72a0  cPathAndTurnLocomotion::Execute
//   00ca7520  model-key / profile change handler
//
// Global tables and singleton cells live at fixed image addresses; naming them
// with macros keeps the source readable while preserving absolute addressing.
#include <stddef.h>

// ---------------------------------------------------------------------------
// External routines.  Member declarations are used for __thiscall targets so
// that the compiler emits an `ecx = this` direct call, exactly as the original
// does; the linker resolves the callee by its mangled name.
// ---------------------------------------------------------------------------
struct ExtObj {
    void __thiscall ClearCreature();                    // 0x00ca0430
    void __thiscall SetModelKey(void* key);             // 0x00c88810
    void __thiscall ResetModel();                       // 0x00ca42c0
    void __thiscall ResetCreature();                    // 0x00ca3f20
    void* __thiscall GetProfile(void* key);             // 0x004df550
    void __thiscall SetByte(unsigned char value);       // 0x00bfc470
    void __thiscall SetFlags(int, int, float, int);     // 0x00bc97f0
    void* __thiscall Get74(void* self);                 // 0x00f3f950
    void* __thiscall GetData();                         // 0x00b18530
    char __thiscall Test(int offset);                   // 0x00f25080
    void __thiscall SetA(void* value);                  // 0x00ca33b0
    void __thiscall KillEffect(void* value);            // not used directly
};

struct EastlString {
    void __thiscall Assign(const char* first, const char* last); // 0x00454cb0
};

extern "C" int __cdecl atexit(void (__cdecl*)(void));
void __cdecl FUN_013c4400(void);
void __cdecl FUN_013c4420(void);

void __cdecl FUN_00f47380(void*);                       // operator delete

void* __cdecl FUN_00401090(void);                       // editor species manager
void* __cdecl FUN_00a206f0(void);                       // EA::Audio::GetSystemAT
void* __cdecl FUN_00b5b800(void);                       // SP::GetCurrentGameMode
unsigned __cdecl FUN_00435e90(void);                    // GetRecorderState
void __cdecl FUN_00435ed0(void* space, unsigned value); // SP::cSPUISpace::KillSetiEffects
void __cdecl FUN_00421cf0(void* message);               // SlotMessage::Destruct
void __cdecl FUN_00d9a290(int a, void* b, void* c, void* d);
void* __cdecl FUN_00883860(void);                        // EA::Messaging::GetServer
char __cdecl FUN_00c9e8e0(int a, void* b);

struct Vec3 { float x, y, z; };

// ---------------------------------------------------------------------------
// SP::cActionCircle -- 0x64-byte action circle: the slot vector lives at +0x50
// and every slot is 0x18 bytes with its world position at +0xc.
// ---------------------------------------------------------------------------
struct cActionCircle {
    struct tSlot {
        bool mOccupied;              // +0x00
        char _pad0[3];
        void* mpReservingNoun;       // +0x04
        unsigned int mReservingId;   // +0x08
        Vec3 mPos;                   // +0x0c
    };
    struct SlotVector {
        tSlot* mpBegin;              // +0x00
        tSlot* mpEnd;                // +0x04
        tSlot* mpCapacity;           // +0x08
        int mAllocator[2];           // +0x0c
    };

    char pad00[0x50];
    SlotVector mSlots;               // +0x50

    void __thiscall Construct();                                     // 0xafba10
    void __thiscall SetAtCenter(const Vec3* center, float radius,
                                const Vec3* facing, float altitude); // 0xaf9cf0
    void __thiscall SetSlotRadius(int index, float radius);          // 0xafef10
    void __thiscall Destroy();                                       // 0xafbad0
};

struct NounManager {
    void* __thiscall FindNoun(unsigned int id);                      // 0xb25f40
};
struct NounContainer {
    void* mpBegin;                                                   // +0x00
    void* mpEnd;                                                     // +0x04
};
struct Noun {
    NounContainer* __thiscall GetContainer();                        // 0xbef6c0
};
struct cCity {
    Vec3* __thiscall GetVehicleSpawnPoint(Vec3* out, int kind);      // 0xbdca30
};

NounManager* __cdecl SP_NounManager();                               // 0xb3d300
void* __cdecl FUN_00ac9310(NounContainer* c, Vec3* pos, bool flag);  // 0xac9310

extern float g_01699b08;        // 0x01699b08
extern float g_01699b0c;        // 0x01699b0c
extern float g_01699b10;        // 0x01699b10
extern float g_1400a58;         // 0x01400a58

struct Timer {
    unsigned __int64 __thiscall GetElapsedTime();        // 0x00bc3190
    void __thiscall Restart();                           // 0x00bc3130
};

struct PlanetModel {
    unsigned int __thiscall GetAverageRadius(Vec3* p);   // 0x00b7e840
};
PlanetModel* __cdecl GetPlanetModel();                   // 0x00b3d350

// ---------------------------------------------------------------------------
// Global cells.
// ---------------------------------------------------------------------------
extern unsigned int g_list_a_flags;     // 0x01699adc
extern int g_list_a_data[3];            // 0x01699ac8
extern int g_list_a_alt;                // 0x0169a104

extern unsigned int g_list_b_flags;     // 0x01699af8
extern void* g_list_b_cell;             // 0x01699ae0
extern void* g_list_b_zero;             // 0x01699af0
extern void* g_list_b_result;           // 0x01699af4

#define G_GAME_DATA      (*(int*)0x016c7aa4)

// ---------------------------------------------------------------------------
// 00ca6f20
// ---------------------------------------------------------------------------
// __fastcall, ecx = this.  Returns one of two singleton list heads.  The first
// use of the primary list registers its atexit destructor exactly once.
// @ 0x00ca6f20
int* __fastcall FUN_00ca6f20(char* self)
{
    int zero = 0;

    if (*(int*)(self + 0x550) != zero) {
        int one = 1;
        if ((g_list_a_flags & one) == 0) {
            g_list_a_flags |= one;
            g_list_a_data[0] = zero;
            g_list_a_data[1] = zero;
            g_list_a_data[2] = zero;
            atexit(FUN_013c4400);
        }
        return g_list_a_data;
    }
    return &g_list_a_alt;
}

// ---------------------------------------------------------------------------
// 00ca6f70
// ---------------------------------------------------------------------------
// A simple block list: mpCur walks a block of 0x100 dwords; when it reaches the
// end the next block header is read from mpBlockPtr and the old block deleted.
struct BlockList {
    char pad00[0xb34];
    unsigned* mpCur;        // +0xb34
    int* mpBlock;           // +0xb38
    unsigned* mpEnd;        // +0xb3c
    int* mpBlockPtr;        // +0xb40
    unsigned* mpLimit;      // +0xb44
};

// @ 0x00ca6f70
unsigned __fastcall FUN_00ca6f70(BlockList* self)
{
    unsigned* cur = self->mpCur;
    if (cur == self->mpLimit)
        return 0;

    unsigned value = *cur;
    if (cur + 1 != self->mpEnd) {
        self->mpCur = cur + 1;
        return value;
    }

    if (self->mpBlock != 0)
        FUN_00f47380(self->mpBlock);

    int* next = self->mpBlockPtr + 1;
    self->mpBlockPtr = next;
    int block = *next;
    self->mpBlock = (int*)block;
    self->mpEnd = (unsigned*)(block + 0x100);
    self->mpCur = (unsigned*)block;
    return value;
}

// ---------------------------------------------------------------------------
// 00ca71d0
// ---------------------------------------------------------------------------
// Same lazy-destructor pattern as 00ca6f20, then a small container check.
struct ObjectAt508 {
    void** mpVtbl;          // +0x00
};

struct Object71d0 {
    char pad00[0x508];
    ObjectAt508 mObject;    // +0x508
    char pad50c[0xb68 - 0x50c];
    void* mpCur;            // +0xb68
    void* mpEnd;            // +0xb6c
};

// @ 0x00ca71d0
void* __fastcall FUN_00ca71d0(Object71d0* self)
{
    int one = 1;
    if ((g_list_b_flags & one) == 0) {
        g_list_b_flags |= one;
        g_list_b_cell = 0;
        atexit(FUN_013c4420);
    }

    void* cur = self->mpCur;
    if (cur != self->mpEnd)
        return cur;

    void* old = g_list_b_cell;
    if (old != 0) {
        g_list_b_cell = 0;
        void** vtbl = *(void***)old;
        ((void (__thiscall*)(void*))vtbl[1])(old);
    }

    g_list_b_zero = 0;
    void* result = ((void* (__thiscall*)(void*))
                    (*(void***)((char*)self + 0x508))[4])((char*)self + 0x508);
    g_list_b_result = result;
    return &g_list_b_cell;
}

// ---------------------------------------------------------------------------
// 00ca7250
// ---------------------------------------------------------------------------
// __thiscall, one stack argument (ret 4).  Assigns either the empty string or
// the [param, param + strlen(param)) range into the eastl string at +0xd14.
struct StringOwner {
    char pad00[0xd14];
    char mString;           // +0xd14
    void __thiscall FUN_00ca7250(char* text);
};

// @ 0x00ca7250
void StringOwner::FUN_00ca7250(char* text)
{
    if (text == 0) {
        ((EastlString*)&mString)->Assign((const char*)0x013ec47c,
                                         (const char*)0x013ec47c);
        return;
    }

    char* next = text;
    char* end;
    do {
        end = next;
        next = end + 1;
    } while (*end != 0);

    ((EastlString*)&mString)->Assign(text, end);
}

// ---------------------------------------------------------------------------
// 00ca7520
// ---------------------------------------------------------------------------
// __thiscall, one stack argument (ret 4).  The magic in key->mMagic selects
// between clearing the model key and looking a species profile up.
struct ModelKey {
    char pad00[4];
    unsigned mMagic;        // +0x04
};

struct SpatialObject {
    char pad00[0xcd0];
    void* mpProfile;        // +0xcd0
    void __thiscall FUN_00ca7520(ModelKey* key);
};

struct EnclosingObject {
    char pad00[0x34];
    SpatialObject mSpatial; // +0x34
};

// @ 0x00ca7520
void SpatialObject::FUN_00ca7520(ModelKey* key)
{
    switch (key->mMagic) {
    case 0x24682294:
    case 0x476a98c7:
        ((ExtObj*)((char*)this - 0x34))->ClearCreature();
        mpProfile = 0;
        ((ExtObj*)this)->SetModelKey(key);
        return;

    case 0x2b978c46: {
        void* profile = ((ExtObj*)FUN_00401090())->GetProfile(key);
        mpProfile = profile;
        if (profile == 0)
            return;
        SpatialObject* base = (SpatialObject*)((char*)this - 0x34);
        ((ExtObj*)base)->ClearCreature();
        ((ExtObj*)base)->ResetModel();
        ((ExtObj*)base)->ResetCreature();
        return;
    }
    }
}

// ---------------------------------------------------------------------------
// 00ca6db0
// ---------------------------------------------------------------------------
// Per-frame vehicle/status update: notify the HUD, probe the object's control
// mode, select the bit in the shared flag word, then run the post-update hook.
struct Self6db0 {
    char pad00[0x18];
    void* mp18;             // +0x18
    char pad1c[0x34 - 0x1c];
    ObjectAt508 mSub34;     // +0x34
    char pad38[0x2a0 - 0x38];
    float m2a0;             // +0x2a0
    char pad2a4[0xaf0 - 0x2a4];
    int* mpAf0;             // +0xaf0
    char padAf4[0xafc - 0xaf4];
    unsigned char mAfC;     // +0xafc
    char padAfD[0xb00 - 0xafd];
    void* mpB00;            // +0xb00
};

// @ 0x00ca6db0
void __fastcall FUN_00ca6db0(Self6db0* self)
{
    int* game = (int*)G_GAME_DATA;
    int* settings = *(int**)((char*)game + 0x78);
    int count = *(int*)((char*)settings + 0xb8);

    ObjectAt508* sub = &self->mSub34;
    void* result = ((void* (__thiscall*)(ObjectAt508*))sub->mpVtbl[0x2c / 4])(sub);
    FUN_00d9a290(count, self->mp18, result, self->mpB00);

    char* status = (char*)((ExtObj*)(*(int**)((char*)game + 0x74)))->Get74(self);
    ((ExtObj*)((char*)self + 0x508))->SetByte((unsigned char)status[1]);

    count = *(int*)((char*)(*(int**)((char*)game + 0x78)) + 0xb8);
    if (count > 0) {
        char* data = (char*)((ExtObj*)self)->GetData();
        if (((ExtObj*)status)->Test(count * 0x4e0 - 0x4e0 + *(int*)(data + 0x70)))
            ((ExtObj*)((char*)self->mpAf0 + 8))->SetFlags(1, 0,
                                                         *(float*)0x014749a8, 0);
    }

    unsigned* flags = (unsigned*)((char*)self->mpAf0 + 0x5fc);
    *flags &= 0xffff83ff;
    switch (*(int*)(status + 0x4ac)) {
    case 1: *flags |= 0x2000; break;
    case 2: *flags |= 0x0800; break;
    case 3: *flags |= 0x1000; break;
    case 4: *flags |= 0x4000; break;
    case 5: *flags |= 0x0400; break;
    }

    ((void (__thiscall*)(ObjectAt508*))sub->mpVtbl[0xf4 / 4])(sub);
    self->m2a0 = *(float*)(status + 0x484);
    result = ((void* (__thiscall*)(ObjectAt508*))sub->mpVtbl[0x2c / 4])(sub);
    ((ExtObj*)self)->SetA(result);
    self->mAfC = 1;
}

// ---------------------------------------------------------------------------
// 00ca6ff0
// ---------------------------------------------------------------------------
// cVehicle::UpdateCombatStatus.  A one-second timer emits a HUD message; the
// current game mode then decides between two ten-second UI-effect timers.
struct MessageBasicRC5 {
    void* mpVtbl;           // +0x00
    volatile int mRefCount; // +0x04
    void* mData0;           // +0x08
    char pad0c[0x10 - 0x0c];
    unsigned mData1;        // +0x10
    char pad14[0x30 - 0x14];
    unsigned mId;           // +0x30
    char pad34[0x38 - 0x34];
    int mRCFlags;           // +0x38
    char pad3c[4];
};

struct Self6ff0 {
    char pad34[0x34];
    ObjectAt508 mSub34;     // +0x34
    char pad38[0xaf0 - 0x38];
    int* mpAf0;             // +0xaf0
    char padAf4[0xb20 - 0xaf4];
    int mB20;               // +0xb20
    char padB24[0xbcc - 0xb24];
    unsigned char mBcc;     // +0xbcc
    char padBcd[0xc88 - 0xbcd];
    Timer mTimer1;          // +0xc88
    char padC9c[0xca8 - 0xc9c];
    Timer mTimer2;          // +0xca8
    void __thiscall FUN_00ca6ff0(void* arg);
};

// @ 0x00ca6ff0
void Self6ff0::FUN_00ca6ff0(void* arg)
{
    if (mTimer1.GetElapsedTime() > 1000) {
        MessageBasicRC5 msg;
        msg.mpVtbl = (void*)0x013eb90c;
        msg.mRefCount = 0;
        msg.mpVtbl = (void*)0x013eb844;
        msg.mRCFlags = 0;
        msg.mId = 0x354b9e8;
        msg.mData0 = this;
        msg.mData1 = 0xffff0000;

        void* server = FUN_00883860();
        ((void (__thiscall*)(void*, unsigned, MessageBasicRC5*, int))
         (*(void***)server)[0x14 / 4])(server, msg.mId, &msg, 0);

        mTimer1.Restart();
        mBcc = 1;
        FUN_00421cf0(&msg);
    }

    if (FUN_00b5b800() == (void*)0x01654c10) {
        if ((*(unsigned*)((char*)mpAf0 + 0x5fc) & 0x3000) == 0)
            return;
        if (mTimer2.GetElapsedTime() <= 10000)
            return;

        mTimer2.Restart();
        unsigned audio = 0;
        void* sys = FUN_00a206f0();
        if (sys)
            audio = ((unsigned (__thiscall*)(void*))(*(void***)sys)[0x20 / 4])(sys);
        FUN_00435ed0(((void**)0x01474b10)[mB20], audio);
        return;
    }

    char active = ((char (__thiscall*)(ObjectAt508*))
                   mSub34.mpVtbl[0x58 / 4])(&mSub34);
    if (!active)
        return;
    if (mTimer2.GetElapsedTime() <= 60000)
        return;

    mTimer2.Restart();
    if (arg != 0) {
        void* obj = ((void* (__thiscall*)(void*, void*))
                     (*(void***)arg)[0x5c / 4])(arg, (void*)0x0137e8e0);
        if (obj != 0 && *(int*)((char*)obj + 0xb20) == 1) {
            unsigned state = FUN_00435e90();
            FUN_00435ed0(((void**)0x01474b04)[mB20], state);
            return;
        }
    }

    unsigned audio = 0;
    void* sys = FUN_00a206f0();
    if (sys)
        audio = ((unsigned (__thiscall*)(void*))(*(void***)sys)[0x20 / 4])(sys);
    FUN_00435ed0(((void**)0x01474af8)[mB20], audio);
}

// ---------------------------------------------------------------------------
// 00ca72a0
// ---------------------------------------------------------------------------
// cPathAndTurnLocomotion::Execute.  Samples the actor position, builds a
// zero-radius action circle around it, then walks the circle's reserved slots
// and tells the locomotion sub-object to head for the first slot whose world
// position lies in a region the current locomotion kind accepts.  When no slot
// qualifies it falls back to the city vehicle spawn point for the empire that
// owns the +0x508 object.
struct Self72a0 {
    char pad34[0x34];
    ObjectAt508 mSub34;     // +0x34
    char pad38[0x508 - 0x38];
    ObjectAt508 m508;       // +0x508
    char pad50c[0xb1c - 0x50c];
    int mB1c;               // +0xb1c
};

// @ 0x00ca72a0
void __fastcall FUN_00ca72a0(Self72a0* self)
{
    ObjectAt508* sub = &self->mSub34;
    float* p = ((float* (__thiscall*)(ObjectAt508*))
                self->mSub34.mpVtbl[0x2c / 4])(sub);
    Vec3 here = { p[0], p[1], p[2] };

    if (FUN_00c9e8e0(self->mB1c, &here))
        return;

    cActionCircle circle;
    circle.Construct();

    Vec3 scratch;
    circle.SetAtCenter(&here, g_1400a58,
        ((Vec3* (__thiscall*)(ObjectAt508*, Vec3*))
         sub->mpVtbl[0x5c / 4])(sub, &scratch),
        0.0f);

    float radius = ((float (__thiscall*)(ObjectAt508*))
                    sub->mpVtbl[0x70 / 4])(sub);
    circle.SetSlotRadius(0, radius);

    int count = (int)(circle.mSlots.mpEnd - circle.mSlots.mpBegin);
    for (int i = 0; i < count; ++i) {
        Vec3& sp = circle.mSlots.mpBegin[i].mPos;
        scratch.x = sp.x;
        scratch.y = sp.y;
        scratch.z = sp.z;
        int kind = self->mB1c;
        unsigned int mask = GetPlanetModel()->GetAverageRadius(&scratch);
        bool ok = true;
        switch (kind) {
        case 0: ok = (mask & 0x98000000) == 0; break;
        case 1: ok = (mask & 0x58000000) == 0; break;
        case 2: ok = ((mask >> 29) & 1) == 0; break;
        default: break;
        }
        if (!ok)
            continue;
        ((void (__thiscall*)(ObjectAt508*, Vec3*))
         sub->mpVtbl[0x38 / 4])(sub, &scratch);
        circle.Destroy();
        return;
    }

    Noun* noun = (Noun*)SP_NounManager()->FindNoun(
        ((unsigned int (__thiscall*)(ObjectAt508*))
         self->m508.mpVtbl[0x10 / 4])(&self->m508));
    if (noun != 0) {
        NounContainer* container = noun->GetContainer();
        if (container->mpBegin != container->mpEnd) {
            bool flag = false;
            NounContainer* slots = noun->GetContainer();
            cCity* city = (cCity*)FUN_00ac9310(slots, &here, flag);
            here = *city->GetVehicleSpawnPoint(&scratch, self->mB1c);
            if (here.x != g_01699b08 ||
                here.y != g_01699b0c ||
                here.z != g_01699b10)
                ((void (__thiscall*)(ObjectAt508*, Vec3*))
                 sub->mpVtbl[0x38 / 4])(sub, &here);
        }
    }

    circle.Destroy();
}

#include "sse_helpers.h"     // reviewed fld/fistp 64-bit rounding (X87RoundToInt64)

// ---------------------------------------------------------------------------
// SP::cVehicle::Init -- 0x00ca6630, 1917 bytes, ret 0x14
//
// Init(locomotion, purpose, model): clears the behaviour-tree flag word at
// +0x5fc and restarts both combat timers; when the vehicle-type property list
// exists it samples the locomotion table for the turn rate (+0x218) and reads
// the default weapon cycle time; then it either loads the given model key or
// asks the object-template database for the first match of the
// locomotion/purpose pair (tables 0x014749ec / 0x01474a28), sets the purpose
// flag bits, creates the default tool, applies the stance bits and the
// player-owned bit, runs SetupFlags, attaches the hit sphere (all modes but
// 0x1654c10) and finally, in mode 0x1654c10, loads ten tunables from property
// list 0xf14790c7 plus a second attached object.
//
// Retail cVehicle layout used below:
//   +0x034 spatial sub-object (vtable: 0x2c position, 0x58 player-owned,
//          0x70 bounding radius, 0x94 load model)
//   +0x218 turn rate         +0x2a0 max speed
//   +0xaf0 behaviour-tree data (flags at +0x5fc)
//   +0xb1c locomotion        +0xb20 purpose        +0xb24 stance
//   +0xc7c default weapon cycle time (ms)
//   +0xc88 / +0xca8 combat timers
//   +0xcfc hit sphere
//   +0xd6c..+0xd88 integer tunables, +0xd8c/+0xd90 float tunables
//   +0xd94 extra attached object
// ---------------------------------------------------------------------------

struct VehVec {
    float x, y, z;
    explicit VehVec(float v) : x(v), y(v), z(v) {}
    VehVec(const VehVec& o) { x = o.x; y = o.y; z = o.z; }
};

struct VehKey { unsigned int instanceID, typeID, groupID; };

struct VehRefObj {
    virtual void AddRef();                              // +0
    virtual void Release();                             // +4
};

struct VehNoun : VehRefObj {
    virtual void s2();
    virtual VehRefObj* Cast(unsigned int id);           // +0xc
};

struct VehProperty {
    char pad[0x12];
    unsigned short type;                                // +0x12 (10 = uint, 13 = float)
    float* GetFloat();                                  // 0x0041ea70
    unsigned int* GetUInt();                            // 0x0041ea00
};

struct VehPropertyList {
    virtual void AddRef();
    virtual void Release();                             // +4
    virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5();
    virtual void v6(); virtual void v7(); virtual void v8();
    virtual bool GetProperty(unsigned int id, VehProperty*& prop);   // +0x24
};

struct VehPropertyMgr {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10();
    virtual bool GetPropertyList(unsigned int instanceID, VehPropertyList** ppList);                  // +0x30
    virtual bool GetPropertyList(unsigned int instanceID, unsigned int groupID,
                                 VehPropertyList** ppList);                                          // +0x2c
};

// EA::AutoRefCount<VehPropertyList> local: taking its address releases/clears.
struct VehPropRef {
    VehPropertyList* p;
    VehPropRef() : p(0) {}
    ~VehPropRef() { if (p) p->Release(); }
    VehPropertyList** Out() {
        if (p) { VehPropertyList* t = p; p = 0; t->Release(); }
        return &p;
    }
};

// Functional-match constraint: 0x24 bytes, child vector at +0x10.
namespace VehFM {
enum Sentinel { kEndConstraint = 0 };
enum EqualConstraint { kEquals = 0 };
struct Constraint {
    unsigned int mParameter, mType, mA, mB;
    Constraint* mpBegin;
    Constraint* mpEnd;
    Constraint* mpCapacity;
    const char* mpName;
    unsigned int mFlags;
    Constraint(Sentinel);                                           // 0x00558830
    Constraint(unsigned int param, EqualConstraint, int value);      // 0x00558960
    ~Constraint()                                                   // out-of-line copy at 0x006066f0
    {
        for (Constraint* c = mpBegin; c < mpEnd; ++c)
            c->~Constraint();
        if (mpBegin && ((int*)mpBegin)[-1])
            FUN_00f47380(mpBegin);
    }
};
}

struct VehKeyVec {                                     // eastl::vector<VehKey>
    VehKey* mpBegin;
    VehKey* mpEnd;
    VehKey* mpCapacity;
    VehKeyVec() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    ~VehKeyVec() { if (mpBegin && ((int*)mpBegin)[-1]) FUN_00f47380(mpBegin); }
};

struct VehTemplateDB {
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void s5(); virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9();
    virtual void __cdecl Find(VehKeyVec* out, int one, VehFM::Constraint a, VehFM::Constraint b);  // +0x28
    virtual void s11(); virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19(); virtual void s20();
    virtual void s21();
    virtual void Register(VehKey* key, int one);                                                   // +0x58
};

struct VehNounMgr {
    VehNoun* CreateNoun(unsigned int id);               // 0x00b20c60, ret 4
};

struct VehLocoTable {
    // loco, the three floats of the sample point, then three float results
    void Lookup(unsigned int loco, float x, float y, float z,
                float* c, float* b, float* a);                                  // 0x00ceec10, ret 0x1c
};

struct VehSub34 {                                       // spatial sub-object at vehicle+0x34
    virtual void s0(); virtual void s1(); virtual void s2(); virtual void s3(); virtual void s4();
    virtual void s5(); virtual void s6(); virtual void s7(); virtual void s8(); virtual void s9();
    virtual void s10();
    virtual VehVec* GetPosition();                      // +0x2c
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15(); virtual void s16();
    virtual void s17(); virtual void s18(); virtual void s19(); virtual void s20(); virtual void s21();
    virtual bool IsPlayerOwned();                       // +0x58
    virtual void s23(); virtual void s24(); virtual void s25(); virtual void s26(); virtual void s27();
    virtual float GetBoundingRadius();                  // +0x70
    virtual void s29(); virtual void s30(); virtual void s31(); virtual void s32(); virtual void s33();
    virtual void s34(); virtual void s35(); virtual void s36();
    virtual void LoadModel(VehKey* key);                // +0x94
};

struct VehHitSphere : VehRefObj {
    void SetPositionRadius(VehVec* pos, float radius);  // 0x00c3f160, ret 8
    void SetRange(float lo, float hi);                  // 0x00c3f1b0, ret 8
    void Attach(void* v);                               // 0x00c3f2d0, ret 4
};

struct VehTimer {
    unsigned int pad[8];
    void Restart();                                     // 0x00bc3130
};

struct VehBtData { char pad[0x5fc]; unsigned int flags; };

struct Vehicle {
    char pad0[0x34];
    VehSub34 sub34;                                     // +0x34
    char pad38[0x218 - 0x38];
    float f218;
    char pad21c[0x2a0 - 0x21c];
    float f2a0;
    char pad2a4[0xaf0 - 0x2a4];
    VehBtData* mpBehaviorTreeData;                      // +0xaf0
    char padaf4[0xb1c - 0xaf4];
    unsigned int mLocomotion;                           // +0xb1c
    unsigned int mPurpose;                              // +0xb20
    unsigned int mStance;                               // +0xb24
    char padb28[0xc7c - 0xb28];
    unsigned int mDefaultWeaponCycleTimeMS;             // +0xc7c
    char padc80[0xc88 - 0xc80];
    VehTimer mCombatStatusTimer;                        // +0xc88
    VehTimer mCombatVoxTimer;                           // +0xca8
    char padcc8[0xcfc - 0xcc8];
    VehHitSphere* mpHitSphere;                          // +0xcfc
    char padd00[0xd6c - 0xd00];
    unsigned int mTunable[8];                           // +0xd6c .. +0xd88
    float mTunableF[2];                                 // +0xd8c, +0xd90
    VehRefObj* mpExtra;                                 // +0xd94

    void CreateDefaultTool();                           // 0x00ca2ae0
    void SetupFlags();                                  // 0x00ca2e80

    __forceinline void SetStance(unsigned int stance)
    {
        mStance = stance;
        mpBehaviorTreeData->flags &= 0xfffffcff;
        switch (mStance) {
        case 0:
            mpBehaviorTreeData->flags |= 0x100;
            break;
        case 1:
            mpBehaviorTreeData->flags |= 0x200;
            break;
        }
    }

    void Init(unsigned int locomotion, unsigned int purpose, VehKey model);
};

extern unsigned int   VehGetCurrentGameMode();          // 0x00b5b800
extern VehNounMgr*    VehNounManager();                 // 0x00b3d300
extern VehPropertyMgr* VehGetPropertyManager();         // 0x0067de30
extern VehTemplateDB* VehObjectTemplateDB();            // 0x0067cb40
extern void           VehAttach(Vehicle* v);            // 0x00dc4b70 (cdecl)
extern VehLocoTable   gVehLocoTable;                    // 0x0169c9c8
extern unsigned int   gVehTemplateIDsByLoco[];          // 0x01474a28 (mode 0x1654c05)
extern unsigned int   gVehTemplateIDs[];                // 0x014749ec ([loco + 3 * purpose])
extern unsigned int   gVehTunablesGroup;                // 0x0157c904

static __forceinline void VehReadUInt(VehPropRef& l, unsigned int id, unsigned int& out)
{
    VehProperty* p;
    if (l.p && l.p->GetProperty(id, p) && p->type == 10)
        out = *p->GetUInt();
}
static __forceinline void VehReadFloat(VehPropRef& l, unsigned int id, float& out)
{
    VehProperty* p;
    if (l.p && l.p->GetProperty(id, p) && p->type == 13)
        out = *p->GetFloat();
}

// @ 0x00ca6630
void Vehicle::Init(unsigned int locomotion, unsigned int purpose, VehKey model)
{
    mLocomotion = locomotion;
    mPurpose = purpose;
    mpBehaviorTreeData->flags = 0;
    mCombatStatusTimer.Restart();
    mCombatVoxTimer.Restart();

    VehPropRef list;
    bool haveType = VehGetPropertyManager()->GetPropertyList(0xb6a5c63d, list.Out());
    float zero = 0.0f;
    if (haveType) {
        float a, b, c;
        gVehLocoTable.Lookup(mLocomotion, zero, zero, zero, &c, &b, &a);
        f218 = a;
        float cycle = 2.0f;
        if (list.p) {
            VehProperty* p;
            if (list.p->GetProperty(0x3a36e7c, p) && p->type == 13)
                cycle = *p->GetFloat();
        }
        mDefaultWeaponCycleTimeMS = (unsigned int)X87RoundToInt64(cycle * 1000.0f);
    }

    if (model.instanceID != 0xffffffff && model.instanceID != 0) {
        sub34.LoadModel(&model);
    } else {
        unsigned int loco = mLocomotion;
        unsigned int purp = mPurpose;
        unsigned int templateID;
        if (VehGetCurrentGameMode() == 0x1654c05)
            templateID = gVehTemplateIDsByLoco[loco];
        else
            templateID = gVehTemplateIDs[loco + purp * 3];
        VehKeyVec keys;
        VehObjectTemplateDB()->Find(&keys, 1, VehFM::Constraint(0x2dc9d1e, VehFM::kEquals, templateID),
                                    VehFM::Constraint(VehFM::kEndConstraint));
        if (keys.mpBegin != keys.mpEnd) {
            model = *keys.mpBegin;
            sub34.LoadModel(&model);
            VehObjectTemplateDB()->Register(&model, 1);
        }
    }

    if (mPurpose == 0)
        mpBehaviorTreeData->flags |= 1;
    else if (mPurpose == 1)
        mpBehaviorTreeData->flags |= 2;
    else if (mPurpose == 2)
        mpBehaviorTreeData->flags |= 4;
    else
        mpBehaviorTreeData->flags |= 8;

    CreateDefaultTool();
    f2a0 = 32.0f;
    SetStance(0);
    if (!sub34.IsPlayerOwned()) {
        mpBehaviorTreeData->flags |= 0x10;
        if (mPurpose == 2)
            SetStance(1);
    }
    SetupFlags();

    if (VehGetCurrentGameMode() != 0x1654c10) {
        VehNounMgr* nm = VehNounManager();
        VehNoun* n = nm->CreateNoun(0x2e72cae);
        VehHitSphere* hs = n ? (VehHitSphere*)n->Cast(0x2e71a5a) : 0;
        VehHitSphere* old = mpHitSphere;
        if (hs != old) {
            if (hs) hs->AddRef();
            mpHitSphere = hs;
            if (old) old->Release();
        }
        mpHitSphere->SetPositionRadius(sub34.GetPosition(), sub34.GetBoundingRadius());
        mpHitSphere->SetRange(0.0f, 3.402823466e+38f);
        mpHitSphere->Attach(this);
    }
    VehAttach(this);

    if (VehGetCurrentGameMode() == 0x1654c10) {
        VehPropRef tun;
        if (VehGetPropertyManager()->GetPropertyList(0xf14790c7, gVehTunablesGroup, tun.Out()) && tun.p) {
            VehReadUInt(tun, 0xbf5cb754, mTunable[0]);
            VehReadUInt(tun, 0x56a51680, mTunable[1]);
            VehReadUInt(tun, 0x28dc8e73, mTunable[2]);
            VehReadUInt(tun, 0xdc7e88d7, mTunable[3]);
            VehReadUInt(tun, 0x302f343d, mTunable[4]);
            VehReadUInt(tun, 0x54c2dd99, mTunable[5]);
            VehReadUInt(tun, 0x3170db50, mTunable[6]);
            VehReadUInt(tun, 0x4dfb4144, mTunable[7]);
            VehReadFloat(tun, 0x5d43cab4, mTunableF[0]);
            VehReadFloat(tun, 0x44aeb188, mTunableF[1]);
        }
        VehNoun* n = VehNounManager()->CreateNoun(0x21ffa3f);
        VehRefObj* extra = n ? n->Cast(0xb075dec5) : 0;
        VehRefObj* old = mpExtra;
        if (extra != old) {
            if (extra) extra->AddRef();
            mpExtra = extra;
            if (old) old->Release();
        }
    }
}
