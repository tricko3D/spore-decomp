// Slice s00d311b0 -- SporeApp.exe: an effect/state-update driver for an audio
// emitter object (0x00d311b0), a game-mode/kind predicate (0x00d317e0) and the
// two heap-sift helpers (0x00d31840 sift-down, 0x00d31980 pop) that a priority
// queue of ref-counted sound-effect objects is built on.
//
// Frozen batch gold6.  32-bit x86, MSVC 2008 SP1, flags /O2 /MD /Gy /TP /arch:SSE /GS-.

#include "types.h"

// Declare N throwaway virtuals so a named slot lands at the right vtable index.
#define PVCAT2(a, b) a##b
#define PVCAT(a, b) PVCAT2(a, b)
#define PV virtual void PVCAT(pv_, __COUNTER__)();
#define PV1 PV
#define PV2 PV PV
#define PV4 PV2 PV2
#define PV8 PV4 PV4

// ---------------------------------------------------------------------------
// external callees
// ---------------------------------------------------------------------------
extern "C" void* SP_GetCurrentGameMode(void);               // 0x00b5b800
extern "C" int   FUN_00d30830(void* p);                     // 0x00d30830

// ---------------------------------------------------------------------------
// Heap element.  Only the two refcount slots are named; the remaining vtable
// entries are padding virtuals so AddRef lands at +0xbc and Release at +0xc0.
// ---------------------------------------------------------------------------
struct HeapObj {
    PV8 PV8 PV8 PV8 PV8 PV4 PV2 PV1
    virtual void AddRef();                                 // vtable +0xbc
    virtual void Release();                                // vtable +0xc0
};

// Ref-counted pointer: the heap stores these by value and every element
// assignment is AddRef(new)/Release(old) with the identity check inlined.
struct Ref {
    HeapObj* p;                                            // +0x00
    Ref() : p(0) {}
    Ref(const Ref& o) { p = o.p; if (p) p->AddRef(); }
    Ref& operator=(const Ref& o)
    {
        HeapObj* old = p;
        HeapObj* nw  = o.p;
        if (nw != old) {
            if (nw) nw->AddRef();
            p = nw;
            if (old) old->Release();
        }
        return *this;
    }
    ~Ref() { if (p) p->Release(); }
    operator HeapObj*() const { return p; }                 // read as a raw ptr
};

// Ordering functor: keeps the element closest to the three-float position at
// the top of the heap.
struct Cmp {
    float px;                                              // +0x00
    float py;                                              // +0x04
    float pz;                                              // +0x08
    bool operator()(HeapObj* a, HeapObj* b) const;         // 0x00d2f3c0
};

// The payload handed to a pop (seven priority floats).
struct Prior {
    float mF[7];
};

// The value carried with a sift (the element payload plus the ordering point).
// The copy constructor must copy the float block before the ref-counted member,
// which is what the original compiler emits.
struct Value {
    Ref   mRef;                                            // +0x00
    Cmp   mCmp;                                            // +0x04
    float mF[4];                                           // +0x10
    Value() {}
    Value(const Prior& src, HeapObj* owner)
    {
        mCmp.px = src.mF[0];
        mCmp.py = src.mF[1];
        mCmp.pz = src.mF[2];
        mF[0] = src.mF[3];
        mF[1] = src.mF[4];
        mF[2] = src.mF[5];
        mF[3] = src.mF[6];
        mRef.p = owner;
        if (mRef.p) mRef.p->AddRef();
    }
    Value(const Value& o)
    {
        mCmp.px = o.mCmp.px;
        mCmp.py = o.mCmp.py;
        mCmp.pz = o.mCmp.pz;
        mF[0] = o.mF[0];
        mF[1] = o.mF[1];
        mF[2] = o.mF[2];
        mF[3] = o.mF[3];
        mRef.p = o.mRef.p;
        if (mRef.p) mRef.p->AddRef();
    }
};

extern "C" void FUN_00d2f880(Ref* first, int top, int hole, Value val);  // 0x00d2f880

// ===========================================================================
// @ 0x00d317e0  FUN_00d317e0
// ===========================================================================
bool FUN_00d317e0(int param_1)
{
    if (SP_GetCurrentGameMode() == (void*)0x1654c10) {
        return false;
    }
    switch (FUN_00d30830((void*)param_1)) {
    case 3:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
        return true;
    }
    return false;
}

