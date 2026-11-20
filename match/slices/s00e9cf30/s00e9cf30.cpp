// s00e9cf30: SP::cSPUFOGfx::UpdateRadarEffect -- thiscall, ret 4.
// Drives the UFO's radar return effect: fades it with the distance between the UFO and the radar
// sweep centre, restarts it after its interval, and positions it with a Transform built from the
// sweep direction.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (no /EHsc).
#include "types.h"

#pragma warning(disable: 4100)

extern "C" double __cdecl fabs(double);
extern "C" double __cdecl sqrt(double);
#pragma intrinsic(fabs)
#pragma intrinsic(sqrt)

struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
    Vector3(float a, float b, float c) : x(a), y(b), z(c) {}
};
struct Quaternion { float x, y, z, w; };
struct Matrix3 {
    float m[9];
    Matrix3& Assign(const Matrix3& o);          // 0x0041cb40, thiscall ret 4
};
struct Vec3POD { float x, y, z; };

// ---- globals used by the effect ---------------------------------------------------------------
extern const Vector3 kRadarCenter;              // 0x016c6350
extern const float   kExactlyOne;               // 0x01485720 (1.0f)
extern const float   kSweepAngle;               // 0x015a8af8
extern const char    kSpaceGameName[];          // 0x016c6858

// ---- opaque callees (address in the comment) --------------------------------------------------
struct cPlayerUFO;
struct cUFOData;
struct cSpaceGame;
struct cPlayerInventory;

struct cSpaceGame {
    cPlayerInventory* GetPlayerInventory();     // 0x00a1ad60
    cUFOData*         GetUFOData();             // 0x00a1ad60
};
struct cPlayerInventory {
    cPlayerUFO* GetPlayerUFO();                 // 0x00ff3d40
    bool        IsRadarActive();                // 0x00ff3bb0
};
struct cPlayerUFO {
    float GetParam0();                          // 0x0104be20
    float GetParam1();                          // 0x0104be30
    float GetParam2();                          // 0x00ce6940
    float GetParam3();                          // 0x0104bdc0
};
cSpaceGame* __stdcall GetSimulator(const char* name);   // 0x01002bd0
cSpaceGame* GetPlayerContext();                 // 0x00ffbe50
int         GetUniverseContext();               // 0x01021080

// Closest radar return to `from` around `center`; false when nothing is in range.
bool sGetClosestRadarObject(const Vector3* from, const Vector3* center, float radius);   // 0x00e9cb50

// The effect slot helper takes the slot's address in ECX and returns the out parameter to hand to
// cEffectFactory::CreateEffect (0x00a16f40 is a thiscall on the slot itself).
struct cEffectSlot {
    void** GetCreateOut();
};

// ---- effect interfaces ------------------------------------------------------------------------
struct Transform {
    int16_t mnFlags;                            // +0x00
    int16_t mnTransformCount;                   // +0x02
    Vector3 mOffset;                            // +0x04
    float   mfScale;                            // +0x10
    Matrix3 mRotation;                          // +0x14
    Transform();                                // 0x00434040
    Transform& SetOffset(const Vector3& v) { mOffset = v; mnFlags |= 4; mnTransformCount++; return *this; }
    Transform& SetRotation(const Matrix3& r) { mRotation = r; mnFlags |= 2; mnTransformCount++; return *this; }
};

struct cEffect {
    virtual void v00();                         // +0x00
    virtual void Release();                     // +0x04
    virtual void Start(int flag);               // +0x08
    virtual void Play(int flag);                // +0x0c
    virtual void v04();                         // +0x10
    virtual void SetTransform(const Transform* xf);   // +0x14
};

struct cEffectManager {                         // 0x0067dcc0
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04();
    virtual void SetParam(uint32_t id, const float* value, int flag);   // +0x14
};
struct cEffectFactory {                         // 0x0067ddd0
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v0a();
    virtual void CreateEffect(uint32_t id, int flag, void** out);       // +0x2c
};
cEffectManager* GetEffectManager();             // 0x0067dcc0
cEffectFactory* GetEffectFactory();             // 0x0067ddd0

void  BuildBasis(Matrix3* out, const Vector3* a, const Vector3* b);     // 0x0069b440
void  RotateAboutAxis(Matrix3* out, const Vector3* axis, float angle); // 0x00576b00
Matrix3* Multiply(Matrix3* out, const Matrix3* a, const Matrix3* b);    // 0x0041de20

struct cTimer {                                 // cSPUFOGfx::mRadarTimer, +0x58, 0x20 bytes
    bool               HasStarted();            // 0x00feba90
    void               Restart();               // 0x00bc3130
    unsigned long long GetElapsedTime();        // 0x00bc3190
    uint8_t pad[0x20];
};

