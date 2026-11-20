// Slice s00c74e80 - SporeApp 0x00c74e80..0x00c751b4
// SP::cSPToolObject::TakeHit(float, unsigned int, cCombatant::eDamageType,
//                            cSPVector3 const &, SP::cCombatant *)
// Flags: /O2 /Ob1 /MD /Gy /TP /arch:SSE /GS-   (no /EHsc: retail has no SEH
// frame, same as the sibling cCombatant::TakeHit wrappers 0x00bcece0/0x00bd0510)
//
// The base call, the active planet record's tool-object lookup, the two 64-bit
// clock reads and the 20 000-tick throttle follow retail.  The hit message is a
// MessageBasicRC5 (MessageRC vtable at 0x013eb90c, complete vtable 0x013eb844)
// posted through EA's messaging server and destroyed by 0x00421cf0.  The hit
// effect transform is an XformMsg (flags/count, position, scale, 3x3 basis;
// default-initialized from the globals at 0x01694530/0x01694554/0x01485720) fed
// to the effect's slot +0x18.
//
// Retail receives an *interior* `this`: the cCombatant part is the second base
// of the complete tool object, whose head starts 0x508 bytes earlier and whose
// spatial sub-object sits at +0x34 (complete).
#include "types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" long __cdecl _InterlockedExchange(long volatile*, long);
#pragma intrinsic(_InterlockedExchange)

struct Vector3 { float x, y, z; };
struct Quat    { float x, y, z, w; };

struct Matrix3 { float m[9]; };

class cCombatant;

// unnamed retail data referenced by the inlined XformMsg constructor
extern const Matrix3 g_1694530;   // 0x01694530  default basis
extern const Vector3 g_1694554;   // 0x01694554  default position
extern const float   kOne;        // 0x01485720  1.0f

// The transform-update message the hit effect consumes (0x38 bytes).  The
// original resets it to the identity transform inline before filling it in.
struct XformMsg {
    u16     flags;     // +0x00  |2 = basis valid, |4 = position valid
    u16     count;     // +0x02
    Vector3 pos;       // +0x04
    float   scale;     // +0x10
    Matrix3 rot;       // +0x14
    XformMsg() : flags(0), count(0) {}
    void SetPos(const Vector3& v) { pos = v; flags |= 4; ++count; }
    void SetRot(const Matrix3& m) { rot = m; flags |= 2; ++count; }
};

// One entry of the active planet record's tool-object table (cToolObjectData).
struct cToolObjectData {
    u32   mID;        // 0x00
    u8    mPad04[0x10];
    float mValue;     // 0x14
    u8    mPad18[0x14];
};

struct cPlanetRecord {
    u8               mPad000[0x134];
    cToolObjectData* mToolObjectsBegin; // 0x134
    cToolObjectData* mToolObjectsEnd;   // 0x138
};

// Spatial part of the complete tool object, at complete + 0x34.
class cToolSpatial {
 public:
    virtual void s0();  virtual void s1();  virtual void s2();  virtual void s3();
    virtual void s4();  virtual void s5();  virtual void s6();  virtual void s7();
    virtual void s8();  virtual void s9();  virtual void s10();
    virtual Vector3 const& GetPosition();     // +0x2c
    virtual Quat const&    GetOrientation();  // +0x30
};

// Head of the complete object; the cCombatant part follows it at +0x508.
struct cToolObjectHead {
    void**       mVtable;   // 0x000
    u8           mPad004[0x30];
    cToolSpatial mSpatial;  // 0x034
    u8           mPad0b8[0x508 - 0xb8];
};

// The cCombatant part; this is the pointer retail gets in ecx.
struct cToolCombatant {
    u8    mPad000[0x38];
    float mField38;         // 0x38
    u8    mPad03c[0x144 - 0x3c];
    u32   mToolObjectID;    // 0x144
    unsigned __int64 mStamp; // 0x148

    void TakeHit(float, unsigned int, int, Vector3 const&, cCombatant*); // @ 0x00c74e80
};

// ---- effect handle ----------------------------------------------------------
class cIVisualEffect {
 public:
    virtual void AddRef();                 // +0x00
    virtual void Release();                // +0x04
    virtual void Stop(int flags);          // +0x08
    virtual void v0c(); virtual void v10(); virtual void v14();
    virtual void Apply(XformMsg* msg);     // +0x18
};

template <class T> struct AutoRefCount {
    T* mp;
    AutoRefCount() : mp(0) {}
    ~AutoRefCount() { if (mp) mp->Release(); }
    T** AsPPTypeParam() {
        T* p = mp;
        if (p) {
            mp = 0;
            p->Release();
        }
        return &mp;
    }
};

class cEffectsManager {
 public:
    virtual void f0();  virtual void f1();  virtual void f2();  virtual void f3();
    virtual void f4();  virtual void f5();  virtual void f6();  virtual void f7();
    virtual void f8();  virtual void f9();  virtual void f10();
    virtual bool CreateEffect(u32 id, int unused, cIVisualEffect** out); // +0x2c
};

extern void* g_vtbl_13eb90c;    // 0x013eb90c  MessageRC vtable
extern void* g_vtbl_13eb844;    // 0x013eb844  MessageBasicRC5 vtable