// ===========================================================================
// @ 0x00d31840  FUN_00d31840  (sift-down: std::_Adjust_heap of a Ref heap)
// ===========================================================================
void FUN_00d31840(Ref* first, int top, int len, int hole, Value val)
{
    int idx;
    for (; (idx = 2 * hole + 2) < len; hole = idx) {
        if (val.mCmp(first[idx], first[idx - 1])) {
            --idx;
        }
        first[hole] = first[idx];
    }
    if (len == idx) {
        first[hole] = first[idx - 1];
        hole = idx - 1;
    }
    FUN_00d2f880(first, top, hole, val);
}

// ===========================================================================
// @ 0x00d31980  FUN_00d31980  (pop: std::_Pop_heap of a Ref heap)
// ===========================================================================
void FUN_00d31980(Ref* first, Ref* last, Prior val)
{
    Ref tmp(*(last - 1));

    *(last - 1) = *first;

    FUN_00d31840(first, 0, (int)(last - first) - 1, 0, Value(val, tmp.p));
}

// ===========================================================================
// @ 0x00d311b0  FUN_00d311b0
//
// Per-frame driver for the label/effect slot attached to one game object.
// While the +0xc8/+0xc9 one-shot flags are set it first asks the input
// singleton whether the pending action is confirmed and, if so, logs the
// 0x1654c02 string and clears +0xc9.  It then classifies the object under the
// cursor (FUN_00d30830) into a 20-way table of property hashes, tells the
// label manager about a changed hash, keeps a +0xc4 fade timer, and finally
// builds up to two transform messages (position, optional orientation matrix,
// fade time): the first is only sent when the effect manager accepts the hash,
// the second is sent straight to the object held at +0xb4.  The retail
// function always falls out of its tail returning false.
// ===========================================================================

#define VTBL(o) (*(void***)(o))

struct EVec3 { float x, y, z; };
struct EBox { EVec3 lo; EVec3 hi; };
struct EMatrix { float m[9]; };

// Transform message (0x38 bytes, ctor 0x00434040).
struct EMsg {
    unsigned short flags;   // +0x00
    unsigned short seq;     // +0x02
    EVec3          pos;     // +0x04
    float          fade;    // +0x10
    EMatrix        mtx;     // +0x14

    void Init();            // 0x00434040
};

// Callees out of this slice, declared as members so the compiler emits the
// retail __thiscall shape (this in ecx, callee pops its stack arguments).
struct CInput         { bool  Confirm(); };                        // 0x00e36e00
struct CGameModeCtx   { void  Log(const char* s); };               // 0x00b1e410
struct CTargetObj     { int   GetState(); };                       // 0x00c04790
struct CCombatMgr     { void* FindTarget(void* noun); };           // 0x00f3f950
struct CAttackable    { bool  IsAttackable(void* noun); };         // 0x00f1b360
struct CLabelMgr      { int   GetHash();                           // 0x008013c0
                        void  SetHash(unsigned id); };             // 0x00801bb0
struct CRefSlot       { void* Get(); };                            // 0x00a16f40

struct EContext {           // the global at 0x016c7aa4
    char         pad0[0x74];
    CCombatMgr*  m74;       // +0x74
    CAttackable* m78;       // +0x78
};
extern "C" EContext* g_16c7aa4;                             // 0x016c7aa4
extern "C" const float g_13f9428;                           // 0x013f9428  0.001f (s per tick)

extern "C" void* __cdecl FUN_00b3d410();                    // 0x00b3d410
extern "C" void* __cdecl FUN_00b3d320();                    // 0x00b3d320
extern "C" void* __cdecl FUN_00b3d240();                    // 0x00b3d240
extern "C" void* __cdecl FUN_00ac8960(void* arg);           // 0x00ac8960
extern "C" void* __cdecl FUN_0067cab0();                    // 0x0067cab0
extern "C" void* __cdecl FUN_0067ddd0();                    // 0x0067ddd0
extern "C" EMatrix* __cdecl FUN_004a9b40(EMatrix* dst, void* quat);  // 0x004a9b40

