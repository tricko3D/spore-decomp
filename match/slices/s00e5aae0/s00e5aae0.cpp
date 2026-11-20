// Slice s00e5aae0: ray / horizontal-plane intersection for the cell view (SPGonzago, SPGonzago.obj).
// The original asks the app for the current camera ray, then intersects it with the plane through
// 0x016b3c28 whose normal is the world up vector 0x015a7c40, keeping only hits in front of the eye.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-  (no EH frame, no security cookie in the original)
//
// The original is a file-local helper (its `s` prefix is the same as `sBlockIsInRange` in the
// neighbouring SPCellGame source): cl 15 gives a `static` function that a same-TU caller reaches a
// custom register calling convention (docs/codegen.md section 2, "static/register calling
// conventions in a TU").  Retail confirms it -- the out pointer arrives in ESI, the function has no
// prologue and saves no register (`movss [esi]`).  The helper below is therefore `static`, and the
// small caller after it keeps it out of line under that convention; with it the whole prologue,
// the ESI stores and the return path are byte-identical to retail.
//
// What still differs is scheduling and register allocation.  Retail keeps the plane-offset dot
// before the `den == 0` test (it is the first block after the camera call), while cl sinks it into
// the taken path: the plane offset is a single-use temporary that cl substitutes into the numerator
// and then evaluates inside the branch.  `_ReadWriteBarrier()` after the plane offset pins it in
// the entry block (cl may not move its loads down), which reproduces retail's block order and the
// sharing of the up-vector loads; what remains is that cl then keeps the two float constants
// (the 0x01485378 zero and the sign mask) in xmm registers where retail uses them as memory
// operands, and that the origin components stay in memory instead of in registers.  Those are
// allocator choices that no source spelling of this function seems to reach.
#include "game/types.h"
#include <intrin.h>

struct Vec3 { float x, y, z; };

// unnamed retail globals (address in the comment)
extern const float kf15a7c40, kf15a7c44, kf15a7c48;   // 0x015a7c40 world up vector
extern const float kf16b3c28, kf16b3c2c, kf16b3c30;   // 0x016b3c28 a point on the plane
extern const float kf1485378;                          // 0x01485378 0.0f (the original compares against
                                                       // this global from memory, twice)

// callees (declaration-only placeholders: the call is `call rel32` with `this` in ecx)
void* App();                                           // 0x0067dd10
struct CCamera {
    void GetRay(Vec3* pOrigin, Vec3* pDirection);      // 0x007c4900
};
// App()'s vtable entry +0x58 hands out that camera.
struct IApp {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21();
    virtual CCamera* GetCamera();
};

// @ 0x00e5aae0
static bool sCellRayToPlane(Vec3* pOut)
{
    Vec3 dir, origin;
    IApp* pApp = (IApp*)App();
    CCamera* pCamera = pApp->GetCamera();
    pCamera->GetRay(&origin, &dir);

    // The up vector and the plane point are read once here; retail loads them in this block and
    // reuses the up components in all three dots.
    const float upx = kf15a7c40, upy = kf15a7c44, upz = kf15a7c48;

    // the plane is dot(x, up) == dot(planePoint, up): retail forms this offset first, before the
    // parallel test.  cl would sink it into the taken path, the barrier holds it here.
    float planeD = -(kf16b3c28 * upx + kf16b3c2c * upy + kf16b3c30 * upz);
    _ReadWriteBarrier();

    // dot(direction, up): zero means the ray runs parallel to the plane
    float den = dir.x * upx + dir.y * upy + dir.z * upz;
    if (den == kf1485378) return false;

    float t = -((origin.x * upx + origin.z * upz + origin.y * upy + planeD) / den);
    // behind the eye: no hit
    if (t < kf1485378) return false;

    pOut->x = origin.x + dir.x * t;
    pOut->y = origin.y + dir.y * t;
    pOut->z = origin.z + dir.z * t;
    return true;
}

// The one thing a C++ translation unit needs so the compiler emits the helper above out of line
// with the file-local register convention retail used (an address-taken pointer would force the
// declared cdecl convention instead).
float gCellRayProbe[3];
void CellRayProbe()
{
    Vec3 hit;
    if (sCellRayToPlane(&hit)) {
        gCellRayProbe[0] = hit.x;
        gCellRayProbe[1] = hit.y;
        gCellRayProbe[2] = hit.z;
    }
}
