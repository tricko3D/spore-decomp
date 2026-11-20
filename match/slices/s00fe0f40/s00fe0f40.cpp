// Slice s00fe0f40: SP::cAppModeSpace::HandleSimulationUpdate
// Per-frame simulation update: countdown-triggered command, deferred-object drain,
// simulation time accumulation, solar->planet transition, the space-mode UI state
// chain, space-game input/tool handling and the planet destination marker.
#include "types.h"
#include <math.h>

// ---- retail callees (address in the comment) ----
void __cdecl operator_delete__(void* p);                        // 0xf47380
void* __cdecl Fn_00401010();                                    // 0x401010  drain predicate host
void* __cdecl Fn_0067dd20();                                    // 0x67dd20
void* __cdecl App();                                            // 0x67dd10
void* __cdecl UIMgr_67dd50();                                   // 0x67dd50
void* __cdecl ConfigManager();                                  // 0x67dd30
void* __cdecl ObjectDb();                                       // 0x67ddc0
void* __cdecl PlanetModel();                                    // 0xb3d350
void* __cdecl PlanetWatcher();                                  // 0xb3d310
void* __cdecl NounManager();                                    // 0xb3d300
void* __cdecl ToolManager();                                    // 0xb3d390
void* __cdecl SpaceGfx();                                       // 0xb3d470
void* __cdecl CommHostMgr();                                    // 0xb3d490
void* __cdecl SpaceUIManager();                                 // 0xb3d410
void* __cdecl GameTimeManager();                                // 0xb3d380
void* __cdecl World();                                          // 0xb3d230
void* __cdecl GameInputManager();                               // 0xb3d250
void* __cdecl TriggerManager();                                 // 0xb3d4d0
void* __cdecl GetUFOSimulator();                                // 0xffbe50
void* __cdecl GetUniverseContext();                             // 0x1021080
void* __cdecl GetPlayerEmpire();                                // 0x1021300
int*  __cdecl Fn_00c30f90(void* empire);                        // 0xc30f90
void  __cdecl Fn_00e1dc70();                                    // 0xe1dc70
void  __cdecl AdvanceBackgroundSim(int n);                      // 0x1022920
void  __cdecl Fn_00b84270(void* model);                         // 0xb84270
void  __cdecl Fn_00b88420(void* model);                         // 0xb88420
bool  __cdecl Fn_00d09660(void* o);                             // 0xd09660
void  __cdecl Fn_00b6dbd0(float f);                             // 0xb6dbd0
void  __cdecl Fn_00804f10(float x, float y);                    // 0x804f10
void  __cdecl Fn_00fb8db0(void* o);                             // 0xfb8db0
void  __cdecl NormalizedSafe(float* out, const void* in);       // 0x449c20 SP::normalized_safe
void  __cdecl Fn_007c40f0(void* out, void* viewer);             // 0x7c40f0
void  __cdecl Fn_00409930(void* t);                             // 0x409930 Transform ctor
void* __cdecl Fn_00c37360();                                    // 0xc37360
void* __cdecl Fn_00b21340(void* nm, void* a, void* b, void* c, void* d, void* e); // 0xb21340

