// Slice s00cd7770 -- SP::cTribeModeStrategy::ShowTribeUI (0x00cd7770, dev 0x007c3270)
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
//
// Lazy-creates the tribe input strategy (+0x23c), pushes the tribe UI state into the input
// manager, lazy-creates the display strategy (+0x240), then samples the current camera / tribe
// rotation (quaternion -> 3x3 matrix -> normalised quaternion) and hands the transformed frame
// to the display strategy's +0x120 sub-object.  Finishes by re-syncing the +0x1c8 UI object and
// sending the MessageBasicRC5 notification through the messaging server.
//
// No class header for these strategies exists yet, so each receiver is a stand-in with the
// fields at their retail offsets.  Every method is __thiscall (the receiver arrives in ECX),
// and the constructor-shaped functions return `this` in EAX, as MSVC's constructors do.

#include "game/types.h"

extern "C" long _InterlockedExchange(long volatile*, long);
#pragma intrinsic(_InterlockedExchange)

// ---------------------------------------------------------------------------------------------
//  external data
// ---------------------------------------------------------------------------------------------
extern void*       DAT_0169b27c;                    // 0x0169b27c  refcounted display object
extern void*       DAT_0169b2c8;                    // 0x0169b2c8  current hut tracker
extern const float DAT_01485720;                    // 0x01485720  1.0f
extern const float DAT_01470f1c;                    // 0x01470f1c  2.0f
extern const char  DAT_014775c8[];                  // 0x014775c8  allocator tag (input strategy)
extern const char  DAT_014775a8[];                  // 0x014775a8  allocator tag (display strategy)
extern const char  DAT_01477588[];                  // 0x01477588  allocator tag (ui state)
extern const char  DAT_0679b868[];                  // 0x0679b868  "show UI" config key
extern const char  DAT_04ea96cb[];                  // 0x04ea96cb  "tutorials" config key
extern void*       g_vtbl_013eb90c;                 // 0x013eb90c  message vtable (first)
extern void*       g_vtbl_013eb844;                 // 0x013eb844  message vtable (final)

// ---------------------------------------------------------------------------------------------
//  external callees
// ---------------------------------------------------------------------------------------------
void* __cdecl FUN_00f473a0(unsigned int n, const char* pName, int a, int b, int c, int d);  // 0x00f473a0
void* __cdecl FUN_00b30d70();                       // 0x00b30d70  GetGameTerrainCursor
void  __cdecl FUN_00d48a00();                       // 0x00d48a00
void  __cdecl FUN_00d49450();                       // 0x00d49450
void  __cdecl FUN_00d49b60(void* p);                // 0x00d49b60
void* __cdecl FUN_00883860();                       // 0x00883860  EA::Messaging::GetServer

// ---------------------------------------------------------------------------------------------
//  vtable access helpers (slot N == byte offset N*4)
// ---------------------------------------------------------------------------------------------
typedef void  (__thiscall* VFn)(void*);
typedef void* (__thiscall* VFnP)(void*);
typedef void  (__thiscall* VFnI)(void*, int);
typedef void  (__thiscall* VFnV3)(void*, void*, int, int);
typedef void* (__thiscall* VFnStr)(void*, const void*);

static inline void** Vt(void* p) { return *(void***)p; }

// ---------------------------------------------------------------------------------------------
//  small math types
// ---------------------------------------------------------------------------------------------
struct Matrix3x3_s00cd7770 { float m[9]; };

