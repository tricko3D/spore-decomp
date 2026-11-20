// SP::MATE_LEAD_Tick (0x00d6d5c0), SPCreatureMate.obj.
//
// Modes 0/1: wait for the animal carrying the "mate lead" stimulus to expose the
// mate behaviour tree (key 0x91bffc22), then switch to mode 2.  Mode 2 walks the
// pair through a four-step cut-scene (approach, trigger/event posts, animation
// selection, animation-finished handshake).
//
// Layouts and callee spellings follow src/game/SPGonzago/SPCreatureMate.cpp,
// which holds the other functions from the same object file.

typedef unsigned int   u32;
typedef unsigned char  u8;

struct Vec3 { float x, y, z; };

struct Vtbl { void* mSlots[0x120 / 4]; };
typedef void   (__thiscall *VFn_v)(void*);
typedef void*  (__thiscall *VFn_p)(void*);
typedef char   (__thiscall *VFn_c)(void*);
typedef float  (__thiscall *VFn_f)(void*);
typedef Vec3*  (__thiscall *VFn_pf)(void*, void*);
typedef void   (__thiscall *VFn_viii)(void*, int, int, int);
#define VSLOT(o, n) (((Vtbl*)*(void**)(o))->mSlots[(n) / 4])

// ---- unnamed retail callees ------------------------------------------------
extern void* __cdecl FUN_00b3d350();                              // 0xb3d350 SP::PlanetModel
extern void* __cdecl FUN_00d99980(void* stim);                    // 0xd99980 GetAnimalFromStimulus
extern char  __cdecl FUN_00d99550(void* anim);                    // 0xd99550 animation still running
extern char  __cdecl FUN_00d99570(void* animal, int a, int b);    // 0xd99570 play animation
extern void* __cdecl FUN_00d40b40(int a);                         // 0xd40b40 GetDesiredAvatarHerd
extern void  __cdecl FUN_00d40cc0(void* member);                  // 0xd40cc0 ClaimNearbyNestForAvatar
extern void* __cdecl FUN_00d539d0();                              // 0xd539d0 cPosseSimulator::Instance
extern void* __cdecl FUN_00d38840();                              // 0xd38840 cCreatureModeStrategy::Instance
extern void* __stdcall FUN_00b3d4d0(int key);                     // 0xb3d4d0 GetTriggerMgr
extern char  __cdecl FUN_004232c0(const Vec3* a, const Vec3* b);  // 0x4232c0 Vector3Equal
extern void* __cdecl FUN_00449c20(Vec3* out, const Vec3* in);     // 0x449c20 normalized_safe
extern int   __fastcall FUN_00d52df0(void* self);                 // 0xd52df0 posse member count

struct PlanetModel {
    Vec3* FUN_00b81630(Vec3* dst, const Vec3* src);               // 0xb81630 ProjectToSurface
};
struct RandomLCG {
    u32 FUN_00a68fb0(u32 range);                                  // 0xa68fb0 RandomUint32Uniform
};
struct Engine {
    void  FUN_00ae09b0(void* target);                             // 0xae09b0 PostAction
    void  FUN_00ae0930(int a, int b, int c, int d, int e);        // 0xae0930 PostTrigger
};
struct CreatureModeStrategy {
    void  FUN_00d3cdc0(u32 event);                                // 0xd3cdc0 SignalCreatureEvent
};
struct StrategyEventSink {
    void  FUN_00d39360(u32 event, void* data);                    // 0xd39360 PostKillMessage
};
struct PosseSim {
    int   FUN_00d52df0();                                         // 0xd52df0
};
struct ActionTarget {
    void* mPad[3];
    ActionTarget* FUN_00ad7a30(void* a, int b);                   // 0xad7a30 ctor
    void          FUN_00ad7ad0();                                 // 0xad7ad0 dtor
};
struct BehaviorStimuli {
    void* mPad;                                                   // +0x00 (4 bytes)
    void* Play(int flags, int a);                                 // 0xbc96a0
};
struct BehaviorTree {
    u8              mPad00[8];                                    // +0x000
    BehaviorStimuli  mStimuli;                                    // +0x008
    u8              mPad0c[0x1d8 - 0x0c];                         // +0x00c
    u32             mKey;                                         // +0x1d8
};
struct CreatureFlags {
    u32 mBit0_3   : 4;
    u32 mBit4     : 1;
    u32 mBit5     : 1;
    u32 mBit6     : 1;
    u32 mBit7_8   : 2;
    u32 mBit9     : 1;
    u32 mBit10_31 : 22;
};
struct Locomotive {
    Vtbl*   mpVtable;                                             // +0x00
    u8      mPad04[0x60];                                         // +0x04
    char  FUN_00c42e20();                                         // 0xc42e20 IsNearGoal
};
struct Creature {
    Vtbl*         mpVtable;                                       // +0x0000
    u8            mPad0004[0xc0 - 0x04];                          // +0x0004
    Locomotive    mLoco;                                          // +0x00c0
    u8            mPadLoco[0xb4c - 0xc0 - sizeof(Locomotive)];    // +0x0124
    BehaviorTree* mpBehaviorTree;                                 // +0x0b4c
    u32           mB50;                                           // +0x0b50
    u32           mB54;                                           // +0x0b54
    CreatureFlags mFlags;                                         // +0x0b58