struct cUFOData {
    uint32_t pad00[0x718 / 4];
    Vector3  mRadarPos;                         // +0x718
};

struct cSPGameDataUFO {
    uint32_t pad00[0x714 / 4];
    int      mMode;                             // +0x714
    Vector3  mRadarPos;                         // +0x718
};

// ---- SP::cSPUFOGfx ---------------------------------------------------------------------------
class cSPUFOGfx {
public:
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v0a();
    virtual void* GetViewData();                // +0x2c
    uint32_t pad04[2];
    void*    mpObject;                          // +0x0c
    uint32_t pad10[2];
    Vector3  mColor;                            // +0x18
    uint32_t pad24[(0x30 - 0x24) / 4];
    bool     mbFlag30;                          // +0x30
    bool     mbFlag31;                          // +0x31
    uint8_t  pad32[0x58 - 0x32];
    cTimer   mRadarTimer;                       // +0x58
    bool     mbEnabled;                         // +0x78
    uint8_t  pad79[3];
    void*    mRingEffect;                       // +0x7c
    uint32_t pad80[(0x90 - 0x80) / 4];
    void*    mAOERepairEffect;                  // +0x90
    void*    mRallyCallEffect;                  // +0x94
    cEffect* mRadarEffect;                      // +0x98
    uint32_t pad9C[(0xb8 - 0x9c) / 4];
    void*    mpAudio;                           // +0xb8

    void UpdateRadarEffect(cSPGameDataUFO* ufo);    // 0x00e9cf30
};

// @ 0x00e9cf30
void cSPUFOGfx::UpdateRadarEffect(cSPGameDataUFO* ufo)
{
    if (!ufo || ufo->mMode != 0)
        return;

    cPlayerUFO* player = GetSimulator(kSpaceGameName)->GetPlayerInventory()->GetPlayerUFO();

    if (GetUniverseContext() && mRadarEffect) {
        mRadarEffect->Play(1);
        if (mRadarEffect) {
            cEffect* old = mRadarEffect;
            mRadarEffect = 0;
            old->Release();
        }
    }

    if (!GetSimulator(kSpaceGameName)->GetPlayerInventory()->IsRadarActive())
        return;
    if (GetUniverseContext())
        return;

    float inner = player->GetParam0();          // fade inner radius
    float outer = player->GetParam1();          // radar radius
    float start = player->GetParam2();          // fade interval (start)
    float end   = player->GetParam3();          // fade interval (end)

    Vector3 pos = GetPlayerContext()->GetUFOData()->mRadarPos;
    Vector3 center = kRadarCenter;

    if (!sGetClosestRadarObject(&pos, &center, outer))
        return;

    if (!mRadarTimer.HasStarted())
        mRadarTimer.Restart();

    float dx = pos.x - center.x;
    float dy = pos.y - center.y;
    float dz = pos.z - center.z;
    float dist = (float)sqrt(dx * dx + dy * dy + dz * dz);

    if (dist < outer) {
        float t = ((inner > dist ? inner : dist) - inner) / (outer - inner);
        float alpha = kExactlyOne - t;

        GetEffectManager()->SetParam(0x5cbe8ff, &alpha, 0);

        if ((double)mRadarTimer.GetElapsedTime() > (double)(start + (end - start) * t)) {
            mRadarTimer.Restart();
            if (mRadarEffect) {
                mRadarEffect->Play(1);
                if (mRadarEffect) {
                    cEffect* old = mRadarEffect;
                    mRadarEffect = 0;
                    old->Release();
                }
            }
            GetEffectFactory()->CreateEffect(0x687d824e, 0,
                reinterpret_cast<cEffectSlot*>(&mRadarEffect)->GetCreateOut());
            if (mRadarEffect)
                mRadarEffect->Start(0);
        }
    }

    if (!mRadarEffect)
        return;

    Transform xf;
    xf.SetOffset(pos);
    float ddx = center.x - pos.x;
    float ddy = center.y - pos.y;
    float ddz = center.z - pos.z;
    center.x = ddx;
    center.y = ddy;
    center.z = ddz;
    Matrix3 basis;
    BuildBasis(&basis, &center, &pos);
    Vector3& axis = center;                     // the sweep axis reuses the dead centre slot
    axis.x = kExactlyOne;
    axis.y = 0.0f;
    axis.z = 0.0f;
    Matrix3 spin;
    RotateAboutAxis(&spin, &axis, -kSweepAngle);
    Matrix3 tmp;
    Matrix3* rot = Multiply(&tmp, &spin, &basis);
    xf.SetRotation(*rot);
    mRadarEffect->SetTransform(&xf);
}
