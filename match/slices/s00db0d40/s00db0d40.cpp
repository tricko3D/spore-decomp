// Slice s00db0d40 -- "TribeEat" distance probe.
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
//   The radius/distance comparison is x87 FCOMIP (`fld [radius]; fcompi st(1);
//   fstp st(0); jbe`) on a value returned in st0, with the argument evaluation
//   order of /fp:precise: that pair of opcodes only appears with
//   /arch:SSE /fp:fast.
//
//   00db0d40  task distance probe: if the citizen can reach the bundle the
//             behaviour tree wants to eat, within the radius taken from the
//             property table, re-point the stimulus at it and report success
//             (returns 1.0f); otherwise 0.0f.
//
// The callee at 0x00b3d2b0 is two different (folded) entry points: one that
// takes no argument and one that takes a position plus a range, so the file
// declares it twice under two names, as retail's call sites require.
typedef unsigned int uint32_t;

struct Vec3 { float x, y, z; };

struct Spatial {
    virtual void s00(); virtual void s01(); virtual void s02(); virtual void s03();
    virtual void s04(); virtual void s05(); virtual void s06(); virtual void s07();
    virtual void s08(); virtual void s09(); virtual void s10();
    virtual const Vec3* GetPosition();                 // vtable +0x2c
    virtual void s12(); virtual void s13(); virtual void s14();
    virtual float vf3c();                              // vtable +0x3c
    char* __thiscall BundleQuery();                    // 0x00cee330
};

struct Query {                                         // what 0x00ac79d0 returns
    char* __thiscall BundleQuery();                    // 0x00cee330
    void* __thiscall FUN_00cee540();                   // 0x00cee540
};

struct Container {
    Query* __thiscall getContainer();                  // 0x00ac79d0
};

struct Target {
    uint32_t pad00[(0x34 - 0x00) / 4];
    Spatial  mPos;                                     // +0x34
};

struct Stimuli {
    uint32_t pad00[(8 - 0x00) / 4];
    void*    mpTarget;                                 // +0x08
    void __thiscall FUN_00d997e0();                    // 0x00d997e0
};

struct Citizen {
    bool __thiscall FUN_00c0b770();                    // 0x00c0b770
    uint32_t pad00[(0x34 - 0x00) / 4];
    Spatial  mBundle;                                  // +0x34
    uint32_t pad38[(0xc0 - 0x38) / 4];
    Spatial  mLoco;                                    // +0xc0
};

struct DistanceCalc {
    float __thiscall Result();                         // 0x00b81470
};

Container* __cdecl FUN_00b3d2b0();                                    // 0x00b3d2b0
Container* __stdcall FUN_00b3d2b0_at(const Vec3* pos, float range);   // 0x00b3d2b0 (2-arg fold)
DistanceCalc* __stdcall CalcDistance(const Vec3* a, const Vec3* b);   // 0x00b3d350, ret 8
float __cdecl GetPropertyT_float(void* props, unsigned id, float def); // 0x004e1c70
extern "C" void* g_pProps158128c;                                     // 0x0158128c
extern const float g_f13ebc5c;                                        // 0x013ebc5c

// @ 0x00db0d40
float __cdecl FUN_00db0d40(Citizen* cArg, int, int, int, Stimuli* stim, unsigned char* out)
{
    // the pointer is copied into a local so its incoming-argument home slot is
    // dead; retail reuses that slot for `radius` (frame-less function)
    Citizen* c = cArg;

    // one shared `return 0.0f` tail: retail's failure paths all jump to it and
    // the success path is the fall-through of the radius/distance comparison
    if (c->FUN_00c0b770())
    {
        // declared before the first test: retail keeps this pointer in edi from
        // the prologue on (`push edi` right after `mov esi,[esp+8]`)
        Spatial* bundle = &c->mBundle;

        if (bundle->BundleQuery() != 0)
            return 0.0f;

        if (FUN_00b3d2b0()->getContainer()->BundleQuery() == 0)
            return 0.0f;

        Spatial* self = (Spatial*)((char*)c + 0xc0);
        Container* mgr = FUN_00b3d2b0_at(self->GetPosition(), bundle->vf3c());
        Target* tgt = (Target*)mgr->getContainer()->FUN_00cee540();

        float radius = GetPropertyT_float(g_pProps158128c, 0x1cd417cf, g_f13ebc5c);
        if (tgt == 0)
            return 0.0f;

        float dist = CalcDistance(
            ((Spatial*)((char*)tgt + 0x34))->GetPosition(),
            self->GetPosition())->Result();

        if (radius > dist)
        {
            if (stim != 0)
                stim->FUN_00d997e0();
            stim->mpTarget = tgt;
            *out = 1;
            return 1.0f;
        }
    }
    return 0.0f;
}
