//
// Reimplementation of the 1293-byte routine that seeds an actor's cached
// transform/orientation block (m_v70 .. m_e8) from its property bag and from
// the object it is currently attached to.
//
// Layout notes (retail offsets, from the original code):
//   this+0x00  actor base
//   this+0x1c  float delta
//   this+0x44  float budget (clamped at 0)
//   this+0x48  float3 direction
//   this+0x60  property bag
//   this+0x70  float3 property vector 1
//   this+0x7c  float3 property vector 2
//   this+0x88  float3 property vector 3
//   this+0x94  float3 normalized direction
//   this+0xa0  float3 blended position
//   this+0xac  float3 attached vector
//   this+0xb8  float3 attached vector 2
//   this+0xc4  float3 normalized attached direction
//   this+0xd0  float3 attached frame (a)
//   this+0xdc  float3 attached frame (b)
//   this+0xe8  bool   contact/height flag
//   this+0x100 float  numerator
//   this+0x210 float  denominator
#include "game/types.h"
#include <math.h>
#include "sse_helpers.h"

// ---------------------------------------------------------------------------
// Reference data used by the original (absolute addresses in the retail image).
// ---------------------------------------------------------------------------
extern const float kOne;            // 0x01485720  1.0f
extern const float g_15827bc;       // 0x015827bc
extern const float g_15827fc;       // 0x015827fc
extern const float g_15827b8;       // 0x015827b8
extern const float g_15827c0;       // 0x015827c0
extern const float g_13ec4b8;       // 0x013ec4b8  epsilon
extern float g_169de88;             // 0x0169de88

extern const char kPropA[];         // 0x0169df44
extern const char kPropB[];         // 0x0169df50
extern const char kPropC[];         // 0x0169df5c

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------
struct CVec3
{
    float x, y, z;

    CVec3 Normalized() const;
};

// Small result block returned by two of the attachment virtuals.
struct SFrame
{
    CVec3 a;    // +0x00
    CVec3 b;    // +0x0c
};

// Inlined vector helper: retail materializes its return value in a stack
// temporary and copies 12 bytes into the member.  The scale goes through a
// float array so the three products stay on SSE after the reciprocal is
// spilled, which is how the original is scheduled.
__forceinline CVec3 CVec3::Normalized() const
{
    const CVec3& v = *this;
    float inv = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + g_13ec4b8);
    float scaled[3];
    scaled[0] = v.x * inv;
    scaled[1] = v.y * inv;
    scaled[2] = v.z * inv;
    CVec3 r;
    r.x = scaled[0];
    r.y = scaled[1];
    r.z = scaled[2];
    return r;
}

static __forceinline CVec3 LerpVec(const CVec3& a, const CVec3& b, float t)
{
    CVec3 r;
    r.x = a.x + (b.x - a.x) * t;
    r.y = a.y + (b.y - a.y) * t;
    r.z = a.z + (b.z - a.z) * t;
    return r;
}

// Attachment interface embedded at object + 0xc0. Only the slots the routine
// actually uses are named; the rest keep the retail slot offsets.
struct IAttachment
{
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28();
    virtual CVec3* GetDirection();          // +0x2c
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58();
    virtual CVec3* GetVector(CVec3* out);   // +0x5c
    virtual void v60(); virtual void v64();
    virtual SFrame* GetFrame();             // +0x68
    virtual SFrame* GetFrame2(SFrame* out); // +0x6c

    CVec3* GetLocalVector();                // 0x00d20610
};

// The attached object; the attachment interface starts at +0xc0.
struct CObject
{
    char pad00[0xc0];
    IAttachment attachment;                 // +0xc0

    bool IsActive();                        // 0x00c0b770
};

struct CManager
{
    CObject* GetObject();                   // 0x00b1fdb0
};

struct CDistanceSource
{
    float GetDistance();                    // 0x00b7e390
};

extern "C" CManager* GetActorManager();     // 0x00b3d300
extern "C" CDistanceSource* GetDistanceSource(); // 0x00b3d350
extern "C" CVec3* GetPropertyVector(CVec3* out, const char* key, void* bag); // 0x0059aed0