struct Quaternion_s00cd7770 {
    float x, y, z, w;
    Quaternion_s00cd7770() {}
    Quaternion_s00cd7770(const Quaternion_s00cd7770& o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
};

// 0x00472b80  rw::math::fpu::QuaternionFromMatrix33 (out-parameter form)
Quaternion_s00cd7770& __cdecl FUN_00472b80(Quaternion_s00cd7770& out,
                                           const Matrix3x3_s00cd7770& rot, float epsilon);

// 0x00472b80's inlined front end: build a rotation matrix from the quaternion.
static __forceinline Matrix3x3_s00cd7770 QuatToMatrix(const float* q)
{
    float qx = q[0], qy = q[1], qz = q[2], qw = q[3];
    Matrix3x3_s00cd7770 rot;
    rot.m[0] = DAT_01485720 - DAT_01470f1c * (qy * qy + qz * qz);
    rot.m[1] = DAT_01470f1c * (qw * qz + qy * qx);
    rot.m[2] = DAT_01470f1c * (qz * qx - qw * qy);
    rot.m[3] = DAT_01470f1c * (qy * qx - qw * qz);
    rot.m[4] = DAT_01485720 - DAT_01470f1c * (qx * qx + qz * qz);
    rot.m[5] = DAT_01470f1c * (qw * qx + qz * qy);
    rot.m[6] = DAT_01470f1c * (qw * qy + qz * qx);
    rot.m[7] = DAT_01470f1c * (qz * qy - qw * qx);
    rot.m[8] = DAT_01485720 - DAT_01470f1c * (qx * qx + qy * qy);
    return rot;
}

// ---------------------------------------------------------------------------------------------
//  stand-in objects
// ---------------------------------------------------------------------------------------------
struct cRefCounted_s00cd7770 {                      // intrusive object embedded at display +0x3c
    virtual void AddRef();                          // slot 0
    virtual void Release();                         // slot 1
};

struct cCommunityEditor_s00cd7770 {
    bool FUN_00d09660();                            // 0x00d09660  editor active?
};

class cTribeInputStrategy_s00cd7770 {
public:
    char                        pad0000[0x04];
    char                        mInput;             // +0x04  embedded input interface
    char                        pad0005[0x5c - 0x05];
    cCommunityEditor_s00cd7770* mpCommunityEditor;  // +0x5c
    void* Ctor(void* owner);                        // 0x00cd1790  (returns this)
    void  FUN_00cce610();                           // 0x00cce610
    void  FUN_00cd16a0(void* value);                // 0x00cd16a0
};

class cTribeDisplayStrategy_s00cd7770 {
public:
    char                 pad0000[0x3c];
    cRefCounted_s00cd7770 mRef;                     // +0x3c  embedded ref-counted object
    void* Ctor(void* owner);                        // 0x00cccc50  (returns this)
    void  FUN_00ccdd70();                           // 0x00ccdd70
    void  FUN_00cc8aa0(bool show);                  // 0x00cc8aa0
};

struct cTerrainSphere_s00cd7770 {
    char  pad0000[0x10f0];
    float m10f0;                                    // +0x10f0
    float m10f4;                                    // +0x10f4
    void  FUN_00c77bf0(unsigned int id);            // 0x00c77bf0  (callee pops the id)
};

struct cGameNounManager_s00cd7770 {
    void*                     FUN_00bfc5f0();       // 0x00bfc5f0  GetPlayerTribe
    cTerrainSphere_s00cd7770* FUN_00f67d90();       // 0x00f67d90  GetCurrentTerrainSphere
};
cGameNounManager_s00cd7770* __cdecl FUN_00b3d300(); // 0x00b3d300  NounManager

struct cConfigManager_s00cd7770 {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void* GetValue(const void* key);        // +0x30
};
cConfigManager_s00cd7770* __cdecl FUN_0067dd30();   // 0x0067dd30  ConfigManager

struct cGameInputManager_s00cd7770 {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void SetInputStrategy(void* strategy, int, int);        // +0x20
};
cGameInputManager_s00cd7770* __cdecl FUN_00b3d250(); // 0x00b3d250  GameInputManager

struct cTerrainCameraController_s00cd7770 {
    void FUN_00b12dd0(void* p);                     // 0x00b12dd0
    void FUN_00b13b50();                            // 0x00b13b50  PushToCamera
};
cTerrainCameraController_s00cd7770* __cdecl FUN_00b3d280(); // 0x00b3d280

struct cThemeManager_s00cd7770 {                    // 0x00cd4580 constructor
    void* Ctor();                                   // 0x00cd4580  (returns this)
};

struct cScopeGuard_s00cd7770 {                      // dtor for the message below
    void Dtor();                                    // 0x00421cf0
};

struct cMessageBasicRC5_s00cd7770 {
    void*         mpVtbl;                           // +0x00
    volatile long mRefCount;                        // +0x04
    void*         mData0;                           // +0x08
    char          pad0c[0x10 - 0x0c];
    unsigned int  mData1;                           // +0x10
    char          pad14[0x30 - 0x14];
    unsigned int  mId;                              // +0x30
    int           mRCFlags;                         // +0x38
};

struct cGameMode_s00cd7770 {
    char  pad0000[0x1a4];
    unsigned char m1a4;                             // +0x1a4
    char  pad01a5[0x1c8 - 0x1a5];
    void* m1c8;                                     // +0x1c8  theme manager
    char  pad01cc[0x23c - 0x1cc];
    cTribeInputStrategy_s00cd7770*   mpInputStrategy;   // +0x23c
    cTribeDisplayStrategy_s00cd7770* mpDisplayStrategy; // +0x240
    char  pad0244[0x394 - 0x244];
    void* mpMusic;                                  // +0x394  refcounted
    char  pad0398[0x3a3 - 0x398];
    unsigned char m3a3;                             // +0x3a3

