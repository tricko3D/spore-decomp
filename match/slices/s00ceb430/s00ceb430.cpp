// 0x00ceb430 -- Simulator::cCityDisplayStrategy::NoteVehicleAdded
//
// A vehicle has appeared.  If the strategy already owns a posse for the
// vehicle's locomotion class, the per-class "vehicle -> posse" map is probed:
// when the vehicle is not in the map yet, a new posse is built, attached to the
// vehicle, given a locomotion-dependent icon and recorded in the map, then the
// posse bars are rebuilt.
//
// Afterwards the vehicle's minimap icon is refreshed: the old icon is removed
// and a new descriptor -- 0xffRRGGBB colour built from the vehicle colour,
// the position reported by the vehicle pivot, a flag word and a reference to
// the vehicle -- is handed to the CivMinimap window.
//
// Recovered layout used here:
//   cCityDisplayStrategy: +0x6c cSPUIPosse*[4], +0x7c tree[4] (stride 0x1c,
//     anchor/end node at +0x04), +0xec UI layout, +0xf0 CivMinimap window
//   vehicle: +0x34 pivot (its own vtable), +0xb1c locomotion, +0xb20 class
#include "game/types.h"
#include "sse_helpers.h"

typedef unsigned char u8;
typedef unsigned int  u32;

struct fVec3 { float x, y, z; };

// ---------------------------------------------------------------------------
// vtables: only the slots this function touches are named.
// ---------------------------------------------------------------------------
struct IObj {
    virtual void AddRef();                              // +0x00
    virtual void Release();                             // +0x04
};
struct IObjKeyed : public IObj {
    virtual void  vf08();                               // +0x08
    virtual void* GetKey(u32 nClassKey);                // +0x0c
};
// The vehicle pivot sub-object at vehicle+0x34.
struct CPivot {
    virtual void  vf00(); virtual void vf04(); virtual void vf08();
    virtual void  vf0c(); virtual void vf10(); virtual void vf14();
    virtual void  vf18(); virtual void vf1c(); virtual void vf20();
    virtual void  vf24(); virtual void vf28();
    virtual fVec3* GetPosition();                       // +0x2c
    virtual void  vf30(); virtual void vf34(); virtual void vf38();
    virtual void  vf3c(); virtual void vf40(); virtual void vf44();
    virtual void  vf48(); virtual void vf4c(); virtual void vf50();
    virtual void  vf54();
    virtual char  IsVisible();                          // +0x58
};
// The vehicle is only addressed by byte offsets here: its pivot sub-object
// sits at +0x34, its locomotion class at +0xb1c and the posse class whose
// posse it joins at +0xb20 (only the pivot has a vtable).
#define VEHICLE_PIVOT(p)    ((CPivot*)((char*)(p) + 0x34))
#define VEHICLE_LOCOMOTION(p) (*(int*)((char*)(p) + 0xb1c))
#define VEHICLE_POSSECLASS(p) (*(int*)((char*)(p) + 0xb20))
// A posse shown in the CivUI posse bar.
struct CPosse {
    virtual void  vf00(); virtual void vf04(); virtual void vf08();
    virtual void  vf0c(); virtual void vf10(); virtual void vf14();
    virtual void  vf18(); virtual void vf1c(); virtual void vf20();
    virtual void  vf24(); virtual void vf28(); virtual void vf2c();
    virtual void  vf30(); virtual void vf34(); virtual void vf38();
    virtual void  vf3c(); virtual void vf40(); virtual void vf44();
    virtual void  vf48(); virtual void vf4c(); virtual void vf50();
    virtual void  vf54(); virtual void vf58(); virtual void vf5c();
    virtual void  vf60(); virtual void vf64(); virtual void vf68();
    virtual void  vf6c(); virtual void vf70(); virtual void vf74();
    virtual void  vf78(); virtual void vf7c(); virtual void vf80();
    virtual void  vf84();
    virtual void  SetKind(int nKind);                   // +0x88
    virtual void  vf8c(); virtual void vf90();
    virtual void  SetIcon(void* pImage, int nFlags);    // +0x94
    virtual void  vf98(); virtual void vf9c(); virtual void vfa0();
    virtual void  SetVehicle(void* pObject);            // +0xa4
    virtual void  vfa8(); virtual void vfac(); virtual void vfb0();
    virtual void  vfb4(); virtual void vfb8(); virtual void vfbc();
    virtual void  vfc0();
    virtual int   GetValue();                           // +0xc4
};
// Allocation header of cSPUIPosse: its constructor runs on a raw cell.
struct CPosseMem {
    CPosse* Construct(void* pKey);                      // 0x00e25950 (thiscall)
};
// The minimap window.
struct CMinimap {
    virtual void vf00(); virtual void vf04(); virtual void vf08();
    virtual void vf0c(); virtual void vf10(); virtual void vf14();
    virtual void vf18(); virtual void vf1c(); virtual void vf20();
    virtual void vf24(); virtual void vf28(); virtual void vf2c();
    virtual void vf30(); virtual void vf34(); virtual void vf38();
    virtual void vf3c(); virtual void vf40(); virtual void vf44();
    virtual void vf48(); virtual void vf4c(); virtual void vf50();
    virtual void vf54(); virtual void vf58(); virtual void vf5c();
    virtual void vf60(); virtual void vf64(); virtual void vf68();
    virtual void vf6c(); virtual void vf70(); virtual void vf74();
    virtual void vf78(); virtual void vf7c(); virtual void vf80();
    virtual void vf84(); virtual void vf88(); virtual void vf8c();
    virtual void vf90(); virtual void vf94(); virtual void vf98();
    virtual void vf9c(); virtual void vfa0(); virtual void vfa4();
    uint8_t mPadA8[0x4c];
    void RemoveIcon(void* pObject, int nA, int nB);     // 0x00e0f9c0 (thiscall)
    void AddIcon(void* pObject, void* pRecord, int nA, int nB); // 0x00e10580 (thiscall)
};
// The vehicle colour source (thiscall helper, one out parameter).
struct IColourSource {
    void GetColour(fVec3* pColour);                     // 0x00c9f650
};

