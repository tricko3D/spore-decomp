// match/slices/s00c43490/s00c43490.cpp -- SP::cLocomotiveObject::MoveToPointAtStandardSpeed (0x00c43490)
//
// Two ways to reach the same navigation goal are built here, each handed to SetGoal:
//   * bPath != 0: normalise (target - GetPosition()) and build the request with the
//     cNavigationGoal initialiser that takes a reference direction (0x00ad2a50, InitToward);
//   * bPath == 0: build the "go straight at the point" request (the small initialiser, whose
//     body the original compiler inlined here).
// Afterwards the desired speed is reset to the standard speed and the move-status flags clear.
//
// Retail's cLocomotiveObject vtable puts GetPosition at +0x2c, SetDesiredSpeed at +0xc4,
// GetStandardSpeed at +0xcc and SetGoal at +0xdc (see s00c436a0 for the +0xc4/+0xdc pair;
// the developer PDB declaration order is 12 slots lower for the 0x94..0xac group).
//
// Shape notes (pass 4): SetGoal receives a conditional expression, so MSVC builds one
// NavRequest temporary per branch (retail: [esp+0x1c] and [esp+0x90]) and destroys both
// after the call behind the "which was built" bitmask (test bl,2 / and ebx,-3 / test bl,1).
// The same 12-byte local is the (target - GetPosition()) delta and then the normalised
// direction; the products are written inv * d, not d * inv, to get retail's movaps copies.
// Frame is retail's 0xf8; only an extra callee-saved register (ebp, pushed for the zero
// constant where retail uses edi and reuses ebx for the tail vtable) is still different.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (this function's recorded retail row), no /EHsc.
// Note: the flag parser keeps "/EHs" and drops the trailing dash, so verify with
//   check.py 0x00c43490 --flags "/O2 /MD /Gy /TP /EHs- /GS- /arch:SSE /fp:fast /Isrc/game/include"
// /EHsc installs an SEH frame retail does not have, and without /fp:fast sqrtf calls the CRT
// __CIsqrt instead of expanding to the inline fsqrt retail uses.
#include <math.h>
#include "game/types.h"
#include "SP/cLocomotiveObject.h"

namespace SP {

struct NavWaypoint;
struct RequestVec3 { float x, y, z; };

extern float kAlignThreshold;   // 0x013ec4b4 (0.9f)
extern float kNormalizeEpsilon; // 0x013ec4b8

// Retail layout of a navigation-goal request (SP::tNavigationGoal, 0x74 bytes), the same shape
// ClearGoalAndStop (0x00c436a0) builds.
struct NavRequest {
    NavWaypoint*  mpBegin;                 // +0x00  waypoint vector
    NavWaypoint*  mpEnd;                   // +0x04
    NavWaypoint*  mpCapacity;              // +0x08
    char          pad0c[0x14 - 0x0c];
    RequestVec3   mDst;                    // +0x14  next waypoint destination
    float         mGoalStopDistance;       // +0x20
    int           mWaypointType;           // +0x24
    char          pad28[0x4c - 0x28];
    unsigned char mWaypointFlags;          // +0x4c
    char          pad4d[0x50 - 0x4d];
    RequestVec3   mFacing;                 // +0x50  reference direction
    int           mType;                   // +0x5c  (0 none, 1/3 follow, 2 hold)
    float         mAcceptableStopDistance; // +0x60
    float         mField64;                // +0x64  (alignment threshold)
    float         mPrevDistance;           // +0x68
    float         mElapsed;                // +0x6c
    int           mField70;                // +0x70

    // cNavigationGoal::InitToward (0x00ad2a50): destination, reference direction, min/max.
    NavRequest(RequestVec3 const &, RequestVec3 const &, float, float);

    // Go straight at the point: no reference direction, stop on arrival.  (Inlined at the
    // retail call site; it is the goal initialiser with a fixed movement mode and no facing.)
    NavRequest(RequestVec3 const &dest, float stopDistance, float acceptableStopDistance)
    {
        mpBegin = 0;
        mpEnd = 0;
        mpCapacity = 0;
        mDst.x = dest.x;
        mDst.y = dest.y;
        mDst.z = dest.z;
        mGoalStopDistance = stopDistance;
        mWaypointType = 0;
        mWaypointFlags = 0;
        mFacing.x = 0.0f;
        mFacing.y = 0.0f;
        mFacing.z = 0.0f;
        mType = 1;
        mAcceptableStopDistance = acceptableStopDistance;
        mField64 = kAlignThreshold;
        mPrevDistance = 3.402823466e+38f;
        mElapsed = 0.0f;
        mField70 = 0;
    }

    // frees the waypoint vector's block through its array cookie
    ~NavRequest()
    {
        if (mpBegin && ((int*)mpBegin)[-1] != 0)
            operator delete[](mpBegin);
    }
};

// cLocomotiveObject with the retail virtual order (only the slots this function calls are named).
class cLocoRetail {
 public:
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28();
    virtual RequestVec3 const & GetPosition();              // +0x2c
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58(); virtual void v5c();
    virtual void v60(); virtual void v64(); virtual void v68(); virtual void v6c();
    virtual void v70(); virtual void v74(); virtual void v78(); virtual void v7c();
    virtual void v80(); virtual void v84(); virtual void v88(); virtual void v8c();
    virtual void v90(); virtual void v94(); virtual void v98(); virtual void v9c();
    virtual void va0(); virtual void va4(); virtual void va8(); virtual void vac();
    virtual void vb0(); virtual void vb4(); virtual void vb8(); virtual void vbc();
    virtual void vc0();
    virtual void SetDesiredSpeed(float speed, int flag);    // +0xc4
    virtual void vc8();
    virtual float GetStandardSpeed();                       // +0xcc
    virtual void vd0(); virtual void vd4(); virtual void vd8();
    virtual void SetGoal(NavRequest const & goal);          // +0xdc

    char          mPad04[0x270 - 0x04];
    unsigned int  mMoveStatusFlags;                         // +0x270
};

// @ 0x00c43490
void cLocomotiveObject::MoveToPointAtStandardSpeed(cSPVector3 const &target, float speed,
                                                   float stopDistance, bool bPath)
{
    cLocoRetail* loco = (cLocoRetail*)this;
    RequestVec3 const &dest = *(RequestVec3 const*)&target;
    float inv = 0.0f;
    RequestVec3 dir;

    if (bPath) {
        RequestVec3 const &pos = loco->GetPosition();
        dir.x = dest.x - pos.x;
        dir.y = dest.y - pos.y;
        dir.z = dest.z - pos.z;
        inv = 1.0f / sqrtf((dir.y * dir.y + dir.x * dir.x) + dir.z * dir.z + kNormalizeEpsilon);
        dir.x = inv * dir.x;
        dir.y = inv * dir.y;
        dir.z = inv * dir.z;
    }

    // One request object per branch; both die after SetGoal, so the compiler merges
    // their destruction behind a "which was built" bitmask.
    loco->SetGoal(bPath ? NavRequest(dest, dir, speed, stopDistance)
                        : NavRequest(dest, speed, stopDistance));

    loco->SetDesiredSpeed(loco->GetStandardSpeed(), 0);
    loco->mMoveStatusFlags = 0;
}

}  // namespace SP