    void ShowTribeUI();
    void FUN_00cd7320(float x, float y, float z);   // 0x00cd7320
};

// ---------------------------------------------------------------------------------------------
//  min by reference: the shape the original uses for the progress ratio
// ---------------------------------------------------------------------------------------------
template <class T> __forceinline const T& min_ref(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

// ---------------------------------------------------------------------------------------------
//  0x00cd7770
// ---------------------------------------------------------------------------------------------
// @ 0x00cd7770
void cGameMode_s00cd7770::ShowTribeUI()
{
    // ---- input strategy (+0x23c), created on first use ---------------------------------------
    if (mpInputStrategy == 0)
    {
        void* pNew = FUN_00f473a0(0xd0, DAT_014775c8, 0, 0, 0, 0);
        if (pNew)
        {
            pNew = ((cTribeInputStrategy_s00cd7770*)pNew)->Ctor(this);
            mpInputStrategy = (cTribeInputStrategy_s00cd7770*)pNew;
            ((VFn)Vt(pNew)[1])(pNew);
        }
        else
        {
            mpInputStrategy = (cTribeInputStrategy_s00cd7770*)pNew;
            ((VFn)Vt(pNew)[1])(pNew);
        }
    }
    else
    {
        mpInputStrategy->FUN_00cce610();
    }

    if (!mpInputStrategy->mpCommunityEditor->FUN_00d09660())
    {
        void* pStr = FUN_0067dd30()->GetValue(DAT_0679b868);
        mpInputStrategy->FUN_00cd16a0(pStr);

        FUN_00b3d250()->SetInputStrategy((char*)mpInputStrategy + 4, 0, 0);
    }

    void* pCursor = FUN_00b30d70();
    ((VFnI)Vt(pCursor)[46])(pCursor, 0);             // +0xb8

    // ---- display strategy (+0x240), created on first use -------------------------------------
    if (mpDisplayStrategy == 0)
    {
        void* pNew = FUN_00f473a0(0x4f8, DAT_014775a8, 0, 0, 0, 0);
        if (pNew)
            pNew = ((cTribeDisplayStrategy_s00cd7770*)pNew)->Ctor(this);

        cTribeDisplayStrategy_s00cd7770* pOld = mpDisplayStrategy;
        if ((cTribeDisplayStrategy_s00cd7770*)pNew != pOld)
        {
            if (pNew)
                ((cTribeDisplayStrategy_s00cd7770*)pNew)->mRef.AddRef();
            mpDisplayStrategy = (cTribeDisplayStrategy_s00cd7770*)pNew;
            if (pOld)
                pOld->mRef.Release();
        }
        mpDisplayStrategy->FUN_00ccdd70();
    }

    // ---- progress ratio: min(m10f0, m10f4) / m10f4 >= 1.0 -----------------------------------
    {
        cTerrainSphere_s00cd7770* pF = FUN_00b3d300()->FUN_00f67d90();
        float* pValue = (float*)pF;
        mpDisplayStrategy->FUN_00cc8aa0(
            min_ref(pValue[0x10f0 / 4], pValue[0x10f4 / 4]) / pValue[0x10f4 / 4] >= DAT_01485720);
    }

    // ---- refcounted music object (+0x394) ----------------------------------------------------
    {
        void* pNew = DAT_0169b27c;
        void* pOld = mpMusic;
        if (pNew != pOld)
        {
            if (pNew)
                ((VFn)Vt(pNew)[1])(pNew);
            mpMusic = pNew;
            if (pOld)
                ((VFn)Vt(pOld)[2])(pOld);
        }
    }

    {
        cTerrainSphere_s00cd7770* pF;
        if (FUN_0067dd30()->GetValue(DAT_04ea96cb) == 0)
        {
            pF = FUN_00b3d300()->FUN_00f67d90();
            pF->FUN_00c77bf0(0x56d1871);
            m1a4 = 0;
        }
    }

    ((VFn)Vt(mpMusic)[8])(mpMusic);

    if (m3a3 == 0)
    {
        void* pTribe = FUN_00b3d300()->FUN_00bfc5f0();
        if (pTribe)
        {
            void* pCam = ((VFnP)Vt(pTribe)[43])(pTribe);                         // +0xac
            float* pPos = (float*)((VFnP)Vt((char*)pCam + 0x34)[11])((char*)pCam + 0x34);  // +0x2c
            FUN_00cd7320(pPos[0], pPos[1], pPos[2]);
        }
    }

    FUN_00d48a00();
    FUN_00d49450();
    {
        void* pMode = DAT_0169b2c8;
        FUN_00d49b60(pMode ? (char*)pMode + 4 : 0);
    }

    // ---- current orientation: quaternion -> rotation matrix -> quaternion --------------------
    {
        void* pTribe = FUN_00b3d300()->FUN_00bfc5f0();
        cTerrainCameraController_s00cd7770* pCam = FUN_00b3d280();
        char* pSub  = (char*)pTribe + 0x120;
        float* q    = (float*)((VFnP)Vt(pSub)[12])(pSub);       // +0x30: quaternion

        Matrix3x3_s00cd7770 rot;
        rot = QuatToMatrix(q);

        Quaternion_s00cd7770 tmp;
        Quaternion_s00cd7770 v = FUN_00472b80(tmp, rot, 0.0f);

        void* pRes = ((void* (__thiscall*)(void*, void*, int))Vt(pSub)[11])(pSub, &v, 1);
        pCam->FUN_00b12dd0(pRes);
        pCam->FUN_00b13b50();
    }

    // ---- ui state object (+0x1c8), created while a MessageBasicRC5 notification is built -----
    {
        cMessageBasicRC5_s00cd7770 msg;
        msg.mpVtbl = &g_vtbl_013eb90c;
        _InterlockedExchange(&msg.mRefCount, 0);
        msg.mpVtbl = &g_vtbl_013eb844;
        msg.mRCFlags = 0;
        msg.mId = 0x1c3d2e0;

        void* pServer = FUN_00883860();
        ((void (__thiscall*)(void*, unsigned int, cMessageBasicRC5_s00cd7770*, int))
         (*(void***)pServer)[5])(pServer, msg.mId, &msg, 0);

        if (m1c8 == 0)
        {
            void* pNew = FUN_00f473a0(0xc8, DAT_01477588, 0, 0, 0, 0);
            if (pNew)
                pNew = ((cThemeManager_s00cd7770*)pNew)->Ctor();

            void* pOld = m1c8;
            if (pNew != pOld)
            {
                if (pNew)
                    ((VFn)Vt(pNew)[1])(pNew);
                m1c8 = pNew;
                if (pOld)
                    ((VFn)Vt(pOld)[2])(pOld);
            }
        }

        ((cScopeGuard_s00cd7770*)&msg)->Dtor();
    }
}