    void* FUN_00c0ee90();                                         // 0xc0ee90
    int   FUN_00c12310(int anim, int a, int b);                   // 0xc12310
    char  FUN_00c12400(int anim);                                 // 0xc12400
    char  FUN_00c123f0(int anim);                                 // 0xc123f0
    void  FUN_00c02c20(int a);                                    // 0xc02c20
    void  FUN_00c1c5c0(int mode, Vec3* p, Vec3* d, float a, float b); // 0xc1c5c0
};
struct Herd {
    u8    mPad0000[0x160];
    void* mpMember;                                               // +0x160
};
// +0x00 mode, +0x04 step, +0x08 flag, +0x0c animation handle
struct MateLeadState {
    int mMode;
    u32 mStep;
    u8  mFlag08;
    u8  mPad09[3];
    int mAnim;
};
// The retail frame gives each posted event record a 24-byte slot although only
// the first three words are written, so the real record type is 24 bytes.
struct EventData {
    void* mA;
    void* mB;
    void* mC;
    void* mPad[3];
};

// @ 0x00d6d5c0
char __cdecl FUN_00d6d5c0(void* c, int a2, int p3, int p4, int p5, void* state, int p7, float dt)
{
    (void)a2; (void)p3; (void)p4; (void)p5; (void)p7; (void)dt;
    Creature* cr = (Creature*)c;
    MateLeadState* st = (MateLeadState*)state;
    // Declared up front: retail gives every one of these temps its own frame
    // slot (see the case-0 vector walk), so their lifetimes span the function.
    PlanetModel* pm;
    Vec3 dir, matePos, target, pos, p2, p1, delta, back, surf2, face, surf1, scaled, scaled2;
    EventData ev1a, ev1b, ev2a, ev2b;
    ActionTarget action;

    if (st->mMode < 0)
        return 1;

    if (st->mMode > 1) {
        // ---- mode 2: the lead-the-mate sequence ----------------------------
        if (st->mMode != 2)
            return 1;
        Creature* mate = (Creature*)cr->FUN_00c0ee90();
        if (mate == 0)
            return 0;

        switch (st->mStep) {
        case 0: {
            // Walk both animals onto the surface points in front of the pair
            // and turn the follower towards the leader.
            pm = (PlanetModel*)FUN_00b3d350();
            Locomotive*  mateLoco = &mate->mLoco;

            float* mp = (float*)((VFn_p)VSLOT(mateLoco, 0x2c))(mateLoco);
            matePos.x = mp[0];
            matePos.y = mp[1];
            matePos.z = mp[2];
            float* mp2 = (float*)((VFn_p)VSLOT(mateLoco, 0x2c))(mateLoco);
            target.x = mp2[0];
            target.y = mp2[1];
            target.z = mp2[2];

            Locomotive* loco = &cr->mLoco;
            float* cp = (float*)((VFn_p)VSLOT(loco, 0x2c))(loco);
            pos.x = cp[0];
            pos.y = cp[1];
            pos.z = cp[2];

            delta.x = pos.x - matePos.x;
            delta.y = pos.y - matePos.y;
            delta.z = pos.z - matePos.z;

            FUN_00449c20(&dir, &delta);
            if (FUN_004232c0(&dir, (const Vec3*)0x169ed64) != 0) {
                // Degenerate direction: use the leader's facing, reversed.
                Vec3* fp = (Vec3*)((VFn_pf)VSLOT(loco, 0x5c))(loco, &face);
                dir.x = -fp->x;
                dir.y = -fp->y;
                dir.z = -fp->z;
            }

            float mateSpeed = ((VFn_f)VSLOT(mateLoco, 0x74))(mateLoco);
            scaled.x = mateSpeed * dir.x;
            scaled.y = mateSpeed * dir.y;
            scaled.z = mateSpeed * dir.z;
            p1.x = matePos.x - scaled.x;
            p1.y = matePos.y - scaled.y;
            p1.z = matePos.z - scaled.z;
            target = *pm->FUN_00b81630(&surf1, &p1);

            float crSpeed = ((VFn_f)VSLOT(loco, 0x74))(loco);
            scaled2.x = crSpeed * dir.x;
            scaled2.y = crSpeed * dir.y;
            scaled2.z = crSpeed * dir.z;
            p2.x = matePos.x + scaled2.x;
            p2.y = matePos.y + scaled2.y;
            p2.z = matePos.z + scaled2.z;
            pos = *pm->FUN_00b81630(&surf2, &p2);

            float mateSpeed2 = ((VFn_f)VSLOT(mateLoco, 0x74))(mateLoco);
            mate->FUN_00c1c5c0(2, &target, &dir, *(float*)0x1488874, mateSpeed2);

            back.x = -dir.x;
            back.y = -dir.y;
            back.z = -dir.z;
            float crSpeed2 = ((VFn_f)VSLOT(loco, 0x74))(loco);
            cr->FUN_00c1c5c0(2, &pos, &back, *(float*)0x1488874, crSpeed2);

            st->mStep = 1;
            return 1;
        }

        case 1: {
            // Both animals have arrived: stop them, post the mate-lead
            // triggers and pick the pair's reaction animation.
            if (mate->mLoco.FUN_00c42e20() == 0)
                return 1;
            if (cr->mLoco.FUN_00c42e20() == 0)
                return 1;

            ((VFn_v)VSLOT(&cr->mLoco, 0xec))(&cr->mLoco);
            ((VFn_v)VSLOT(&mate->mLoco, 0xec))(&mate->mLoco);

            short b9 = cr->mFlags.mBit9;
            if (b9) {
                Herd* herd = (Herd*)FUN_00d40b40(1);
                if (herd != 0 && herd->mpMember != 0)
                    FUN_00d40cc0(herd->mpMember);
            }

            ActionTarget* pat = action.FUN_00ad7a30(&cr->mLoco, 0);
            ((Engine*)FUN_00b3d4d0(0x112aea10))->FUN_00ae09b0(pat);
            action.FUN_00ad7ad0();

            ((Engine*)FUN_00b3d4d0(0x147adc0))->FUN_00ae0930(1, 0, 0, 0, 0);
            ((CreatureModeStrategy*)FUN_00d38840())->FUN_00d3cdc0(0x335dd8bb);

            switch (((RandomLCG*)0x1601760)->FUN_00a68fb0(4)) {
            case 0:
                st->mAnim = cr->FUN_00c12310(0x31527640, -1, 0);
                mate->FUN_00c12310(0x31527641, -1, 0);
                break;
            case 1:
                st->mAnim = cr->FUN_00c12310(0x04f39ea7, -1, 0);
                mate->FUN_00c12310(0x04f39ece, -1, 0);
                break;
            case 2:
                st->mAnim = cr->FUN_00c12310(0x04f39ead, -1, 0);
                mate->FUN_00c12310(0x04f39ee6, -1, 0);
                break;
            case 3:
                st->mAnim = cr->FUN_00c12310(0x04f39ebd, -1, 0);
                mate->FUN_00c12310(0x04f39eef, -1, 0);
                break;
            default:
                break;
            }

            ev1a.mA = cr;
            ev1a.mB = mate;
            ev1a.mC = 0;
            (*(StrategyEventSink**)0x169e294)->FUN_00d39360(0xd335362f, &ev1a);
            ev1b.mA = mate;
            ev1b.mB = cr;
            ev1b.mC = 0;
            (*(StrategyEventSink**)0x169e294)->FUN_00d39360(0xd335362f, &ev1b);

            cr->mFlags.mBit6 = 1;
            st->mStep = 2;
            return 1;
        }

        case 2: {
            // The leader's reaction animation has ended: count the posse and
            // post the second event pair, still reporting "not finished".
            if (cr->FUN_00c12400(st->mAnim) == 0)
                return 1;
            st->mFlag08 = 1;

            int count = ((PosseSim*)FUN_00d539d0())->FUN_00d52df0() + 1;
            cr->FUN_00c02c20(count);

            ev2a.mA = cr;
            ev2a.mB = mate;
            ev2a.mC = 0;
            (*(StrategyEventSink**)0x169e294)->FUN_00d39360(0xd3353630, &ev2a);
            ev2b.mA = mate;
            ev2b.mB = cr;
            ev2b.mC = 0;
            (*(StrategyEventSink**)0x169e294)->FUN_00d39360(0xd3353630, &ev2b);
            return 0;
        }

        case 3:
            // Wait for the follower's mate animation to finish.
            if (cr->FUN_00c123f0(0) != 0)
                return 0;
            return 1;

        default:
            return 1;
        }
    }

    // ---- modes 0/1: acquire the follower --------------------------------
    void* anim = cr->mpBehaviorTree->mStimuli.Play(8, 0);
    Creature* animal = (Creature*)FUN_00d99980(anim);
    if (anim == 0)
        return 0;
    if (animal == 0)
        return 0;

    if (st->mMode != 1) {
        if (FUN_00d99550(anim) != 0) {
            st->mMode = 1;
            return 1;
        }
        if (FUN_00d99570(animal, 8, 0) != 0) {
            if (animal->mpBehaviorTree->mKey != 0x91bffc22) {
                st->mMode = 1;
                return 1;
            }
        }
    }

    if (animal->mpBehaviorTree->mKey != 0x91bffc22)
        return 1;
    st->mMode = 2;
    st->mStep = 0;
    ((VFn_viii)VSLOT(cr, 0x84))(cr, (int)((char*)animal + 0x5a8), 1, 0);
    return 1;
}
