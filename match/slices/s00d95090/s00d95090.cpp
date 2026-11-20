// @ 0x00d95090
// SP::CitizenTree::RepairBuilding_Activate -- the sibling of
// ExtinguishBuilding_Activate (0x00d961b0) in the same behavior family: the
// same IsDamaged-filtered spatial lane lookup, the GetCity/GetMembers/
// FUN_00d94d10 chain, the building-index lookup into the visualizer's action
// circle array, the two-slot occupancy test, the slot reservation, the facing
// computation and the two movement branches (flags & 1).
//
// It differs from the extinguish hook in that it does not raise the sprint
// flag or call SetSprinting, it picks the slot nearest the citizen's own
// position instead of the first free slot, and its memory block is smaller
// (a bool at +0x00, building index +0x04, slot index +0x08 and one
// window-text ref at +0x0c; the activation writes 1 as the +0x00 value).
#include "types.h"

#define nullptr 0

#define PVCAT2(a, b) a##b
#define PVCAT(a, b) PVCAT2(a, b)
#define PV  virtual void PVCAT(pv_, __COUNTER__)();
#define PV2 PV PV
#define PV4 PV2 PV2
#define PV8 PV4 PV4
#define PV16 PV8 PV8

// the engine's vector carries a user copy ctor: copy-initialising one ("T x(y)")
// compiles to movss pairs, not the integer-register moves a plain POD copy gets.
struct cVector3 {
    float m_fX;
    float m_fY;
    float m_fZ;
    cVector3() {}
    cVector3(const cVector3& v) : m_fX(v.m_fX), m_fY(v.m_fY), m_fZ(v.m_fZ) {}
};

struct cVector4 {
    float m_fX;
    float m_fY;
    float m_fZ;
    float m_fW;
};

class cGateCitizen;
class cCity;
class cCityVisualizer;
class cActionCircle;
class cActionSlot;
class cBuildingData;
class cPlanetModel;
class IWinText;

// ---------------------------------------------------------------------------
// globals (retail keeps these in .data, past the analysed image)
// ---------------------------------------------------------------------------
extern float g1470f1c;   // 0x01470f1c  2.0f
extern float g1471064;   // 0x01471064  0.5f

// ---------------------------------------------------------------------------
// small services
// ---------------------------------------------------------------------------
class cPlanetModel {
public:
    cVector4* BuildSurfaceOrientation();             // 0x00b7f250
};

// retail pushes three arguments and does not clean them, so the callee pops.
cPlanetModel* __stdcall PlanetModel(const cVector3& vDirection,
                                    const cVector3& vPosition,
                                    const cVector3& vUnitDirection);  // 0x00b3d350
void __cdecl NormalizedSafe(cVector3& vOut, const cVector3& vDirection);  // 0x00449c20
// the retail call pushes three arguments: the member blob, the citizen's
// position and the damage filter (right-to-left, one `add esp,0xc`).
cBuildingData* __cdecl FUN_00d94d10(void* pMembers, const cVector3* pPosition,
                                    void* pfnIsDamaged);              // 0x00d94d10

// ---------------------------------------------------------------------------
// action circles
// ---------------------------------------------------------------------------
class cActionSlot {                                  // stride 0x18
public:
    void Reserve(cGateCitizen* pAgent);              // 0x00afa030
    bool m_bReserved;                                // +0x00
    uint8_t m_pad001[0x0b];                          // +0x01
    cVector3 m_vPosition;                            // +0x0c
};

class cActionCircle {                                // stride 0x64
public:
    int32_t CountOccupiedSlots();                    // 0x00af9fa0
    int32_t FindNearestSlot(const cVector3* pPosition);  // 0x00afa5d0
    cActionSlot* SlotAt(uint32_t nSlot);             // 0x00af9ff0
    uint8_t m_pad000[8];                             // +0x00
    cActionSlot m_slots[1];                          // +0x08 (stride 0x18 each)
    uint8_t m_padEnd[0x64 - 0x08 - 0x18];            // +0x20
};

class cCityVisualizer {
public:
    int32_t FindBuildingIndex(cBuildingData* pData); // 0x00d58130
    uint8_t m_pad000[0x220];                         // +0x000
    cActionCircle* m_pCircleBegin;                   // +0x220
    cActionCircle* m_pCircleEnd;                     // +0x224
};

// the creature spatial/locomotion sub-object at gate-citizen +0xc0
class cSpatial {
public:
    PV8 PV2 PV                                       // +0x00 .. +0x28
    virtual cVector3* GetPosition();                 // +0x2c
    PV2                                              // +0x30 .. +0x34
    virtual void SetPosition(const cVector3* pPosition);           // +0x38
    virtual void SetOrientation(const cVector4* pOrientation);     // +0x3c
};

class cCity {
public:
    void* GetMembers();                              // 0x00c8e810 (cTribe::GetMembers)
};