// Actor class owning the cached block.  Member functions below are the
// retail helper routines; only their call sites are reproduced here.
struct CGonzagoActor
{
    char pad00[0x1c];
    float m_f1c;                            // +0x1c
    char pad20[0x24];
    float m_f44;                            // +0x44
    CVec3 m_v48;                            // +0x48
    char pad54[0xc];
    char m_props[0x10];                     // +0x60
    CVec3 m_v70;                            // +0x70
    CVec3 m_v7c;                            // +0x7c
    CVec3 m_v88;                            // +0x88
    CVec3 m_v94;                            // +0x94
    CVec3 m_vA0;                            // +0xa0
    CVec3 m_vAC;                            // +0xac
    CVec3 m_vB8;                            // +0xb8
    CVec3 m_vC4;                            // +0xc4
    CVec3 m_vD0;                            // +0xd0
    CVec3 m_vDC;                            // +0xdc
    bool m_bE8;                             // +0xe8
    char padE9[0x17];
    float m_f100;                           // +0x100
    char pad104[0x10c];
    float m_f210;                           // +0x210

    void SubD270A0(CVec3* out);             // 0x00d270a0
    void SubD271D0(CVec3* out);             // 0x00d271d0

    void Update();
};

// @ 0x00d27300 CGonzagoActor::Update
void CGonzagoActor::Update()
{
    CVec3 off;
    // One 24-byte scratch object serves as the property-bag return slot, the
    // first offset position and the GetFrame2 result, as in the original.
    SFrame frame;
    CVec3* v;

    // Three cached vectors out of the property bag.
    v = GetPropertyVector(&frame.a, kPropA, m_props);
    m_v70.x = v->x;
    m_v70.y = v->y;
    m_v70.z = v->z;

    v = GetPropertyVector(&frame.a, kPropB, m_props);
    m_v7c.x = v->x;
    m_v7c.y = v->y;
    m_v7c.z = v->z;

    v = GetPropertyVector(&frame.a, kPropC, m_props);
    m_v88.x = v->x;
    m_v88.y = v->y;
    m_v88.z = v->z;

    // Normalize the actor direction.
    m_v94 = m_v48.Normalized();

    CManager* mgr = GetActorManager();
    CObject* obj = mgr->GetObject();
    if (!obj)
        return;

    IAttachment* at = &obj->attachment;

    // Normalize the attachment direction.
    m_vC4 = at->GetDirection()->Normalized();

    SubD270A0(&frame.a);
    SubD271D0(&frame.a);

    // First offset: attached frame height scaled by a constant.
    float blend = obj->IsActive() ? kOne : g_15827bc;
    SFrame* fh0 = at->GetFrame();
    CVec3& b0 = fh0->b;     // keeping the reference reproduces the original's
    float k = b0.z * g_15827fc;  // dead `add eax,0xc` address computation

    off.x = m_vC4.x * k;
    off.y = m_vC4.y * k;
    off.z = m_vC4.z * k;

    CVec3* base = at->GetDirection();
    frame.a.x = base->x + off.x;
    frame.a.y = base->y + off.y;
    frame.a.z = base->z + off.z;

    // Second offset uses the blended scale.
    float k2 = k * blend + g_15827b8;
    off.x = m_vC4.x * k2;
    off.y = m_vC4.y * k2;
    off.z = m_vC4.z * k2;

    CVec3* base2 = at->GetDirection();
    CVec3 p1;
    p1.x = base2->x + off.x;
    p1.y = base2->y + off.y;
    p1.z = base2->z + off.z;

    float t = SseClamp(m_f100 / m_f210 - g_15827c0, 0.0f, 1.0f);

    m_vA0 = LerpVec(p1, frame.a, t);

    // Height of the attachment vs. the clearance to the nearest point.
    SFrame* fh = at->GetFrame();
    float height = fh->b.z - fh->a.z;
    float dist = GetDistanceSource()->GetDistance();
    CVec3* q = at->GetDirection();
    float reach2 = q->x * q->x + q->y * q->y + q->z * q->z;
    float clear;
    if (dist * dist > reach2)
        clear = dist - sqrtf(reach2);
    else
        clear = 0.0f;
    m_bE8 = (clear > height);

    m_vAC = *at->GetLocalVector();

    m_vB8 = *at->GetVector(&frame.a);

    SFrame* fr = at->GetFrame2(&frame);
    m_vDC = fr->b;
    m_vD0 = fr->a;

    m_f44 -= m_f1c;
    if (m_f44 < 0.0f)
        m_f44 = 0.0f;

    g_169de88 -= m_f1c;
    if (g_169de88 < 0.0f)
        g_169de88 = 0.0f;
}
