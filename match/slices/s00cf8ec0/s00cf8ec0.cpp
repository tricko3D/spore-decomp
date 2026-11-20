// Slice s00cf8ec0 -- Spore Simulator "civilization panel update" helper.
//
// Modelled from the retail disassembly of 0x00cf8ec0: the routine asks the noun
// manager for the current terrain sphere, turns the sphere kind into a
// threshold (0 -> 5, 1 -> 3, otherwise 1), posts a three-dword key to the
// sphere when the player civilization's counter is above that threshold, then
// shows the civilization panel for the player's locomotion component position
// and pushes two of the target's fields into the panel's data block.
//
// The class owning the target object is not identified yet; only the member
// offsets this routine actually touches are modelled, with explicit padding
// for the gaps (0x34: locomotion component with its own vtable, 0xb1c/0xb20).
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-

typedef unsigned int  u32;
typedef unsigned char u8;

// A user copy constructor is required: the retail build copies the by-value
// position through fld/fstp per float (docs/codegen.md 5.1/5.2).
struct Vec3 {
    float x;
    float y;
    float z;

    Vec3() {}
    Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {}
};

// Key posted to the terrain sphere (three dwords; the third is constant).
struct CivEventReq {
    u32 m_id;      // +0x00
    u32 m_field4;  // +0x04
    u32 m_hash;    // +0x08
};

struct cTerrainSphere;

// --------------------------------------------------------------- extern decls
struct NounMgr;
extern "C" NounMgr* SP_NounManager();                                       // 0xb3d300
extern "C" void* EAAllocate(u32 size, const char* tag, int a, int b, int c, int d); // 0xf473a0
extern "C" void* FUN_00ae5c30(void* self);                                  // 0xae5c30 SimSingleton ctor

// ---------------------------------------------------------------- modelled types
struct NounMgr {
    cTerrainSphere* GetCurrentTerrainSphere();   // 0xf67d90
    void*           GetPlayerCivilization();     // 0xb25fb0
};

struct cTerrainSphere {
    int  GetKind();                              // 0xc75420
    u32  Publish(CivEventReq* req);              // 0xc78420
};

struct ICiv {
    int  GetCounter();                           // 0xbf0c50
};

// The panel result object: only its +0xc pointer and SetMode are used here.
struct cPanelData {
    u8  m_pad00[0x24];
    int m_value24;                               // +0x24
    int m_value28;                               // +0x28
};

struct cCivUIResult {
    u8           m_pad00[0xc];
    cPanelData*  m_data0c;                       // +0x0c

    void SetMode(int mode);                      // 0x7eb820
};

struct cSimSingleton {
    cCivUIResult* ShowPanel(int id, Vec3 pos);   // 0xae37c0
};

// Locomotion component at target+0x34; the routine calls its +0x2c virtual,
// which hands back a vector.
struct cLoco {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual Vec3* GetPositionVec();              // vtable +0x2c
};

struct cTarget {
    u8     m_pad00[0x34];
    cLoco  m_loco;                               // +0x34
    u8     m_pad38[0xb1c - 0x34 - sizeof(cLoco)];
    int    m_fieldB1c;                           // +0xb1c
    int    m_fieldB20;                           // +0xb20
};

extern void* g_pSimSingleton;                    // 0x0167a60c

static __forceinline cSimSingleton* SimSingleton()
{
    cSimSingleton* p = (cSimSingleton*)g_pSimSingleton;
    if (p == 0) {
        p = (cSimSingleton*)EAAllocate(0xc8, "Simulator/SimSingleton", 0, 0, 0, 0);
        if (p != 0)
            p = (cSimSingleton*)((void* (__thiscall*)(void*))FUN_00ae5c30)(p);
        else
            p = 0;
        g_pSimSingleton = p;
    }
    return p;
}

// @ 0x00cf8ec0
void __stdcall UpdateCivPanel(cTarget* self)
{
    int threshold = 1;
    switch (SP_NounManager()->GetCurrentTerrainSphere()->GetKind()) {
    case 0:
        threshold = 5;
        break;
    case 1:
        threshold = 3;
        break;
    case 2:
        // Kind 2 keeps the default threshold; the store is dead but it is what
        // makes cl emit the original three-way dispatch (test tree + register
        // held threshold) instead of a two-way one.
        threshold = 1;
        break;
    }

    cTerrainSphere* sphere = (cTerrainSphere*)SP_NounManager()->GetCurrentTerrainSphere();
    if (sphere != 0) {
        if (((ICiv*)SP_NounManager()->GetPlayerCivilization())->GetCounter() > threshold) {
            CivEventReq req;
            req.m_id = 0x521f703;
            req.m_field4 = 0;
            req.m_hash = 0xaa9a8ed7;
            sphere->Publish(&req);
        }
    }

    cCivUIResult* panel = SimSingleton()->ShowPanel(0, *self->m_loco.GetPositionVec());
    panel->SetMode(0xb);
    panel->m_data0c->m_value24 = self->m_fieldB20;
    panel->m_data0c->m_value28 = self->m_fieldB1c;
}