// ---------------------------------------------------------------------------
// the gate citizen
// ---------------------------------------------------------------------------
class cGateCitizen {
public:
    cCity* GetCity();                                // 0x00c23e40
    void MoveToPointAndFacingAtSpeed(uint32_t nMode, const cVector3* pPoint,
                                     const cVector3* pFacing, float fSpeed,
                                     float fTurn);   // 0x00c1c5c0
    uint8_t m_pad000[0xc0];                          // +0x000
    cSpatial m_spatial;                              // +0x0c0
    uint8_t m_pad0c4[0xfb8 - 0xc4];                  // +0x0c4
    cCityVisualizer* m_pVisualizer;                  // +0xfb8
};

// ---------------------------------------------------------------------------
// the repair-building memory block: a window-text ref like the extinguish
// block, but only one ref and a bool ahead of the two indices
// ---------------------------------------------------------------------------
class IWinText {
public:
    virtual void V00();                              // +0x00
    virtual void Release();                          // +0x04
    virtual void V08();                              // +0x08
    virtual void Detach(int32_t nMode);              // +0x0c
};

class cWinTextRef {
public:
    void Assign(void* pText);                        // 0x00b5f950
    IWinText* m_pText;                               // +0x00
};

class cRepairMemory {
public:
    bool m_b000;                                     // +0x00
    uint8_t m_pad001[3];                             // +0x01
    int32_t m_nBuildingIndex;                        // +0x04
    int32_t m_nSlotIndex;                            // +0x08
    cWinTextRef m_ref00C;                            // +0x0c
};

// @ 0x00d95090
bool __cdecl RepairBuilding_Activate(cGateCitizen* pCitizen, int32_t nArg2,
                                     int32_t nArg3, uint32_t nFlags, int32_t nArg5,
                                     cRepairMemory* pMem)
{
    if (pMem != nullptr) {
        // retail stores +0x04 before +0x08, so these are two separate
        // statements (a chained assignment stores the rightmost first).
        pMem->m_nBuildingIndex = -1;
        pMem->m_nSlotIndex = -1;
        pMem->m_ref00C.m_pText = nullptr;
    }

    // The body is written as a guarded block whose failures all leave through
    // the single trailing `return false`, which is the shape retail lays out
    // (every failed check is a forward jump to the shared tail).
    do {
        // CitizenTree::IsDamaged (0x00d91320) rides along as the lookup's damage
        // filter; the citizen's own position narrows the search.
        cBuildingData* pData =
            FUN_00d94d10(pCitizen->GetCity()->GetMembers(),
                         pCitizen->m_spatial.GetPosition(),
                         (void*)0x00d91320);
        if (pData == nullptr)
            break;

        // retail re-loads the visualiser out of the citizen for each use instead
        // of keeping it in a local.
        int32_t nIndex = pCitizen->m_pVisualizer->FindBuildingIndex(pData);
        if (nIndex == -1)
            break;

        cActionCircle* pCircle = pCitizen->m_pVisualizer->m_pCircleBegin + nIndex;
        if (pCircle->CountOccupiedSlots() >= 2)
            break;

        // This last guard is written with the success arm nested: that is the arm
        // order retail lays out, with every failure edge falling through to the
        // one shared `return false` after the success epilogue.
        int32_t nSlot = pCircle->FindNearestSlot(pCitizen->m_spatial.GetPosition());
        if (nSlot != -1) {
            cActionSlot* pSlot = pCircle->SlotAt(nSlot);
            pSlot->Reserve(pCitizen);

            // retail loads the reservation's position into the slot-local and
            // subtracts the slot's own fields from the circle's first slot,
            // keeping both in xmm.
            cVector3 vSlotPos = pSlot->m_vPosition;
            cVector3 vDirection;
            vDirection.m_fX = pCircle->m_slots[0].m_vPosition.m_fX - vSlotPos.m_fX;
            vDirection.m_fY = pCircle->m_slots[0].m_vPosition.m_fY - vSlotPos.m_fY;
            vDirection.m_fZ = pCircle->m_slots[0].m_vPosition.m_fZ - vSlotPos.m_fZ;

            cVector3 vUnit;
            NormalizedSafe(vUnit, vDirection);

            // retail lays the orientation branch out as the fall-through and jumps
            // to the plain move when the flag is clear.
            if ((nFlags & 1) != 0) {
                pCitizen->m_spatial.SetPosition(&vSlotPos);
                pCitizen->m_spatial.SetOrientation(
                    PlanetModel(vDirection, vSlotPos, vUnit)->BuildSurfaceOrientation());
            }
            else {
                pCitizen->MoveToPointAndFacingAtSpeed(2, &pSlot->m_vPosition, &vUnit,
                                                      g1471064, g1470f1c);
            }

            pMem->m_b000 = true;
            pMem->m_ref00C.Assign(pData);
            pMem->m_nBuildingIndex = nIndex;
            pMem->m_nSlotIndex = nSlot;
            return true;
        }
    } while (0);

    return false;
}