// ---- on-stack hit message (EA::Messaging) -----------------------------------
struct IMessageRC {
    virtual int AddRef();                    // +0x00
    virtual int Release();                   // +0x04
};
struct MessageRC : IMessageRC {              // vtable 0x013eb90c
    volatile long mRefCount;                 // +0x04
    MessageRC() { _InterlockedExchange(&mRefCount, 0); }
};
struct MessageBasic5 {
    void* mData0;                            // +0x08
    u8    mPad0c[4];
    u32   mData1;                            // +0x10
    u8    mPad14[0x30 - 0x14];
    u32   mId;                               // +0x30
    u32   mPad34;
    MessageBasic5() : mId(0) {}
};
struct MessageBasicRC5 : MessageBasic5, MessageRC {   // vtable 0x013eb844
    u32 mRCFlags;                            // +0x38
    u32 mPad3c;
    MessageBasicRC5() : mRCFlags(0) {}
    void Destruct();                         // 0x00421cf0
    virtual int AddRef();
    virtual int Release();
};

// EA messenger server; slot 5 posts a message id with its payload.
class cMessageServer {
 public:
    virtual void m0();  virtual void m1();  virtual void m2();  virtual void m3();
    virtual void m4();
    virtual void Post(u32 messageID, void* payload, int flags); // +0x14
};

// Receiver of the direct base call; only the method is modelled.
class cCombatantBase {
 public:
    void TakeHit(float, unsigned int, int, Vector3 const&, cCombatant*); // 0x00bfcdd0
    int  GetDamageState();                                               // 0x008e7f80
};

class cGameTimeManager {
 public:
    unsigned __int64 GetTicks();                                        // 0x00b316c0
};

class cGameNounManager {
 public:
    void* GetAvatar();                                                  // 0x00b1fdb0
};

class cAvatar {
 public:
    void NotifyHit();                                                   // 0x00ff63e0
};

// ---- callees (retail addresses) ---------------------------------------------
cPlanetRecord* __cdecl SP_GetActivePlanetRecord();                     // 0x010212a0
void* __cdecl SP_GameTimeManager();                                    // 0x00b3d380
cEffectsManager* __cdecl SP_EffectsManager();                          // 0x0067ddd0
void* __cdecl EA_Messaging_GetServer();                                // 0x00883860
Matrix3* __cdecl Matrix3FromQuaternion(Matrix3* out, Quat const& q);   // 0x0059c190
void __cdecl FUN_00ff5a10(void* complete, cPlanetRecord*);             // 0x00ff5a10
void* __cdecl SpaceGameGet();                                          // 0x01002bd0

// @ 0x00c74e80
void cToolCombatant::TakeHit(float damage, unsigned int unused, int damageType,
                             Vector3 const& position, cCombatant* attacker)
{
    cToolCombatant* self = this;
    const u32 kMessageThrottle = 0x4e20;   // 20000 ticks
    const u32 kHitMessageID    = 0x005c3bed8;

    ((cCombatantBase*)self)->TakeHit(damage, unused, damageType, position, attacker);

    cPlanetRecord* record = SP_GetActivePlanetRecord();
    cToolObjectData* it  = record->mToolObjectsBegin;
    cToolObjectData* end = record->mToolObjectsEnd;
    if (it != end) {
        u32 id = self->mToolObjectID;
        do {
            if (it->mID == id) {
                it->mValue = self->mField38;
                break;
            }
            it++;
        } while (it != end);
    }

    cGameTimeManager* gm = (cGameTimeManager*)SP_GameTimeManager();
    unsigned __int64 now = gm->GetTicks();

    bool post = true;
    if (now > kMessageThrottle) {
        if (now - self->mStamp < kMessageThrottle)
            post = false;
    }
    if (post) {
        MessageBasicRC5 msg;
        msg.mData0 = (void*)((char*)self - 0x508);
        msg.mData1 = 0xffff0000;
        msg.mId    = kHitMessageID;
        cMessageServer* server = (cMessageServer*)EA_Messaging_GetServer();
        server->Post(msg.mId, &msg, 0);
        msg.Destruct();
    }
    cGameTimeManager* gm2 = (cGameTimeManager*)SP_GameTimeManager();
    self->mStamp = gm2->GetTicks();

    if (((cCombatantBase*)self)->GetDamageState() != 2)
        return;

    cToolObjectHead* complete = (cToolObjectHead*)((char*)self - 0x508);

    AutoRefCount<cIVisualEffect> fx;
    if (SP_EffectsManager()->CreateEffect(0x3cd72eef, 0, fx.AsPPTypeParam())) {
        Matrix3 basis;
        XformMsg xm;
        // The original resets the message to the identity transform inline
        // (basis at 0x01694530, position at 0x01694554, scale 1.0f).
        xm.pos = g_1694554;
        xm.scale = kOne;
        for (int i = 0; i < 9; ++i)
            xm.rot.m[i] = g_1694530.m[i];
        xm.SetRot(*Matrix3FromQuaternion(&basis, complete->mSpatial.GetOrientation()));
        xm.SetPos(complete->mSpatial.GetPosition());
        fx.mp->Apply(&xm);
        fx.mp->Stop(0);
    }

    FUN_00ff5a10(complete, SP_GetActivePlanetRecord());

    if (self->mToolObjectID == 0x405dc1df) {
        cGameNounManager* nouns = (cGameNounManager*)SpaceGameGet();
        cAvatar* avatar = (cAvatar*)nouns->GetAvatar();
        if (avatar != 0)
            avatar->NotifyHit();
    }
}