// ---------------------------------------------------------------------------
// eastl red-black tree keyed by reference counted object pointers.  Each tree
// is 0x1c bytes; the end/iterator anchor sits at +0x04 and a node stores the
// key at +0x10 and the mapped value at +0x14.
// ---------------------------------------------------------------------------
struct CMapNode;
struct CRefMap {
    u32      mnAllocTag;                                // +0x00
    void*    mpAnchor;                                  // +0x04 end node
    void*    mpAnchorRight;                             // +0x08
    void*    mpRoot;                                    // +0x0c
    u8       mbColor;                                   // +0x10
    u8       mPad011[3];
    u32      mnSize;                                    // +0x14
    u32      mPad018;                                   // +0x18

    void      find(void** ppNode, const void* pKey);    // 0x00e5c780 (thiscall)
    void*     LookupOrInsert(const void* pKey, int nValue); // 0x00ceb2b0 (thiscall)
};
// Mapped slot returned by the lookup; updated right after it is found.
struct CMapSlot {
    void Update();                                      // 0x00b5f950 (thiscall)
};

// ---------------------------------------------------------------------------
// helpers (relocations; the verifier masks call targets)
// ---------------------------------------------------------------------------
void* __cdecl FUN_009512c0();                                        // 0x009512c0
void* __cdecl FUN_009512d0(u32 nSize, u32 nAlign, const char* pszWhat,
                           void* pContext);                          // 0x009512d0
void* __cdecl FUN_008061c0(void* pLayout, u32 nImageID);             // 0x008061c0

// ---------------------------------------------------------------------------
// The minimap icon descriptor handed to CMinimap::AddIcon.
// ---------------------------------------------------------------------------
struct SEventIcon {
    u32    mnColor;                                     // +0x00 0xffRRGGBB
    fVec3  mPosition;                                   // +0x04
    u32    mnFlags;                                     // +0x10
    void*  mpObject;                                    // +0x14
    float  mf18;                                        // +0x18
    u32    mn1c;                                        // +0x1c
    u32    mn20;                                        // +0x20
};

// Window reference cell used by the strategy.
struct CAutoRef {
    void* mpObject;                                     // +0x00
};

