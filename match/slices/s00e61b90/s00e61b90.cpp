// Slice s00e61b90 -- SPGonzago/SPCellGame.obj (cell stage): spawn one visual effect that
// points from a position toward a target (or along the cell view direction when there is
// no target), hand it a cSPTransform built from that facing, start it and release it.
// The retail routine is an internal helper of the cell TU (two arguments in registers, the
// other two on the stack), so it is kept `static` here with same-TU callers fixing its
// convention.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast  (cell TU; no /EHsc)
#include "types.h"

typedef uint16_t uint16;
typedef uint32_t uint32;

// ---------------------------------------------------------------- types
struct Vector3 { float x, y, z; };

struct Matrix3 { float m[9]; };

struct cSPTransform {                  // 0x38 bytes, same layout as the s00e6b370 slice
    uint16 mFlags;
    uint16 mModCount;
    float  mTranslation[3];
    float  mScale;
    Matrix3 mRotation;
};

struct cIVisualEffect {                // EA::Swarm::cIVisualEffect
    virtual int  AddRef();                                  // +0x00
    virtual int  Release();                                 // +0x04
    virtual bool Start(int flags);                          // +0x08
    virtual bool Stop(int flags);                           // +0x0c
    virtual void v10();                                     // +0x10
    virtual void v14();                                     // +0x14
    virtual void SetTransform(const cSPTransform* xf);      // +0x18
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void v40(int kind, const void* data, int flag); // +0x40
};

// ---------------------------------------------------------------- globals
extern char* gspCellGfx;              // 0x016b3c08  SP::cCellGfx*
extern float gT_016b3c54[3];          // 0x016b3c54  default transform translation
extern Matrix3 gIdentityMatrix;       // 0x016b3c60  identity 3x3 rotation
extern Vector3 gUp_015a7c40;          // 0x015a7c40  reference "up" axis
extern float gOne;                    // 0x01485720 = 1.0f
extern float gMinPos;                 // 0x014851ec = 1.5258789e-05f (squared-length epsilon)

// ---------------------------------------------------------------- callees
bool CreateEffectSafe(void* world, uint32 id, int flags, cIVisualEffect** ppEffect); // 0x00628450
void Matrix33_ctor(Matrix3* self, const Matrix3* src);                  // 0x0041cb40
float* NormalizeVector(float* out, const float* in);                    // 0x00436ce0
Matrix3 Matrix3FromFacingAndUp(const Vector3* facing, const Vector3* up); // 0x0069b440
Vector3* GetCellViewDirection(Vector3* out);                            // 0x00e5cae0

// @ 0x00e61b90
static void SpawnDirectedCellEffect(const Vector3* pos, const uint32* effectId,
                                   int spaceEffects, const Vector3* target)
{
    cIVisualEffect* fx = 0;
    char* slot = spaceEffects != 0 ? gspCellGfx + 0x161bc : gspCellGfx + 0x161c8;
    CreateEffectSafe(*(void**)slot, *effectId, 0, &fx);

    if (pos != 0) {
        float dx = 0.0f;
        float dy = 0.0f;
        float dz = 0.0f;
        if (target != 0) {
            dx = target->x - pos->x;
            dy = target->y - pos->y;
            dz = target->z - pos->z;
        }

        Vector3 dir;
        Vector3 out;
        Vector3* v;
        if (target != 0 && dz * dz + dx * dx + dy * dy > gMinPos) {
            dir.x = dx;
            dir.y = dy;
            dir.z = dz;
            v = (Vector3*)NormalizeVector(&out.x, &dir.x);
        } else {
            v = GetCellViewDirection(&out);
        }
        dir = *v;

        cSPTransform xf;
        xf.mFlags = 0;
        xf.mModCount = 0;
        xf.mTranslation[0] = gT_016b3c54[0];
        xf.mTranslation[1] = gT_016b3c54[1];
        xf.mTranslation[2] = gT_016b3c54[2];
        xf.mScale = gOne;
        Matrix33_ctor(&xf.mRotation, &gIdentityMatrix);

        xf.mFlags |= 4;
        xf.mModCount++;
        *(Vector3*)xf.mTranslation = *pos;

        Vector3 facing;
        facing.x = -dir.x;
        facing.y = -dir.y;
        facing.z = -dir.z;
        xf.mRotation = Matrix3FromFacingAndUp(&facing, &gUp_015a7c40);

        xf.mFlags |= 2;
        xf.mModCount++;

        fx->SetTransform(&xf);
        fx->Start(0);
        if (target != 0)
            fx->v40(9, target, 1);
    }

    if (fx != 0)
        fx->Release();
}

// Same-TU callers, as in the retail object: a static helper keeps its internal register
// convention only while local call sites exist.
void SpawnCellDirectedEffect(const Vector3* pos, const uint32* effectId,
                             int spaceEffects, const Vector3* target)
{
    SpawnDirectedCellEffect(pos, effectId, spaceEffects, target);
}

void SpawnCellEffectAt(const Vector3* pos, const uint32* effectId, int spaceEffects)
{
    SpawnDirectedCellEffect(pos, effectId, spaceEffects, 0);
}
