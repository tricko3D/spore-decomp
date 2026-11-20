// Heading update with a limited turn rate.
//
// The object keeps a heading quaternion (+0x60) and a forward axis (+0x70).
// Each update:
//   * rotates the forward axis about the roll axis (+0x94) by the roll angle
//     (+0x34) to get the "up" axis used to build the new attitude,
//   * picks a target point (one of two, depending on the current game mode),
//   * measures the angle between the forward axis and the direction to that
//     target,
//   * if the angle is larger than the allowed turn (turn rate +0x1c scaled by
//     a constant), slerps towards the target quaternion by limit/angle;
//     otherwise it snaps straight to it.
// The resulting quaternion is written to +0x154.

// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast

#include <math.h>
#include "sse_helpers.h"
#pragma intrinsic(fabs, sqrt)

struct cVec3_s00d2_7810 {
    float x, y, z;
};

struct cQuat_s00d2_7810 {
    float x, y, z, w;
};

inline cVec3_s00d2_7810 operator-(const cVec3_s00d2_7810& a, const cVec3_s00d2_7810& b)
{
    cVec3_s00d2_7810 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

// The turn-rate scale is read from a data float (0x0169df1c), not a literal:
// its value is a runtime global, so it keeps its own declaration.
extern const float kTurnRateScale; // 0x0169df1c

void* __cdecl GetCurrentGameMode();                                        // 0x00b5b800
cVec3_s00d2_7810* __cdecl RotateVectorByAxisAngle(cVec3_s00d2_7810* pOut,
                                                  const cVec3_s00d2_7810* pAxis,
                                                  const cVec3_s00d2_7810* pVector,
                                                  float fAngle);            // 0x00d21d10
cQuat_s00d2_7810* __cdecl QuaternionFromFacingAndUp(cQuat_s00d2_7810* pOut,
                                                    const cVec3_s00d2_7810* pFacing,
                                                    const cVec3_s00d2_7810* pUp); // 0x0069b600
cQuat_s00d2_7810 __cdecl Slerp(const cQuat_s00d2_7810& from,
                               const cQuat_s00d2_7810& to, float t);        // 0x005b26f0

class cHeading_s00d2_7810 {
public:
    void UpdateHeading(bool bLimitTurn);              // @ 0x00d27810

private:
    unsigned char mnPad000[0x1c];                     // +0x000
    float mfTurnRate;                                 // +0x01c
    unsigned char mnPad020[0x14];                     // +0x020
    float mfRollAngle;                                // +0x034
    unsigned char mnPad038[0x28];                     // +0x038
    cQuat_s00d2_7810 mHeading;                        // +0x060
    cVec3_s00d2_7810 mForward;                        // +0x070
    unsigned char mnPad07c[0x18];                     // +0x07c
    cVec3_s00d2_7810 mRollAxis;                       // +0x094
    unsigned char mnPad0a0[0x65];                     // +0x0a0
    bool mbDisableTurnLimit;                          // +0x105
    unsigned char mnPad106[0x0e];                     // +0x106
    cVec3_s00d2_7810 mPosition;                       // +0x114
    unsigned char mnPad120[0x1c];                     // +0x120
    cVec3_s00d2_7810 mTargetDefault;                  // +0x13c
    unsigned char mnPad148[0x0c];                     // +0x148
    cQuat_s00d2_7810 mHeadingResult;                  // +0x154
    bool mbUseModeTarget;                             // +0x164
    unsigned char mnPad165[0x03];                     // +0x165
    cVec3_s00d2_7810 mTargetMode;                     // +0x168
};

// @ 0x00d27810
void cHeading_s00d2_7810::UpdateHeading(bool bLimitTurn)
{
    const float fTurnLimit = mfTurnRate * kTurnRateScale;

    float fInvLength;
    float fAngle;
    cVec3_s00d2_7810 direction;
    cVec3_s00d2_7810 up;

    RotateVectorByAxisAngle(&up, &mRollAxis, &mForward, mfRollAngle);

    const bool bModeTarget = GetCurrentGameMode() == (void*)0x01654c05;
    const cVec3_s00d2_7810* pTarget =
        (mbUseModeTarget && bModeTarget) ? &mTargetMode : &mTargetDefault;

    const cVec3_s00d2_7810 delta = mPosition - *pTarget;

    // |delta|^2 + 1e-8, accumulated z..x; the squares are promoted to double
    // for the x87 sqrt, as in the retail build.
    double fLengthSq = (double)delta.z * delta.z;
    fLengthSq += (double)delta.y * delta.y;
    fLengthSq += (double)delta.x * delta.x;
    fLengthSq += 1e-8f;
    fInvLength = 1.0f / (float)sqrt(fLengthSq);

    // The retail build scales the direction and accumulates the dot product
    // interleaved, scaling z, x, y.
    direction.z = delta.z * fInvLength;
    float fDot = mForward.z * direction.z;
    direction.x = delta.x * fInvLength;
    fDot += mForward.x * direction.x;
    direction.y = delta.y * fInvLength;
    fDot += mForward.y * direction.y;
    fDot = SseClamp(fDot, -1.0f, 1.0f);

    fAngle = (float)acos((double)fDot);

    // Written negated: `fabs(fAngle) > fTurnLimit` makes the compiler swap the
    // x87 comparison operands (fxch), which retail does not do.
    if (!mbDisableTurnLimit && bLimitTurn && !(fabsf(fAngle) <= fTurnLimit)) {
        cQuat_s00d2_7810 qTarget;
        QuaternionFromFacingAndUp(&qTarget, &direction, &up);

        const float fT = SseClamp(fTurnLimit / fAngle, 0.0f, 1.0f);
        mHeadingResult = Slerp(mHeading, qTarget, fT);
        return;
    }

    cQuat_s00d2_7810 qHeading;
    QuaternionFromFacingAndUp(&qHeading, &direction, &up);
    mHeadingResult = qHeading;
}
