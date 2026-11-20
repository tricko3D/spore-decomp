// s00dcedb0.cpp -- cMovingFormation per-tick update (0x00dcedb0)
// Reconstructed from the annotated disassembly; raw member offsets as used by
// the neighbouring slice s00dce4d0.  Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
typedef unsigned int       uint32_t;
typedef unsigned char      uint8_t;
typedef unsigned long long uint64_t;

#define VF(p, off) (*(void**)((char*)(*(void**)(p)) + (off)))
// the entity's vehicle/audio sub-object: retail re-reads it from the entity
// every time it is used, so it is a macro rather than a local.
#define EV(e) (*(char**)((char*)e + 0xaf0))
// element k of this object's entity list at +0x10 / +0x14
#define LIST(k) (((TObj**)*(char**)((char*)this + 0x10))[k])

extern "C" void* __cdecl FUN_00b3d300(void);                 // SP::NounManager
extern "C" void* __cdecl FUN_00b5b800(void);                 // current game mode
extern "C" void* __cdecl FUN_00b3d380(void);
extern "C" void* __cdecl FUN_00a206f0(void);                 // audio system
extern "C" void  __cdecl FUN_00dc6a20(void);                 // SP::RemoveVehicles
extern "C" void  __cdecl FUN_011e0744(void*, void*, void*);  // memmove
extern "C" void  __cdecl FUN_00cd7d10(void);
extern "C" void  __cdecl FUN_00d3d420(void);
extern "C" void  __cdecl FUN_00ae72d0(void);
extern "C" void  __cdecl FUN_00b1e500(void);
extern "C" void  __cdecl FUN_00bc8ff0(float, double, void*, void*, void*,
                                      uint32_t, uint32_t, void*);  // 0x00bc8ff0
extern char  DAT_01654c04;
extern char  DAT_018eb6de8;
extern float DAT_013f9428;

class TObj {
public:
    void   FUN_00dcedb0(uint64_t elapsed);
    void   FUN_00dceca0();
    void*  F_b25fb0();                                   // 0x00b25fb0
    void*  F_b21340(void*, void*, void*, void*, void*);   // 0x00b21340
    void   F_cac710();                                   // 0x00cac710
    void   F_c9f2d0(uint32_t);                           // 0x00c9f2d0
    void   F_ca96b0();                                   // 0x00ca96b0
    void   F_c9e700();                                   // 0x00c9e700
    void   F_c9f5c0();                                   // 0x00c9f5c0
    void   F_ca31f0();                                   // 0x00ca31f0
    void*  F_bc9980(float);                              // 0x00bc9980
    void   F_b225d0(void*);                              // 0x00b225d0
    uint64_t F_b316c0();                                 // 0x00b316c0
};

