// Behavior-tree action tick at 0x00d959d0.
//
// The citizen asks its owner's action circle for the free slot nearest the
// citizen's own position, reserves it, and moves onto it.  Bit 0 of the flags
// argument selects between snapping the pose onto the slot (position + surface
// orientation, then a move request) and interrupting the current animation to
// walk to the slot.  Either way the action's blocking effect is cleared, the
// output slot is reset and the owner's action counter is bumped.
#include <math.h>

typedef unsigned int uint32_t;

// Vtable dispatch on a sub-object whose class header is not available yet; the
// slot byte offsets are the ones the retail call sites use.
#define VSLOT(pObject, nOffset) (*(void**)(*(char**)(pObject) + (nOffset)))

struct cVector3_s00d9_59d0 {
    float m_fX;                                     // +0x00
    float m_fY;                                     // +0x04
    float m_fZ;                                     // +0x08
};

struct Quat_s00d9_59d0 {
    float m_fX;
    float m_fY;
    float m_fZ;
    float m_fW;
};

extern float g169f20c_s00d9_59d0;                   // 0x0169f20c
extern float g169f210_s00d9_59d0;                   // 0x0169f210
extern float g169f214_s00d9_59d0;                   // 0x0169f214
extern float g1485720_s00d9_59d0;                   // 0x01485720  1.0f
extern float g1470f1c_s00d9_59d0;                   // 0x01470f1c  2.0f
extern float g13ec4b4_s00d9_59d0;                   // 0x013ec4b4  0.9f
extern float g147c0f4_s00d9_59d0;                   // 0x0147c0f4  FLT_MAX
extern float g13ec4b8_s00d9_59d0;                   // 0x013ec4b8  1e-08f

// 0x00f47380 (operator delete[])
void EaFree_s00d9_59d0(void* pMemory);

class cCitizen_s00d9_59d0;

class cActionSlot_s00d9_59d0 {                     // stride 0x18
public:
    void Reserve(cCitizen_s00d9_59d0* pAgent);      // 0x00afa030
    bool m_bReserved;                               // +0x00
    unsigned char m_pad001[0x0b];                   // +0x01
    cVector3_s00d9_59d0 m_vPosition;                // +0x0c
};

class cActionCircle_s00d9_59d0 {
public:
    cActionSlot_s00d9_59d0* FindSlot(const cVector3_s00d9_59d0* pPosition);  // 0x00afad70
    unsigned char m_pad000[0x14];                   // +0x00
    cVector3_s00d9_59d0 m_vCenter;                  // +0x14
};

struct cOwner_s00d9_59d0 {
    unsigned char m_pad000[0x70];                   // +0x000
    cActionCircle_s00d9_59d0 m_actionCircle;        // +0x070
    unsigned char m_pad090[0xa38 - 0x090];          // +0x090
    int m_nActionCounter_a38;                       // +0xa38
};

struct cAllocationHeader_s00d9_59d0 {
    int m_nCapacity;                                // +0x00
};

class cBuffer_s00d9_59d0 {                          // ref-counted byte buffer
public:
    ~cBuffer_s00d9_59d0()
    {
        if (m_pData != 0) {
            cAllocationHeader_s00d9_59d0* pHeader = (cAllocationHeader_s00d9_59d0*)m_pData - 1;
            if (pHeader->m_nCapacity != 0)
                EaFree_s00d9_59d0(m_pData);
        }
    }
    unsigned char* m_pData;                         // +0x00
};

struct cMoveRequest_s00d9_59d0 {
    cBuffer_s00d9_59d0 m_buffer;                    // +0x00
    uint32_t m_n004;                                // +0x04
    uint32_t m_n008;                                // +0x08
    unsigned char m_pad00C[8];                      // +0x0c
    cVector3_s00d9_59d0 m_vTarget;                  // +0x14
    float m_f020;                                   // +0x20
    uint32_t m_n024;                                // +0x24
    unsigned char m_pad028[0x24];                   // +0x28
    bool m_b04C;                                    // +0x4c
    unsigned char m_pad04D[3];                      // +0x4d
    float m_f050;                                   // +0x50
    float m_f054;                                   // +0x54
    float m_f058;                                   // +0x58
    uint32_t m_n05C;                                // +0x5c
    float m_f060;                                   // +0x60
    float m_f064;                                   // +0x64
    float m_f068;                                   // +0x68
    float m_f06C;                                   // +0x6c
    uint32_t m_n070;                                // +0x70
};

class cPlanetModel_s00d9_59d0 {
public:
    Quat_s00d9_59d0* BuildSurfaceOrientation2(Quat_s00d9_59d0* pOut,
                                             const cVector3_s00d9_59d0* pPosition,
                                             const cVector3_s00d9_59d0* pDirection);  // 0x00b7f250
};

cPlanetModel_s00d9_59d0* SP_PlanetModel_s00d9_59d0();   // 0x00b3d350