namespace Simulator {
class cCityDisplayStrategy {
public:
    virtual void vf00();                                // +0x00
    u8         mPad004[0x68];                           // +0x04
    void*      mpPosses[4];                             // +0x6c
    CRefMap    mVehiclePosseMaps[4];                    // +0x7c
    void*      mpLayout;                                // +0xec
    CAutoRef   mMiniMapWin;                             // +0xf0

    void NoteVehicleAdded(void* pNoun);                 // 0x00ceb430
    void RebuildPosseBars();                            // 0x00ce90c0
};

// ------------------------------------------------------------------ 0x00ceb430
// @ 0x00ceb430
void cCityDisplayStrategy::NoteVehicleAdded(void* pNoun)
{
    char    bVisible = VEHICLE_PIVOT(pNoun)->IsVisible();
    int     nClass = VEHICLE_POSSECLASS(pNoun);
    CPosse* pPosse = 0;

    if (nClass == 4)
        nClass = 0;

    if (mpPosses[nClass] != 0 && bVisible) {
        void* pRef = pNoun;
        ((IObj*)pNoun)->AddRef();

        u8* pBase = (u8*)this + nClass * 0x1c;
        CRefMap* pMap = (CRefMap*)(pBase + 0x7c);
        void* pNode;
        pMap->find(&pNode, &pRef);

        ((IObj*)pNoun)->Release();

        if (pNode == (void*)(pBase + 0x80)) {
            void* pMem = FUN_009512d0(0x14c, 4, (const char*)0x1476a7c,
                                      FUN_009512c0());
            if (pMem != 0)
                pPosse = ((CPosseMem*)pMem)->Construct(
                    ((IObjKeyed*)pNoun)->GetKey(0xee3f516e));

            ((CPosse**)mpPosses)[nClass]->SetVehicle(pPosse);

            int nKind = 0;
            u32 nImageID;
            switch (VEHICLE_LOCOMOTION(pNoun)) {
            case 1: nKind = 1; nImageID = 0x5d6943e; break;
            case 2: nKind = 2; nImageID = 0x5d69467; break;
            default: nImageID = 0x5d693e7; break;
            }
            void* pImage = FUN_008061c0(mpLayout, nImageID);
            pPosse->SetIcon(pImage, 0);
            pPosse->SetKind(nKind);

            pRef = pNoun;
            ((IObj*)pNoun)->AddRef();
            ((CMapSlot*)pMap->LookupOrInsert(&pRef, pPosse->GetValue()))->Update();
            ((IObj*)pNoun)->Release();

            RebuildPosseBars();
        }
    }

    if (mMiniMapWin.mpObject != 0) {
        ((CMinimap*)mMiniMapWin.mpObject)->RemoveIcon(pNoun, 1, 0);

        fVec3 colour;
        SEventIcon rec;
        rec.mnColor = 0;
        rec.mPosition.x = 0.0f;
        rec.mPosition.y = 0.0f;
        rec.mPosition.z = -1.0f;
        rec.mpObject = 0;
        rec.mf18 = 0.0f;
        rec.mn1c = 0;
        rec.mn20 = 0;
        rec.mnFlags = 0x100;

        ((IColourSource*)pNoun)->GetColour(&colour);

        u32 nB = (u32)SseTruncToInt(colour.z * 255.0f);
        u32 nG = (u32)SseTruncToInt(colour.y * 255.0f);
        u32 nR = (u32)SseTruncToInt(colour.x * 255.0f);
        rec.mnColor = (((nR | 0xffffff00) << 8) | (nG & 0xff)) << 8 | (nB & 0xff);

        fVec3* pPosition = VEHICLE_PIVOT(pNoun)->GetPosition();
        rec.mPosition = *pPosition;

        void* pOld = rec.mpObject;
        if (pOld != pNoun) {
            ((IObj*)pNoun)->AddRef();
            rec.mpObject = pNoun;
            if (pOld != 0)
                ((IObj*)pOld)->Release();
        }

        if (bVisible)
            rec.mnFlags |= 8;

        ((CMinimap*)mMiniMapWin.mpObject)->AddIcon(pNoun, &rec, 0, 0);

        if (rec.mpObject != 0)
            ((IObj*)rec.mpObject)->Release();
    }
}
}  // namespace Simulator
