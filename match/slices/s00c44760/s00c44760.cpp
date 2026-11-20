// SP::cLocomotiveObject::OnPathCompleted(bool) -- 0x00c44760
//
// The callback is a virtual of the small sub-object that SP::cLocomotiveObject keeps at
// +0xd4 (the a-star client); it is entered with `this` pointing at that sub-object, so the
// locomotive itself is (char*)this - 0xd4.  The locomotive's move-status word sits at +0x270,
// the last path segment's two end points at +0x1a8/+0x1b4 and the queued path at +0x4b8.
//
// Flags: /O2 /Ob1 /MD /Gy /TP /EHs- /GS- /arch:SSE
#include "game/types.h"

typedef unsigned int u32;

// A three-float point.  The copy constructor makes the element copy use x87 (fld/fstp), as the
// original's inlined push_back does.
struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
};

struct Waypoint {                 // 0x3c bytes
    float mFields[15];
};

struct WaypointVector {
    Waypoint* mpBegin;
    Waypoint* mpEnd;
    Waypoint* mpCapacity;

    bool empty() const { return mpBegin == mpEnd; }
};

void EaFree(void* p);             // 0x00f47380 operator delete[]
inline void* operator new(unsigned int, void* p) { return p; }

// SP::tNavigationGoal as a self-contained request: waypoint vector plus the destination data,
// 0x74 bytes.  Its out-of-line constructor builds it from the queued waypoints and its
// destructor frees the copied waypoint block.
struct PathRequest {              // 0x74 bytes; first word is the waypoint array
    Waypoint* mpBegin;
    u32       mRest[28];
    PathRequest(const WaypointVector& v);                         // 0x00c1bad0
    ~PathRequest() { if (mpBegin && ((int*)mpBegin)[-1] != 0) EaFree(mpBegin); }
};

// Global list of anchor points the parts/locomotion code appends to (0x0168e81c).
class cVector3Vector {
public:
    Vector3* mpBegin;
    Vector3* mpEnd;
    Vector3* mpCapacity;

    void DoInsertValue(Vector3* position, const Vector3& value);  // 0x004b5ad0

    void push_back(const Vector3& value)
    {
        if (mpEnd < mpCapacity)
            ::new((void*)mpEnd++) Vector3(value);
        else
            DoInsertValue(mpEnd, value);
    }
};

extern cVector3Vector gVec168e81c;    // 0x0168e81c

// The enclosing SP::cLocomotiveObject; only the virtual slots this function reaches are named
// (retail order: SetDesiredSpeed at +0xc4, GetDesiredSpeed at +0xcc, SetGoal at +0xdc).
#define PV(n) virtual void pv##n();

struct cPathOwner {
    PV(0) PV(1) PV(2) PV(3) PV(4) PV(5) PV(6) PV(7) PV(8) PV(9) PV(10) PV(11) PV(12) PV(13)
    PV(14) PV(15) PV(16) PV(17) PV(18) PV(19) PV(20) PV(21) PV(22) PV(23) PV(24) PV(25)
    PV(26) PV(27) PV(28) PV(29) PV(30) PV(31) PV(32) PV(33) PV(34) PV(35) PV(36) PV(37)
    PV(38) PV(39) PV(40) PV(41) PV(42) PV(43) PV(44) PV(45) PV(46) PV(47) PV(48)
    virtual void SetDesiredSpeed(float speed, bool forced);        // +0xc4
    PV(50)
    virtual float GetDesiredSpeed();                               // +0xcc
    PV(52) PV(53) PV(54)
    virtual void SetGoal(const PathRequest& goal);                 // +0xdc
};

// The a-star client sub-object at SP::cLocomotiveObject + 0xd4.
struct cAStarClient_s00c44760 {
    void OnPathCompleted(bool completed);

    cPathOwner* GetLocomotive() { return (cPathOwner*)((char*)this - 0xd4); }

    char           mPad0000[0xd4];   // +0x000 (up to the locomotive's mPrStart)
    Vector3        mPrStart;         // +0x0d4
    Vector3        mPrEnd;           // +0x0e0
    char           mPad0Ec[0xb0];    // +0x0ec
    u32            mMoveStatusFlags; // +0x19c
    char           mPad1A0[0x244];   // +0x1a0
    WaypointVector mPathInQueue;     // +0x3e4
};

enum {
    kWaitingForPath = 4,
    kLastPathFailed = 8,
};

// @ 0x00c44760
void cAStarClient_s00c44760::OnPathCompleted(bool completed)
{
    if (completed) {
        if (mPathInQueue.empty())
            return;

        mMoveStatusFlags &= ~(u32)kWaitingForPath;

        cPathOwner* loco = GetLocomotive();
        loco->SetGoal(PathRequest(mPathInQueue));
        loco->SetDesiredSpeed(GetLocomotive()->GetDesiredSpeed(), true);
    } else {
        mMoveStatusFlags = kLastPathFailed;

        gVec168e81c.push_back(mPrStart);
        gVec168e81c.push_back(mPrEnd);
    }
}
