// s0101a330 -- cSPSpaceCameraController::FUN_0101a330, per-frame space camera update
//   (flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-)
#include <math.h>
#include "sse_helpers.h"

typedef unsigned int  u32;
typedef unsigned char u8;
typedef unsigned short u16;

struct Vector3 { float x, y, z; };
struct Vector4 { float x, y, z, w; };

struct Matrix33 {
    float m[9];
    void Assign(const Matrix33& o);      // 0x0041cb40 (out of line)
};

// payload handed to the camera pose sink (0x38 bytes on the stack)
struct CameraPose {
    u16      mKind;      // +0x00
    u16      mSeq;       // +0x02
    Vector4  mPos;       // +0x04
    Matrix33 mBasis;     // +0x14
};

struct IPoseSink {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void OnPoseA(CameraPose* p);     // +0x18
    virtual void v1c();
    virtual void OnPoseB(CameraPose* p);     // +0x20
};
struct PoseHost {
    u8         pad00[0x2c];
    IPoseSink* mSink;                        // +0x2c
};

struct MessageServerT {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void Post(u32 id, void* msg, u32 extra);  // +0x14
};

MessageServerT* __cdecl MessageServer();                 // 0x0067dcc0
PoseHost*       __cdecl PoseHostFn();                    // 0x00b3d470
int             __cdecl FUN_01021080();                  // 0x01021080 (universe context)
Matrix33        __cdecl Matrix33FromAxisAngle(const Vector3& axis, float angle); // 0x00453b20

extern float     g_16dda3c;
extern float     g_16dd3d0;
extern float     g_16dd3b4;
extern float     g_16dd384;
extern float     g_16dd374;
extern float     g_16dda64;
extern float     g_16dda68;
extern float     g_16dda6c;
extern Matrix33  g_16dda40;
extern const Vector3 kWorldUp;                           // 0x015b6df4 (0,0,1)

struct cSPSpaceCameraController {
    u8      pad00[0x68];
    Vector3 m68;        // +0x68 facing/input angles
    Vector3 m74;        // +0x74
    float   m80;        // +0x80
    u32     m84;        // +0x84
    u32     m88;        // +0x88
    Vector3 m8c;        // +0x8c
    Vector3 m98;        // +0x98
    Vector3 ma4;        // +0xa4
    float   mb0;        // +0xb0
    float   mb4;        // +0xb4
    float   mb8;        // +0xb8
    float   mbc;        // +0xbc
    float   mc0;        // +0xc0
    float   mc4;        // +0xc4
    float   mc8;        // +0xc8
    float   mcc;        // +0xcc
    float   md0;        // +0xd0
    u8      md4[7];     // +0xd4

    float FUN_010175b0(int mode, float value);   // 0x010175b0 (thiscall, ret 8)
    float FUN_01017410(float a);                 // 0x01017410 (thiscall, ret 4)
    void  FUN_0101a330(unsigned long long ticks);
};

static __forceinline Matrix33 Mul33(const Matrix33& a, const Matrix33& b)
{
    Matrix33 r;
    r.m[0] = a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6];
    r.m[1] = a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7];
    r.m[2] = a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8];
    r.m[3] = a.m[3] * b.m[0] + a.m[4] * b.m[3] + a.m[5] * b.m[6];
    r.m[4] = a.m[3] * b.m[1] + a.m[4] * b.m[4] + a.m[5] * b.m[7];
    r.m[5] = a.m[3] * b.m[2] + a.m[4] * b.m[5] + a.m[5] * b.m[8];
    r.m[6] = a.m[6] * b.m[0] + a.m[7] * b.m[3] + a.m[8] * b.m[6];
    r.m[7] = a.m[6] * b.m[1] + a.m[7] * b.m[4] + a.m[8] * b.m[7];
    r.m[8] = a.m[6] * b.m[2] + a.m[7] * b.m[5] + a.m[8] * b.m[8];
    return r;
}

// @ 0x0101a330
void cSPSpaceCameraController::FUN_0101a330(unsigned long long ticks)
{
    float time = (float)ticks;
    float dt = time * 0.001f;
    int ctx = FUN_01021080();

    float lo = 0.0f;
    if (ctx == 1)
        lo = g_16dd3d0;
    else if (ctx == 2)
        lo = g_16dd384;

    float hi = 0.0f;
    if (ctx == 1)
        hi = g_16dd3b4;
    else if (ctx == 2)
        hi = g_16dd374;

    float prev = m68.x;
    float bound = (m68.x > mb0) ? hi : lo;
    if (mb0 != bound)
        m80 = (m68.x - bound) * m80 / (mb0 - bound);

    float mapped = FUN_010175b0(ctx, prev);
    float limit = (ctx == 2) ? 3.14159274f * 0.75f : 1.57079637f;
    m68.z = SseClamp(m80 + mapped, 0.1f, limit);

    if (md0 < 1.0f) {
        md0 += dt;
        float m = SseClamp(md0, 0.0f, 1.0f) / 1.0f;
        float s = (3.0f - 2.0f * m) * m * m;

        float yaw = fmodf(g_16dda3c, mcc - mc8);
        if (yaw <= 3.14159274f)
            yaw -= g_16dda3c;
        else if (yaw < -3.14159274f)
            yaw += g_16dda3c;
        m68.y = yaw * s + mc8;

        float pitch = fmodf(g_16dda3c, mc4 - mc0);
        if (pitch <= 3.14159274f)
            pitch -= g_16dda3c;
        else if (pitch < -3.14159274f)
            pitch += g_16dda3c;
        m68.z = pitch * s + mc0;
        m80 = m68.z - mapped;
    }
    else if (md4[0]) {
        md4[0] = 0;
        MessageServer()->Post(0x580d038u, 0, 0);
    }

    if (ctx == 2) {
        float delta = FUN_01017410(prev) * time;
        m68.y += delta;

        CameraPose msg;
        msg.mKind = 0;
        msg.mSeq = 0;
        msg.mBasis.Assign(g_16dda40);
        msg.mPos.x = g_16dda64;
        msg.mPos.y = g_16dda68;
        msg.mPos.z = g_16dda6c;
        msg.mPos.w = 1.0f;
        (*(IPoseSink**)((u32)PoseHostFn() + 0x2c))->OnPoseB(&msg);

        Matrix33 rot;
        rot.Assign(Matrix33FromAxisAngle(kWorldUp, delta));
        Matrix33 prod = Mul33(msg.mBasis, rot);
        msg.mBasis = prod;
        msg.mKind |= 2;
        msg.mSeq++;
        (*(IPoseSink**)((u32)PoseHostFn() + 0x2c))->OnPoseA(&msg);
    }

    m68.y = fmodf(6.28318548f, m68.y);
    if (m68.y >= 3.14159274f)
        m68.y -= 6.28318548f;
    mb0 = prev;
    mbc = SseClamp((m68.x - lo) / (hi - lo), 0.0f, 1.0f);
}