// thiscall helpers: one dummy class per signature, so the callee stays a member
// function and the first argument is the retail ecx (this pointer).
#define THISCALL_0(ret, name) struct name##_S { ret F(); }; \
    static ret name(void* p) { return ((name##_S*)p)->F(); }
#define THISCALL_1(ret, name, A) struct name##_S { ret F(A a); }; \
    static ret name(void* p, A a) { return ((name##_S*)p)->F(a); }
#define THISCALL_2(ret, name, A, B) struct name##_S { ret F(A a, B b); }; \
    static ret name(void* p, A a, B b) { return ((name##_S*)p)->F(a, b); }
#define THISCALL_3(ret, name, A, B, C) struct name##_S { ret F(A a, B b, C c); }; \
    static ret name(void* p, A a, B b, C c) { return ((name##_S*)p)->F(a, b, c); }

THISCALL_0(void*, Fn_00a1ad60)                       // 0xa1ad60 GetPlayerInventory
THISCALL_0(void*, Fn_00ff3f00)                       // 0xff3f00
THISCALL_0(void*, Fn_0104f930)                       // 0x104f930
THISCALL_0(bool,  Fn_0104bd50)                       // 0x104bd50
THISCALL_0(int,   Fn_0104be00)                       // 0x104be00
THISCALL_0(int,   Fn_00b31c60)                       // 0xb31c60 scaled game time
THISCALL_0(void,  Fn_00b515e0)                       // 0xb515e0 PlanetWatcher::Update
THISCALL_1(void,  Fn_00b5e9a0, int)                  // 0xb5e9a0 SP::UpdateWorldVisibility
THISCALL_1(void,  Fn_00ffe860, int)                  // 0xffe860 UpdateUfoKeys
struct Fn_00ffe570_S { void F(float x, float y, bool b, int i1, int i2, int i3, int i4); };
static void Fn_00ffe570(void* p, float x, float y, bool b, int i1, int i2, int i3, int i4)
{ ((Fn_00ffe570_S*)p)->F(x, y, b, i1, i2, i3, i4); }        // 0xffe570
THISCALL_1(void,  Fn_00ffcab0, float)                // 0xffcab0 Update
THISCALL_1(void,  Fn_00d124e0, int)                  // 0xd124e0
THISCALL_1(void,  Fn_010377f0, int)                  // 0x10377f0 UpdateForSpaceGame
THISCALL_1(void,  Fn_00ae8510, int)                  // 0xae8510 CommHost::Update
THISCALL_0(void,  Fn_0104f2a0)                       // 0x104f2a0
THISCALL_3(void,  Fn_01051090, void*, float*, uint32_t) // PlayerUseToolUpdate
THISCALL_3(void,  Fn_01050bb0, void*, float*, uint32_t) // PlayerUseToolStart
THISCALL_1(void,  Fn_010743a0, uint32_t)             // cSPUISpace::Update
THISCALL_3(void,  Fn_00697b20, float, float, int)    // 0x697b20
THISCALL_1(void,  Fn_007c4010, uint32_t*)            // cViewer::FUN
THISCALL_0(float, Fn_00b7e4d0)                       // 0xb7e4d0
THISCALL_0(float, Fn_00c37120)                       // 0xc37120
THISCALL_0(float, Fn_00c373b0)                       // 0xc373b0
THISCALL_0(float, Fn_00c373c0)                       // 0xc373c0
THISCALL_0(float, Fn_00c373d0)                       // 0xc373d0
THISCALL_0(float, Fn_00c373e0)                       // 0xc373e0
THISCALL_0(bool,  Fn_00e36fa0)                       // 0xe36fa0
THISCALL_0(void,  Fn_00e36de0)                       // 0xe36de0 ResetState
THISCALL_0(bool,  Fn_00e36dd0)                       // 0xe36dd0
THISCALL_0(void,  Fn_00e3b1f0)                       // 0xe3b1f0 HidePanels
THISCALL_0(bool,  Fn_00e36e10)                       // 0xe36e10
THISCALL_3(void,  Fn_00e3e350, char, const char*, char) // 0xe3e350 Activate
THISCALL_0(int64_t, Fn_0093a5e0)                     // 0x93a5e0 GetElapsedTime
THISCALL_0(float, Fn_01017220)                       // 0x1017220

extern float g_timeScale;        // 0x13ec5b4
extern float g_zero;             // 0x13eb1bc
extern float g_one;              // 0x1485720
extern float g_kHalf;            // 0x1471064
extern float g_kb8a0;            // 0x13eb8a0
extern float g_k85378;           // 0x1485378
extern float g_k65544;           // 0x1465544
extern float g_v16d9944;         // 0x16d9944
extern float g_v16d9948;         // 0x16d9948
extern float g_v16d994c;         // 0x16d994c
extern char  gAppNameTag[];      // 0x1654c05
extern void* g_TestSystem;       // 0x15fd928 (cTestSystem singleton)

// ---- vtable thunks ----
typedef void (__thiscall* T0)(void*);
typedef void (__thiscall* T1)(void*, uint32_t);
typedef void (__thiscall* TF)(void*, float);
typedef void (__thiscall* T2)(void*, void*, void*);
typedef bool (__thiscall* TB1)(void*, uint32_t);
typedef bool (__thiscall* TBP)(void*, void*);
typedef void* (__thiscall* TP0)(void*);
typedef void* (__thiscall* TP1)(void*, uint32_t);
typedef uint32_t (__thiscall* TU0)(void*);
static void* Vfn(void* o, int off) { return *(void**)(*(char**)o + off); }

// ---- layout (offsets taken from the retail code) ----
struct Node { Node* next; Node* prev; void* payload; };   // payload at +0x08
struct Vec3 { float x, y, z; };
struct Transform { float m[11]; };   // 0x2c bytes

struct cSpaceGameState {
    uint8_t pad_00[0x14];
    void*   mpUISpace;      // +0x14
    uint8_t pad_18[0x08];
    void*   mpObj20;        // +0x20
    uint8_t pad_24[0x2c];
    uint8_t mFlags50;       // +0x50
};

struct cInventory {
    uint8_t pad_00[0x604];
    float   mField604;      // +0x604
};

namespace SP {
struct cAppModeSpace {
    void HandleSimulationUpdate(float dtA, float dtB);
    void CompleteTransitionFromSolarToPlanet();     // 0xfe0c60
    void CompleteTransitionFromEntryToNewGame();    // 0xfdbc20
    void TransitionFromCivToSpace(int a, int b);    // 0xfdba50

    uint8_t          pad_000[0xc8];
    uint8_t          mTransform_c8[0x20];   // +0xc8 .. +0xe7
    int32_t          m_e8;                  // +0xe8
    uint8_t          pad_ec[0x01];          // +0xec
    bool             m_ed;                  // +0xed
    bool             m_ee;                  // +0xee
    uint8_t          pad_ef[0x21];          // +0xef .. +0x10f
    uint8_t          mStopwatch110[0x10];   // +0x110
    uint8_t          pad_120[0x08];         // +0x120 .. +0x127
    float            m_f128;                // +0x128
    float            m_f12c;                // +0x12c
    bool             m_130;                 // +0x130
    bool             m_131;                 // +0x131
    uint8_t          pad_132[0x2a];         // +0x132 .. +0x15b
    cSpaceGameState* mpSpaceState;          // +0x15c
    uint8_t          pad_160[0x04];         // +0x160 .. +0x163
    uint32_t         m_164;                 // +0x164
    uint8_t          pad_168[0x02];         // +0x168 .. +0x169
    bool             m_16a;                 // +0x16a
    uint8_t          pad_16b[0x05];         // +0x16b .. +0x16f
    Node             mList170;              // +0x170
    uint8_t          pad_17c[0x11];         // +0x17c .. +0x18c
    bool             m_18d;                 // +0x18d
    bool             m_18e;                 // +0x18e
    bool             m_18f;                 // +0x18f
    bool             m_190;                 // +0x190
    bool             m_191;                 // +0x191
    bool             m_192;                 // +0x192
    uint8_t          pad_193[0x01];         // +0x193
    uint32_t         m_194;                 // +0x194
};

// @ 0x00fe0f40
void cAppModeSpace::HandleSimulationUpdate(float dtA, float dtB)
{
    if (m_194 != 0) {
        if (--m_194 == 0) {
            void* p = Fn_0067dd20();
            ((T1)Vfn(p, 0x28))(p, 0x21851ebe);
        }
        return;
    }

    // the deferred-object list at +0x170: drop entries until the predicate stops us
    if (mList170.next != &mList170) {
        void* pred = Fn_00401010();
        if (!((TBP)Vfn(pred, 0x3c))(pred, &mList170.next->payload)) {
            for (;;) {
                Node* n = mList170.next;
                n->prev->next = n->next;
                n->next->prev = n->prev;
                operator_delete__(n);
                if (mList170.next == &mList170) {
                    m_16a = true;
                    break;
                }
                if (((TBP)Vfn(pred, 0x3c))(pred, &mList170.next->payload))
                    break;
            }
        }
    }

    float fA = dtB * g_timeScale;
    __declspec(align(8)) unsigned __int64 msA = (unsigned __int64)fA;
    m_164 += (uint32_t)msA;

    if (mpSpaceState->mFlags50 & 2) {
        void* pm23 = *(void**)((char*)PlanetModel() + 0x24);
        if (((TU0)Vfn(pm23, 0x50))(pm23)) {
            if (m_16a || m_164 > 0x3a98) {
                Fn_00b84270(PlanetModel());
                Fn_00b515e0(PlanetWatcher());
                Fn_00b88420(PlanetModel());
                CompleteTransitionFromSolarToPlanet();
            }
        }
    }

    int a = (int)fA;
    float fB = dtA * g_timeScale;
    int b = (int)fB;
    (void)b;

    if (mpSpaceState->mFlags50 & 0x10) {
        if (!m_16a) {
            Fn_00b5e9a0(World(), (uint32_t)a);
            return;
        }
        CompleteTransitionFromEntryToNewGame();
        TransitionFromCivToSpace(1, 1);
    }

    void* ui = SpaceUIManager();
    if (m_18e && Fn_00e36fa0(ui)
        && (g_TestSystem == 0
            || *(void**)((char*)g_TestSystem + 0x70) == (char*)g_TestSystem + 0x70)) {
        if (m_18f) {
            Fn_00e36de0(ui);
            m_18f = false;
            m_190 = true;
            return;
        }
        if (m_190 && Fn_00e36dd0(ui)) {
            Fn_00e3b1f0(ui);
            m_190 = false;
            m_191 = true;
            return;
        }
        if (m_191 && Fn_00e36e10(ui)) {
            m_191 = false;
            m_192 = true;
            return;
        }
        if (m_192) {
            m_192 = false;
            Fn_00e3e350(ui, 0, gAppNameTag, 1);
            return;
        }
        Fn_00b5e9a0(World(), (uint32_t)a);
        return;
    }

    if (m_131) {
        if (((int*)TriggerManager())[0x2c / 4] != 1
            && ((int*)TriggerManager())[0x2c / 4] != 2) {
            m_131 = false;
            if (*Fn_00c30f90(GetPlayerEmpire()) == 0)
                Fn_00e1dc70();
        }
    }

    void* ctx = GetUniverseContext();
    void* world = World();
    int gameTime = Fn_00b31c60(GameTimeManager());
    AdvanceBackgroundSim(gameTime);
    ((T1)Vfn(world, 0x2c))(world, (uint32_t)a);

    bool inputFlag = (((uint8_t*)GameTimeManager())[0x48] & 1) != 0;
    Fn_00ffe860(GetUFOSimulator(), a);

    if (mpSpaceState->mpObj20)
        Fn_00d124e0(mpSpaceState->mpObj20, a);

    if (!inputFlag) {
        void** vec = (void**)Fn_00b21340(NounManager(), (void*)0xcd7d10, (void*)0xd3d420,
                                         (void*)0xcdb110, (void*)0xb1e520, (void*)0xce9f6639);
        int n = (int)((char*)vec[2] - (char*)vec[1]) >> 2;
        for (int i = 0; i < n; i++) {
            void* o = ((void**)vec[1])[i];
            if (!o)
                continue;
            ((T0)Vfn(o, 0x00))(o);
            if (((uint8_t*)o)[0x135])
                ((TF)Vfn(o, 0x60))(o, fB);
            ((T0)Vfn(o, 0x04))(o);
        }
        Fn_0104f2a0(ToolManager());
    }

    Fn_010377f0(SpaceGfx(), a);
    Fn_00ae8510(CommHostMgr(), a);

    if (!inputFlag) {
        void* toolMgr = ToolManager();
        void* tool = Fn_00ff3f00(Fn_00a1ad60(mpSpaceState));
        if (tool) {
            void* prop = Fn_0104f930(tool);
            if (prop) {
                Vec3 pos;
                pos.x = g_v16d9944;
                pos.y = g_v16d9948;
                pos.z = g_v16d994c;
                ((T1)Vfn(prop, 0x3c))(prop, (uint32_t)&pos);
                ((T2)Vfn(prop, 0x2c))(prop, &pos, tool);
                if (((TB1)Vfn(GameInputManager(), 0x18))(GameInputManager(), 0xddddddddu)
                    && !((TB1)Vfn(GameInputManager(), 0x18))(GameInputManager(), 0xcdcdcdcdu)) {
                    if (Fn_0104bd50(tool)) {
                        Fn_01051090(toolMgr, tool, (float*)&pos, (uint32_t)a);
                    } else if (Fn_0104be00(tool) != -1 && !m_18d) {
                        Fn_01050bb0(toolMgr, tool, (float*)&pos, 1);
                    }
                }
            }
        }

        if (ctx == 0) {
            void* viewer = ((TP0)Vfn(((TP0)Vfn(App(), 0x50))(App()), 0x38))(((TP0)Vfn(App(), 0x50))(App()));
            if (viewer) {
                void* mode = ((TP1)Vfn(viewer, 0x0c))(viewer, 0x303154cdu);
                if (mode) {
                    int64_t elapsed = Fn_0093a5e0(mStopwatch110);
                    if (((TB1)Vfn(GameInputManager(), 0x18))(GameInputManager(), 0xccccccccu)
                        && elapsed > 250) {
                        // aim the planet destination at the screen centre
                        void* v1 = ((TP0)Vfn(UIMgr_67dd50(), 0x1c))(UIMgr_67dd50());
                        float w = (float)*(int*)((char*)v1 + 8);
                        float h = (float)*(int*)((char*)v1 + 0xc);
                        void* v2 = ((TP0)Vfn(((TP0)Vfn(App(), 0x50))(App()), 0x1c))(((TP0)Vfn(App(), 0x50))(App()));
                        uint32_t rect[4];
                        Fn_007c4010(v2, rect);
                        int cx = (rect[0] + rect[2]) / 2;
                        int cy = (rect[1] + rect[3]) / 2;

                        void* inv2 = Fn_00a1ad60(GetUFOSimulator());
                        float radius = Fn_00b7e4d0(PlanetModel());
                        float d1 = Fn_00c37120(inv2) - radius;
                        float d2 = Fn_01017220(mode) - radius;
                        float ratio = d1 / d2;
                        float c = ratio < 0.0f ? 0.0f : ratio;
                        c = c > 1.0f ? 1.0f : c;
                        float k = 1.0 - c;

                        float a0 = Fn_00c373b0(Fn_00c37360());
                        float a1 = Fn_00c373c0(Fn_00c37360());
                        float dA = (a1 - a0) * k;
                        float b0 = Fn_00c373d0(Fn_00c37360());
                        float b1 = Fn_00c373e0(Fn_00c37360());
                        float dB = (b1 - b0) * k;

                        ((cInventory*)inv2)->mField604 = 0.0f;
                        float dx = ((w - (float)cx) / (float)cx) * (dB + b0);
                        float dy = ((h - (float)cy) / (float)cy) * (dA + a0);
                        Fn_00ffe570(GetUFOSimulator(), dx, dy, false, 1, 0, 0, 0);
                    } else {
                        void* o = ((TP0)Vfn(UIMgr_67dd50(), 0x1c))(UIMgr_67dd50());
                        bool flag = ((uint8_t*)o)[0x10] != 0;
                        inputFlag = flag;
                        if (((TB1)Vfn(ConfigManager(), 0x30))(ConfigManager(), 0x636ec26u) && flag) {
                            float x0 = (float)*(int*)((char*)o + 8);
                            if (x0 >= g_k85378) {
                                float x1 = (float)*(int*)o;
                                if (x1 > x0) {
                                    bool neg;
                                    float v;
                                    if (x0 > x1 * g_kHalf) {
                                        v = x1 - x0;
                                        neg = true;
                                    } else {
                                        v = x0;
                                        neg = false;
                                    }
                                    if (fabs(v * g_kb8a0) <= 1.0) {
                                        cInventory* pinv = (cInventory*)Fn_00a1ad60(GetUFOSimulator());
                                        pinv->mField604 = neg ? g_zero : g_one;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (!m_130 && mpSpaceState && mpSpaceState->mpObj20
        && !Fn_00d09660(mpSpaceState->mpObj20)) {
        bool apply;
        int type = (int)ctx;
        if (type == 0) {
            apply = ((TB1)Vfn(GameInputManager(), 0x18))(GameInputManager(), 0xcdcdcdcdu);
        } else if (type < 1 || type > 2) {
            apply = false;
        } else {
            apply = m_ed || m_ee;
        }
        if (apply) {
            Fn_00804f10(m_f128, m_f12c);
            Fn_00697b20(mTransform_c8, m_f128, m_f12c, m_e8);
        }
    }
    m_130 = false;

    void* appViewer = ((TP0)Vfn(App(), 0x50))(App());
    ((T1)Vfn(appViewer, 0x2c))(appViewer, (uint32_t)a);
    Fn_00ffcab0(GetUFOSimulator(), fB);
    Fn_010743a0(mpSpaceState->mpUISpace, (uint32_t)a);
    Fn_00b6dbd0(dtB);

    void* db = ObjectDb();
    if (db && ctx == 0 && ((TU0)Vfn(db, 0x4c))(db) == 0x5e51c01u) {
        void* inv3 = Fn_00a1ad60(GetUFOSimulator());
        if (inv3) {
            Vec3 v;
            void* sub = (char*)inv3 + 0x34;
            void* src = ((TP0)Vfn(sub, 0x2c))(sub);
            NormalizedSafe(&v.x, src);
            v.x *= g_k65544;
            v.y *= g_k65544;
            v.z *= g_k65544;
            Vec3 out2;
            ((T2)Vfn(ObjectDb(), 0x38))(ObjectDb(), &v, &out2);
        }

        void* viewer3 = ((TP0)Vfn(((TP0)Vfn(App(), 0x50))(App()), 0x1c))(((TP0)Vfn(App(), 0x50))(App()));
        if (viewer3) {
            Transform t;
            Fn_00409930(&t);
            Fn_007c40f0(&t, viewer3);
            ((T1)Vfn(ObjectDb(), 0x3c))(ObjectDb(), (uint32_t)((char*)&t + 4));
        }

        void* pm = PlanetModel();
        if (pm && *(void**)((char*)pm + 0x24)) {
            void* pm24 = *(void**)((char*)pm + 0x24);
            Vec3 marker;
            void* r = ((TP1)Vfn(pm24, 0x10))(pm24, (uint32_t)&marker);
            Fn_00fb8db0(r);
            ((T1)Vfn(ObjectDb(), 0x30))(ObjectDb(), (uint32_t)&marker);
            ((T0)Vfn(ObjectDb(), 0x0c))(ObjectDb());
        }
    }

    ((T1)Vfn(world, 0x30))(world, (uint32_t)a);
    Fn_00b5e9a0(world, a);
    m_18d = false;
}
}
