// @ 0x00db0b80 FUN_00db0b80
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
// "TribeEat" task bind: create / fetch the task node, play the bind (idle)
// animation, seed the task state block and hand the node to the animation
// controller.  Returns true.
typedef unsigned int uint32_t;

struct IRef {
    virtual void AddRef();
    virtual void Release();
};

struct Vec3 { float x, y, z; };

// Sub-object at +0x08 of the citizen's animation controller: idle playback.
struct AnimSub {
    void* __thiscall PlayIdleAnimation(unsigned id, int a);   // 0x00bc96a0, ret 8
};

// Citizen animation controller (Citizen +0xb4c).
struct AnimCtl {
    uint32_t pad00;
    uint32_t pad04;
    AnimSub  mIdle;                                           // +0x08
    void __thiscall BindTask(void* task, void* owner, int a, int b);  // 0x00bcb480
};

// Spatial interface sub-object (Citizen +0x34 / +0xc0).
struct Spatial {
    virtual void s00(); virtual void s01(); virtual void s02(); virtual void s03();
    virtual void s04(); virtual void s05(); virtual void s06(); virtual void s07();
    virtual void s08(); virtual void s09(); virtual void s10(); virtual void s11();
    virtual void s12(); virtual void s13(); virtual void s14(); virtual void s15();
    virtual void s16(); virtual void s17(); virtual void s18(); virtual void s19();
    virtual void s20(); virtual void s21();
    virtual bool IsBundle();                                  // +0x58
};

struct Citizen {
    virtual void c00(); virtual void c01(); virtual void c02(); virtual void c03();
    virtual void c04(); virtual void c05(); virtual void c06(); virtual void c07();
    virtual void c08(); virtual void c09(); virtual void c10(); virtual void c11();
    virtual void c12(); virtual void c13(); virtual void c14(); virtual void c15();
    virtual void c16(); virtual void c17(); virtual void c18(); virtual void c19();
    virtual void c20(); virtual void c21(); virtual void c22(); virtual void c23();
    virtual void c24(); virtual void c25(); virtual void c26(); virtual void c27();
    virtual void c28(); virtual void c29(); virtual void c30(); virtual void c31();
    virtual void c32();
    virtual void vf84(void* part, int a, int b);               // +0x84
    void __thiscall PlayIdleSoundDelay(float f);               // 0x00c0b9d0
    uint32_t pad04[(0x34 - 0x04) / 4];
    Spatial  mBundle;                                          // +0x34
    uint32_t pad38[(0xc0 - 0x38) / 4];
    Spatial  mLoco;                                            // +0xc0
    uint32_t padc4[(0xb4c - 0xc4) / 4];
    AnimCtl* mpAnim;                                           // +0xb4c
};

// Second argument: only its +0x18 int matters here.
struct OrderParams {
    uint32_t pad00[6];
    int      mi18;                                             // +0x18
};

// Task state block (6th argument).
struct EatState {
    int           mi0;                                         // +0x00
    unsigned char mb4;                                         // +0x04
    unsigned char pad05[3];
    void*         mp8;                                         // +0x08
    IRef*         mpHolder;                                    // +0x0c
    int           mi10;                                        // +0x10
};

// Static random source at 0x01601760.
struct RandomSource {
    int __thiscall Uniform(int n);                             // 0x00a68fb0, ret 4
};
extern RandomSource g_random1601760;                           // 0x01601760
extern const float  g_f13eecd8;                                // 0x013eecd8

// Task-node factory / lookup used by the behaviour-tree manager.
void* __stdcall FUN_00bc9b00(unsigned id, void* a, void* b);   // 0x00bc9b00, ret 0xc

struct TaskQuery {
    void* __thiscall Fetch();                                  // 0x00bca620
};
struct TaskProto {
    void* __thiscall Build(void* name);                        // 0x00bcb0b0
};
struct TaskNode {
    uint32_t pad00[(0x1a4 - 0x00) / 4];
    int      mi1a4;                                            // +0x1a4
};

void* __cdecl FUN_00d998d0(void* anim, unsigned id);            // 0x00d998d0

char __cdecl FUN_00db0b80(Citizen* c, int, int, int, int, EatState* state, OrderParams* params)
{
    if (state != 0)
        state->mpHolder = 0;

    void* anim = c->mpAnim->mIdle.PlayIdleAnimation(8, 0);
    state->mp8 = 0;
    IRef* found = (IRef*)FUN_00d998d0(anim, 0x4ffcdeda);
    IRef* old = state->mpHolder;
    if (found != old) {
        if (found != 0)
            found->AddRef();
        state->mpHolder = found;
        if (old != 0)
            old->Release();
    }

    state->mb4 = 0;
    state->mi10 = -1;
    state->mi0 = -1;
    c->vf84(0, 0, 0);

    if (params != 0)
        state->mb4 = params->mi18 != 0 ? 1 : 0;

    if (!((Spatial*)((char*)c + 0xc0))->IsBundle()) {
        int r = g_random1601760.Uniform(0x14);
        c->PlayIdleSoundDelay((float)r + g_f13eecd8);
    }

    void* node = ((TaskQuery*)FUN_00bc9b00(0x67609b9, (void*)0xdabd10, c))->Fetch();
    if (node == 0) {
        node = ((TaskProto*)FUN_00bc9b00(0x67609b9, (void*)0x20, (void*)0x18eb4b7))
                   ->Build((void*)0x147c2ac);
        ((TaskNode*)node)->mi1a4 = 0;
    }
    c->mpAnim->BindTask(node, c, 0, 0);
    return 1;
}
