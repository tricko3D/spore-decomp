// @ 0x00e6f660
// SP cell TU: take a wander target, (when `last`) snap it onto the world basis plane, then face
// the cell at it (0x00e56f80) and step the cell (0x00e6eec0).
//
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (cell TU; no /EHsc).
//
// Shape notes from the retail code: the target vector and the cell position are 3-float objects
// (retail keeps `v` at [esp+4..0xc] and reserves a second 0xc-byte home after it, frame 0x18), the
// difference is a by-value 3-float object, and both arms write their result into the same three
// registers so the three stores end up merged after the branch join. Retail also keeps the cell
// pointer in ESI across the whole body: 0x00e6eec0 is one of the module's register-ABI callees
// (it reads its `self` from ESI at entry), which only a /GL + link /LTCG build of this TU can
// express -- with the row's plain flags the pointer stays in a scratch register instead.
#include "types.h"
typedef uint32_t uint32;

// Stand-in for the game's 3-float vector class (SP::cSPVector3): a user copy ctor / operator= is
// what makes whole-object copies compile to movss instead of integer moves.
struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
    Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {}
    Vec3& operator=(const Vec3& o) { x = o.x; y = o.y; z = o.z; return *this; }
};

static Vec3 Sub(const Vec3& a, const Vec3& b)
{
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

struct cEntity {
    char  pad0[0x4c];
    float mX;                                 // +0x4c  position
    float mY;                                 // +0x50
    float mZ;                                 // +0x54
};

// World basis rows (three Vector3s, read-only data).
extern const float gBasisX[3];                // 0x015a7c28
extern const float gBasisY[3];                // 0x015a7c34
extern const float gBasisZ[3];                // 0x015a7c40

extern "C" void __cdecl FUN_00e56f80(cEntity* self, float* v);                 // 0x00e56f80 face
// Self is not a stack argument: the callee reads the cell from ESI (register ABI, see above).
extern "C" void __cdecl FUN_00e6eec0(float dt, float f18, float f40,
                                     uint32 hash, int mode);                   // 0x00e6eec0 StepCell

// @ 0x00e6f660
void FUN_00e6f660(cEntity* self, const float* pos, float dt, float f18, float f40,
                  uint32 hash, int mode, unsigned char last)
{
    Vec3 v(pos[0], pos[1], pos[2]);

    if (last) {
        Vec3 p(self->mX, self->mY, self->mZ);
        Vec3 d = Sub(v, p);

        float up  = d.z * gBasisY[2] + d.x * gBasisY[0] + d.y * gBasisY[1];
        float fwd = d.z * gBasisZ[2] + d.x * gBasisZ[0] + d.y * gBasisZ[1];
        if (up > fwd) {
            v = Vec3(p.x + gBasisX[0] * up, p.y + gBasisX[1] * up, p.z + gBasisX[2] * up);
        } else {
            float t = d.z * gBasisX[2] + d.x * gBasisX[0] + d.y * gBasisX[1];
            v = Vec3(p.x + gBasisX[0] * t, p.y + gBasisX[1] * t, p.z + gBasisX[2] * t);
        }
    }

    FUN_00e56f80(self, &v.x);
    FUN_00e6eec0(dt, f18, f40, hash, mode);
}