// The object carrying the slot: ref-counted object at +0xb4, the object it is
// attached to at +0xb8, the selected hash at +0xbc, a bool at +0xc0, the fade
// timer at +0xc4 and the two one-shot flags at +0xc8/+0xc9.
struct ESlot {
    char     pad0[0xb4];
    void*    m_b4;          // +0xb4
    void*    m_b8;          // +0xb8
    unsigned m_bc;          // +0xbc
    char     m_c0;          // +0xc0
    char     pad1[3];
    float    m_c4;          // +0xc4
    bool     m_c8;          // +0xc8
    bool     m_c9;          // +0xc9

    bool FUN_00d311b0(int dt);
};

bool ESlot::FUN_00d311b0(int dt)
{
    if (m_c8 && m_c9) {
        if (!((CInput*)FUN_00b3d410())->Confirm())
            return false;
        m_c9 = false;
        ((CGameModeCtx*)FUN_00b3d320())->Log((const char*)0x1654c02);
    }

    void* src = FUN_00b3d240();
    void* a = ((void* (__thiscall*)(void*))VTBL(src)[0x30 / 4])(src);
    void* b = a ? ((void* (__thiscall*)(void*, unsigned))VTBL(a)[0x0c / 4])(a, 0x17f243b)
                : 0;

    bool     playEffect = false;
    unsigned id = 0x1002;

    switch (FUN_00d30830(a) - 2) {
    case 0:
        id = 0x5ecbdffd;
        break;
    case 1:
        id = 0xf212a38b;
        break;
    case 2:
        id = 0xf6879190;
        break;
    case 4:
        id = 0xf36a0c84;
        if (SP_GetCurrentGameMode() == (void*)0x1654c10) {
            void* x = FUN_00ac8960(b);
            if (x) {
                if (((CTargetObj*)x)->GetState() == 2) {
                    id = 0x07e849be;
                    break;
                }
                if (((CTargetObj*)x)->GetState() != 1)
                    id = 0x07e849c5;
            } else {
                void* y = g_16c7aa4->m74->FindTarget(b);
                if (y) {
                    int v = *(int*)((char*)y + 0x4a8);
                    if (v == 2)
                        id = 0x07e849be;
                    else if (v != 1)
                        id = 0x07e849c5;
                }
            }
        }
        break;
    case 5:
        id = 0x514ab7b7;
        break;
    case 6:
        id = 0x314ab7b1;
        break;
    case 7:
    case 12:
        id = 0x71664d85;
        break;
    case 8:
    case 22:
        id = 0x03b360dc;
        break;
    case 9:
        id = 0x03b360e2;
        playEffect = true;
        break;
    case 10:
    case 19:
        id = 0x03b360e4;
        playEffect = true;
        break;
    case 11:
        id = 0xf4f6c345;
        break;
    case 13:
        id = 0x755c4eb5;
        break;
    case 14:
    case 18:
        id = 0xb14ab7b9;
        break;
    case 16:
    case 21:
        id = 0x03b360df;
        break;
    case 17:
        id = 0x7542cdb5;
        break;
    case 20:
        id = 0xb14ab7b6;
        break;
    case 23:
        id = 0x342d221f;
        break;
    case 15:
        id = g_16c7aa4->m78->IsAttackable(b) ? 0x08054dea : 0x07a2a419;
        break;
    case 24:
    case 25:
    case 26:
        id = 0x1002;
        break;
    }

    CLabelMgr* mgr = (CLabelMgr*)FUN_0067cab0();
    if (mgr->GetHash() != (int)id)
        mgr->SetHash(id);

    char  flagB = 1;            // raw byte from the found object, copied as-is
    void* c = 0;
    if (b) {
        c = ((void* (__thiscall*)(void*, unsigned))VTBL(b)[0x0c / 4])(b, 0x1186577);
        if (c)
            flagB = *(char*)((char*)c + 0x77);
    }

    if (m_bc == id && m_b8 == b && m_c0 == flagB) {
        m_c4 = 0.1f;
    } else {
        if ((m_c4 -= (float)(unsigned)dt * g_13f9428) < 1.52587890625e-05f
            || flagB != m_c0) {
            m_bc = id;
            void* old = m_b8;
            if (b != old) {
                if (b)
                    ((void (__thiscall*)(void*))VTBL(b)[0 / 4])(b);
                m_b8 = b;
                if (old)
                    ((void (__thiscall*)(void*))VTBL(old)[4 / 4])(old);
            }
            m_c0 = flagB;
            if (m_b4) {
                ((void (__thiscall*)(void*, int))VTBL(m_b4)[0x0c / 4])(m_b4, 0);
                void* dead = m_b4;
                if (dead) {
                    m_b4 = 0;
                    ((void (__thiscall*)(void*))VTBL(dead)[4 / 4])(dead);
                }
            }
            if (playEffect && a && c) {
                EMsg msg;
                msg.Init();

                unsigned fx;
                float    fade;
                if (*(char*)((char*)c + 0x77)) {
                    EVec3* p = ((EVec3* (__thiscall*)(void*))VTBL(c)[0x2c / 4])(c);
                    msg.pos = *p;
                    msg.flags |= 4;
                    msg.seq++;

                    EMatrix  tmp;
                    void*    q  = ((void* (__thiscall*)(void*))VTBL(c)[0x30 / 4])(c);
                    EMatrix* pm = FUN_004a9b40(&tmp, q);
                    msg.mtx = *pm;
                    msg.flags |= 2;
                    msg.seq++;
                    fx = 0x8e37991d;
                    fade = ((float (__thiscall*)(void*))VTBL(c)[0x74 / 4])(c);
                } else {
                    EBox box;
                    ((void (__thiscall*)(void*, EBox*))VTBL(c)[0x6c / 4])(c, &box);

                    // The centre is folded into the corner the getter filled in.
                    box.lo.x = (box.hi.x + box.lo.x) * 0.5f;
                    box.lo.y = (box.hi.y + box.lo.y) * 0.5f;
                    box.lo.z = (box.hi.z + box.lo.z) * 0.5f;
                    msg.pos = box.lo;
                    msg.flags |= 4;
                    msg.seq++;
                    fx = 0x5ebfd37c;
                    fade = ((float (__thiscall*)(void*))VTBL(c)[0x74 / 4])(c);
                }
                msg.seq++;
                msg.fade = fade;

                void* fxMgr = FUN_0067ddd0();
                if (((bool (__thiscall*)(void*, unsigned, int, void*))
                     VTBL(fxMgr)[0x2c / 4])(fxMgr, fx, 0, ((CRefSlot*)&m_b4)->Get())) {
                    ((void (__thiscall*)(void*, EMsg*))VTBL(m_b4)[0x18 / 4])(m_b4, &msg);
                    ((void (__thiscall*)(void*, int))VTBL(m_b4)[8 / 4])(m_b4, 0);
                }
            }
        }
    }

    if (m_b4 && m_b8) {
        void* d = ((void* (__thiscall*)(void*, unsigned))VTBL(m_b8)[0x0c / 4])(m_b8, 0x1186577);
        if (d) {
            EMsg msg;
            msg.Init();

            float fade;
            if (*(char*)((char*)d + 0x77)) {
                EVec3* p = ((EVec3* (__thiscall*)(void*))VTBL(d)[0x2c / 4])(d);
                msg.pos = *p;
                msg.flags |= 4;
                msg.seq++;

                EMatrix  tmp;
                void*    q  = ((void* (__thiscall*)(void*))VTBL(d)[0x30 / 4])(d);
                EMatrix* pm = FUN_004a9b40(&tmp, q);
                msg.mtx = *pm;
                msg.flags |= 2;
                msg.seq++;
                fade = ((float (__thiscall*)(void*))VTBL(d)[0x74 / 4])(d);
            } else {
                EBox box;
                ((void (__thiscall*)(void*, EBox*))VTBL(d)[0x6c / 4])(d, &box);

                // The centre is folded into the corner the getter filled in.
                box.lo.x = (box.hi.x + box.lo.x) * 0.5f;
                box.lo.y = (box.hi.y + box.lo.y) * 0.5f;
                box.lo.z = (box.hi.z + box.lo.z) * 0.5f;
                msg.pos = box.lo;
                msg.flags |= 4;
                msg.seq++;
                fade = ((float (__thiscall*)(void*))VTBL(d)[0x74 / 4])(d);
            }
            msg.seq++;
            msg.fade = fade;

            ((void (__thiscall*)(void*, EMsg*))VTBL(m_b4)[0x18 / 4])(m_b4, &msg);
        }
    }

    return false;
}