// ===========================================================================
// 0x00dcedb0 -- timer tick, entity pass, dead-entity compaction, audio flush
// ===========================================================================
// @ 0x00dcedb0
void TObj::FUN_00dcedb0(uint64_t elapsed) {
    if (elapsed > *(uint64_t*)((char*)this + 0x50)) {
        FUN_00dceca0();
        *(uint32_t*)((char*)this + 0x50) = 0x3e8;
        *(uint32_t*)((char*)this + 0x54) = 0;
    } else {
        *(uint64_t*)((char*)this + 0x50) = *(uint64_t*)((char*)this + 0x50) - elapsed;
    }

    if (FUN_00b5b800() == (void*)&DAT_01654c04) {
        char* civ = (char*)((TObj*)FUN_00b3d300())->F_b25fb0();
        *(uint8_t*)(civ + 0x430) = 0;
    }

    TObj* mgr = (TObj*)FUN_00b3d300();
    char* gd = (char*)mgr->F_b21340(
                    (void*)FUN_00cd7d10, (void*)FUN_00d3d420, (void*)FUN_00ae72d0,
                    (void*)FUN_00b1e500, &DAT_018eb6de8);
    int count = (int)(*(char**)(gd + 8) - *(char**)(gd + 4)) >> 2;
    float dt = (float)elapsed * DAT_013f9428;
    int i = 0;
    for (; i < count; i++) {
        TObj* e = ((TObj**)*(void**)(gd + 4))[i];
        e->F_cac710();
        e->F_c9f2d0((uint32_t)elapsed);
        e->F_ca96b0();
        e->F_c9e700();
        if (((uint32_t(__thiscall*)(void*))VF((char*)e + 0x5d0, 0x20))((char*)e + 0x5d0) == 0) {
            if (*(void**)EV(e) != 0) {
                void* h = ((TObj*)(EV(e) + 8))->F_bc9980(dt);
                uint64_t now = ((TObj*)FUN_00b3d380())->F_b316c0();
                FUN_00bc8ff0(dt, (double)now * DAT_013f9428,
                             *(void**)EV(e), e, EV(e) + 0x1d0,
                             *(uint32_t*)(EV(e) + 0x5e8),
                             *(uint32_t*)(EV(e) + 0x5fc), (void*)h);
            }
        }
        e->F_c9f5c0();
        e->F_ca31f0();
    }

    FUN_00dc6a20();

    int n = (int)(*(char**)((char*)this + 0x14) - *(char**)((char*)this + 0x10)) >> 2;
    int k = 0;
    while (k < n) {
        if (((uint8_t(__thiscall*)(void*, float))VF(LIST(k), 0x54))(LIST(k), dt) == 0) {
            ((TObj*)FUN_00b3d300())->F_b225d0(LIST(k));
            TObj* last = *(TObj**)(*(char**)((char*)this + 0x14) - 4);
            TObj* cur = LIST(k);
            if (last != cur) {
                if (last != 0)
                    ((void(__thiscall*)(void*))VF(last, 0))((void*)last);
                LIST(k) = last;
                if (cur != 0)
                    ((void(__thiscall*)(void*))VF(cur, 4))((void*)cur);
            }
            *(uint32_t*)((char*)this + 0x14) = *(uint32_t*)((char*)this + 0x14) - 4;
            TObj* tail = *(TObj**)(*(char**)((char*)this + 0x14));
            if (tail != 0)
                ((void(__thiscall*)(void*))VF(tail, 4))((void*)tail);
            k--;
            n--;
        }
        k++;
    }

    if (*(void**)((char*)this + 0x24) != *(void**)((char*)this + 0x28)) {
        int m = (int)(*(char**)((char*)this + 0x28) - *(char**)((char*)this + 0x24)) >> 2;
        for (int j = 0; j < m; j++) {
            uint32_t val;
            void* sys = FUN_00a206f0();
            if (sys != 0)
                val = ((uint32_t(__thiscall*)(void*))VF(sys, 0x20))(sys);
            else
                val = 0;
            void* snd = ((void**)*(void**)((char*)this + 0x24))[j];
            void* sys2 = FUN_00a206f0();
            if (sys2 != 0) {
                ((void(__thiscall*)(void*, uint32_t))VF(sys2, 0x38))(sys2, 0x3475365);
                ((void(__thiscall*)(void*, uint32_t, void*))VF(sys2, 0x40))(sys2, 0x3475381, snd);
                ((void(__thiscall*)(void*, uint32_t, uint32_t))VF(sys2, 0x40))(sys2, 0x3475385, val);
                ((void(__thiscall*)(void*))VF(sys2, 0x58))(sys2);
            }
        }
        char* b = *(char**)((char*)this + 0x24);
        char* e2 = *(char**)((char*)this + 0x28);
        FUN_011e0744(b, e2, (void*)(e2 - e2));
        *(uint32_t*)((char*)this + 0x28) = *(uint32_t*)((char*)this + 0x28) +
                                 (uint32_t)((int)((e2 - b) >> 2) * -4);
    }
}