struct cCitizen_s00d9_59d0 {
    void InterruptAnimation(uint32_t nAnim, int nBlend, int nFlags);            // 0x00c12310
    void MoveToPointAndFacingAtSpeed(int nMode, const cVector3_s00d9_59d0* pPoint,
                                     const cVector3_s00d9_59d0* pFacing,
                                     float fSpeed, float fTurn);                // 0x00c1c5c0
    void SetCurrentToolEffect(int nTool, unsigned int nId);                     // 0x00c24f40
    unsigned char m_pad000[0xc0];                   // +0x000
    unsigned char m_spatial[0xfb8 - 0xc0];          // +0x0c0 (vptr only is modelled)
    cOwner_s00d9_59d0* m_pOwner;                    // +0xfb8
};

typedef cVector3_s00d9_59d0* (__thiscall *TFnGetPosition_s00d9_59d0)(void* pObject);
typedef void (__thiscall *TFnSetPosition_s00d9_59d0)(void* pObject, const cVector3_s00d9_59d0* pPosition);
typedef void (__thiscall *TFnSetOrientation_s00d9_59d0)(void* pObject, const Quat_s00d9_59d0* pOrientation);
typedef void (__thiscall *TFnRequestMove_s00d9_59d0)(void* pObject, const cMoveRequest_s00d9_59d0* pRequest);

// @ 0x00d959d0
bool ActionMoveToSlot_Tick(cCitizen_s00d9_59d0* pCitizen, int nArg2, int nArg3,
                           uint32_t nFlags, int nArg5, int* pnOut)
{
    void* pSpatial = pCitizen->m_spatial;

    cVector3_s00d9_59d0* pPosition =
        ((TFnGetPosition_s00d9_59d0)VSLOT(pSpatial, 0x2c))(pSpatial);
    cActionSlot_s00d9_59d0* pSlot =
        pCitizen->m_pOwner->m_actionCircle.FindSlot(pPosition);
    if (pSlot == 0)
        return false;

    pSlot->Reserve(pCitizen);

    const cVector3_s00d9_59d0* pCenter = &pCitizen->m_pOwner->m_actionCircle.m_vCenter;
    const cVector3_s00d9_59d0* pSlotPosition = &pSlot->m_vPosition;

    // Retail computes the three deltas in the order z, x, y (the x87 sum is
    // dz*dz + dx*dx + dy*dy), so the field assignments are written in the
    // reverse order: the compiler emits independent float stores back to front.
    cVector3_s00d9_59d0 vDelta;
    vDelta.m_fY = pCenter->m_fY - pSlotPosition->m_fY;
    vDelta.m_fX = pCenter->m_fX - pSlotPosition->m_fX;
    vDelta.m_fZ = pCenter->m_fZ - pSlotPosition->m_fZ;
    float fInvLength = 1.0f / sqrtf(vDelta.m_fY * vDelta.m_fY + vDelta.m_fX * vDelta.m_fX
                                    + vDelta.m_fZ * vDelta.m_fZ + g13ec4b8_s00d9_59d0);

    cVector3_s00d9_59d0 vDirection;
    vDirection.m_fX = vDelta.m_fX * fInvLength;
    vDirection.m_fY = vDelta.m_fY * fInvLength;
    vDirection.m_fZ = vDelta.m_fZ * fInvLength;

    if (nFlags & 1)
    {
        ((TFnSetPosition_s00d9_59d0)VSLOT(pSpatial, 0x38))(pSpatial, pSlotPosition);

        Quat_s00d9_59d0 qOrientation;
        ((TFnSetOrientation_s00d9_59d0)VSLOT(pSpatial, 0x3c))(
            pSpatial, SP_PlanetModel_s00d9_59d0()->BuildSurfaceOrientation2(
                          &qOrientation, pSlotPosition, &vDirection));

        {
            // The move-request entry point is fetched up front: the original
            // reads the vtable slot before filling the request in.
            TFnRequestMove_s00d9_59d0 fnRequestMove =
                (TFnRequestMove_s00d9_59d0)VSLOT(pSpatial, 0xdc);
            cMoveRequest_s00d9_59d0 request;
            request.m_vTarget.m_fX = g169f20c_s00d9_59d0;
            request.m_vTarget.m_fY = g169f210_s00d9_59d0;
            request.m_vTarget.m_fZ = g169f214_s00d9_59d0;
            request.m_f060 = g1470f1c_s00d9_59d0;
            request.m_f020 = g1485720_s00d9_59d0;
            request.m_f064 = g13ec4b4_s00d9_59d0;
            request.m_buffer.m_pData = 0;
            request.m_n004 = 0;
            request.m_n008 = 0;
            request.m_n024 = 0;
            request.m_b04C = false;
            request.m_f050 = 0.0f;
            request.m_f054 = 0.0f;
            request.m_f058 = 0.0f;
            request.m_n05C = 0;
            request.m_f068 = g147c0f4_s00d9_59d0;
            request.m_f06C = 0.0f;
            request.m_n070 = 0;
            fnRequestMove(pSpatial, &request);
        }
    }
    else
    {
        pCitizen->InterruptAnimation(0x02481db0u, -1, 0);
        pCitizen->MoveToPointAndFacingAtSpeed(2, pSlotPosition, &vDirection,
                                              1.0f, g1470f1c_s00d9_59d0);
    }

    pCitizen->SetCurrentToolEffect(0x0b, (unsigned int)-1);
    *pnOut = 0;
    pCitizen->m_pOwner->m_nActionCounter_a38++;
    return true;
}
