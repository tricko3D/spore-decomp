// 0x00cad880 -- per-tick path/turn update of the locomotion sub-object embedded at object+0x34.
//
// Structure (from the retail disassembly):
//   1. tick the owner object (0xca72a0), read the current position through vtable slot 0x2c;
//   2. in mode 2 the position is re-projected onto the planet surface (PlanetModel 0xb3d350
//      split into surface/extra vectors, then 0xb81630);
//   3. copy the requested heading into the sub-object (+0x1b4 and +0xd00), zero +0x1c0/+0x1c4/+0xcfc,
//      store the surface position at +0x1a8 and let the owner start the path (0xca0170);
//   4. if the owner refuses the path, hand the heading to vtable slot 0xe0 with the path time
//      scaling factor (global 0x1470f1c) and return;
//   5. ground state 2 runs the ground query (0xcac740) and, on success, only refreshes the
//      path float through vtable slot 0xc4 from the owner value (0xc9f380) and returns;
//   6. otherwise advance to step 4, notify the interface found through owner slot 0x0c with the id
//      at 0x17f243b (0x1021300 then slot 0x4c), pick the state descriptor for the owner mode,
//      clear the path float through slot 0xc4, build the curve request (0xb3d290 + 0xac6a10),
//      reset the ground state and stop the timer at +0xb6c (0xbc3110).
//
// Compiler flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast

struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};

#define VSLOT(o, off) ((*(void***)(o))[(off) / 4])

// -- helper calls (retail addresses) -------------------------------------------------------------
struct SimCurve { void Apply(); };                                            // 0x00ac6a10
struct SimTimer { unsigned char mData[0x28]; void Stop(); };                  // 0x00bc3110
struct Planet { Vec3* ProjectToSurface(); };                                  // 0x00b81630
// Owner object (the sub-object sits at owner+0x34); these are direct calls.
struct SimOuter {
    void  Tick();                                                             // 0x00ca72a0
    bool  BeginPath(const Vec3& heading);                                     // 0x00ca0170
    bool  GroundQuery(Vec3 sample, float zero, float dt);                     // 0x00cac740
    float PathScale();                                                        // 0x00c9f380
};
void  Sim_1021300();                                                          // 0x01021300
static __forceinline void InterfaceNotify(void* o)                            // vtable +0x4c
{
    ((void (__thiscall*)(void*))VSLOT(o, 0x4c))(o);
}

void* __stdcall PlanetModelSplit(Vec3* extra, Vec3* surface);                 // 0x00b3d350

void* __stdcall CreateCurve(const void* desc, const Vec3* surface, const Vec3* heading,
                            void* args, void* path, bool grounded, int index);  // 0x00b3d290

// -- retail globals ------------------------------------------------------------------------------
extern float kPathTime;                     // 0x01470f1c
extern char  kStateDescOne;                 // 0x01565b2c
extern char  kStateDescTwo;                 // 0x01565b40
extern char  kStateDescOther;               // 0x01565b54
extern char  kIID_PathRequest;              // 0x017f243b

// -- vtable slots of the sub-object ---------------------------------------------------------------
static __forceinline Vec3* SubGetPos(void* o)
{
    return ((Vec3* (__thiscall*)(void*))VSLOT(o, 0x2c))(o);
}
static __forceinline void SubSetPathFloat(void* o, float value, int flag)
{
    ((void (__thiscall*)(void*, float, int))VSLOT(o, 0xc4))(o, value, flag);
}
static __forceinline void SubSetPathFloatVt(void** vtbl, void* o, float value, int flag)
{
    ((void (__thiscall*)(void*, float, int))vtbl[0xc4 / 4])(o, value, flag);
}
static __forceinline void SubBeginPath(void* o, const Vec3* heading, float dt, float scale, int flag)
{
    ((void (__thiscall*)(void*, const Vec3*, float, float, int))VSLOT(o, 0xe0))(o, heading, dt, scale, flag);
}
static __forceinline void* OuterSlot(SimOuter* o, int off, const char* id)
{
    return ((void* (__thiscall*)(void*, const char*))VSLOT(o, off))(o, id);
}
static __forceinline SimOuter* Owner(void* self)
{
    return (SimOuter*)((unsigned char*)self - 0x34);
}

// -- layout of the sub-object (only the fields this update touches) --------------------------------
struct SimSub {
    unsigned char mPathBase[0xd4];          // +0x000
    unsigned char mPath[0xd4];              // +0x0d4
    Vec3          mSurfacePos;              // +0x1a8
    Vec3          mHeading;                 // +0x1b4
    float         mZeroA;                   // +0x1c0
    float         mZeroB;                   // +0x1c4
    unsigned char mPad1c8[0x270 - 0x1c8];
    int           mStep;                    // +0x270
    unsigned char mPad274[0x4b8 - 0x274];
    unsigned char mCurveArgs[0x18];         // +0x4b8
    unsigned char mPad4d0[0xae8 - 0x4d0];
    int           mMode;                    // +0xae8
    unsigned char mPadAec[0xb6c - 0xaec];
    SimTimer      mTimer;                   // +0xb6c
    int           mGroundState;             // +0xb94
    unsigned char mPadB98[0xcf0 - 0xb98];
    int           mPathIndex;               // +0xcf0
    unsigned char mPadCf4[0xcfc - 0xcf4];
    int           mFlag;                    // +0xcfc
    Vec3          mDest;                    // +0xd00

    void Execute(const Vec3& heading, float dt);
};

// @ 0x00cad880
void SimSub::Execute(const Vec3& heading, float dt)
{
    SimOuter* outer = Owner(this);

    outer->Tick();

    Vec3* p = SubGetPos(this);
    Vec3 surface;
    surface.x = p->x;
    surface.y = p->y;
    surface.z = p->z;
    if (mMode == 2)
    {
        Vec3 extra;
        Planet* model = (Planet*)PlanetModelSplit(&extra, &surface);
        surface = *model->ProjectToSurface();
    }

    Vec3 request;
    request.x = heading.x;
    request.y = heading.y;
    request.z = heading.z;
    mSurfacePos = surface;
    mHeading = request;
    mZeroB = 0.0f;
    mZeroA = 0.0f;
    mFlag = 0;
    mDest = request;

    if (!outer->BeginPath(request))
    {
        SubBeginPath(this, &request, dt, kPathTime, 0);
        return;
    }

    if (mGroundState == 2)
    {
        if (outer->GroundQuery(request, 0.0f, dt))
        {
            void** vtbl = *(void***)this;
            SubSetPathFloatVt(vtbl, this, outer->PathScale(), 0);
            return;
        }
    }

    mStep = 4;

    SimOuter* owner = Owner(this);
    if (owner)
    {
        void* iface = OuterSlot(owner, 0x0c, &kIID_PathRequest);
        if (iface)
        {
            Sim_1021300();
            InterfaceNotify(iface);
        }
    }

    const void* desc;
    switch (mMode)
    {
    case 1:  desc = &kStateDescOne;   break;
    case 2:  desc = &kStateDescTwo;   break;
    default: desc = &kStateDescOther; break;
    }

    SubSetPathFloat(this, 0.0f, 0);

    void* path = Owner(this) ? (void*)&mPath[0] : 0;
    SimCurve* curve = (SimCurve*)CreateCurve(desc, &surface, &request, &mCurveArgs,
                                             path, mGroundState == 1, mPathIndex);
    curve->Apply();

    mGroundState = 0;
    mTimer.Stop();
}
