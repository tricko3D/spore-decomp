// Slice s00d164d0 - SporeApp 0x00d164d0..0x00d16620
// SP::cGameEditInputStrategy::DeleteSelected: walk the editor selection, drop
// every placed gameplay marker from it, and remove the marker (and the noun in
// its +0x1a4 slot) from the noun manager. Removing a herd noun flags the herd
// update pass.
// Flags: /O2 /Ob1 /MD /Gy /EHsc /TP /arch:SSE /GS-
//
// Residual diff (329 vs 332 bytes).  The old callee-saved blocker (cl keeping
// `this` in ebp and the loop index in edi) is gone: the two member reads of
// `mpSelectedObject` at the top of the loop make cl pick retail's assignment
// (`this` in edi, spilled at [esp+0x14] and reloaded at the end of the
// removal arm; index in ebp, spilled at [esp+0xc]; `&mSelected` in esi), so
// the prologue, the compare, the reload-for-`&&`, the store order and the
// whole swap-remove arm are byte-identical now.
//
// The two reads are spelled so that the second one is a real reload of the
// member (retail loads it again for the `&&` test instead of reusing the value
// the compare left behind); the `volatile` qualifier on that one access is the
// compiler fence that pins it (it emits no extra code and the load is one
// retail performs too).  `pOld` is the same load, so the release arm does not
// read the member a third time, exactly like retail.
//
// What is left is one address form: retail re-reads the selection element as
//   mov eax,[esi] / add eax,ebx / mov eax,[eax] / test eax,eax
// (materialise `&mpBegin[i]`, then load through it, giving `mov ecx,eax`
// before the Cast vcall), while cl folds the byte offset into the addressing
// mode:
//   mov eax,[esi] / mov ecx,[ebx+eax] / test ecx,ecx
// Every spelling tried for that access (mpBegin[i], operator[], pointer
// arithmetic, char offsets, reference and pointer bindings, declaration in the
// `if`, casts through u8*/void*, __restrict, ...) folds the same way; the
// materialised form does not occur in any of the 17k matched retail functions
// either.  The 3 bytes are that fold plus the `mov ecx,eax` it makes
// unnecessary.
#include "types.h"

typedef unsigned char  u8;
typedef unsigned int   u32;

// ---------------------------------------------------------------------------------------------
// Support types
// ---------------------------------------------------------------------------------------------
#define PV1(n)  virtual void n();
#define PV2(n)  PV1(n##0) PV1(n##1)
#define PV4(n)  PV2(n##0) PV2(n##1)
#define PV8(n)  PV4(n##0) PV4(n##1)
#define PV16(n) PV8(n##0) PV8(n##1)

struct cGameplayMarkerCmd;

// One entry of the editor selection. Cast(0x36be278) yields the gameplay-marker
// command; AddRef/Release are the intrusive reference count (slots 0xbc/0xc0).
struct ISelectable {
    PV16(v00)                                       // 0x00
    PV16(v10)                                       // 0x40
    PV8(v20)                                        // 0x80
    PV4(v24)                                        // 0xa0
    PV2(v28)                                        // 0xb0
    virtual cGameplayMarkerCmd* Cast(u32 typeID);   // 0xb8
    virtual void AddRef();                          // 0xbc
    virtual void Release();                         // 0xc0
};

// Sub-object at marker+0x34: the marker's placement state.
struct IMarkerPart {
    PV16(v00)                                       // 0x00
    PV4(v10)                                        // 0x40
    virtual bool IsPlaced();                        // 0x50
};

// Object in the marker's +0x1a4 slot (its noun/handler).
struct cMarkerHandler {
    PV4(v00)                                        // 0x00
    PV4(v10)                                        // 0x10
    virtual u32 v20();                              // 0x20
};

struct cGameplayMarkerCmd {
    char            pad00[0x34];
    IMarkerPart     part;                           // +0x34
    char            pad38[0x1a4 - 0x38];
    cMarkerHandler* mpField1a4;                     // +0x1a4
};

// Reference-counted slot of the editor selection array.
struct ObjRef {
    ISelectable* mpObject;                          // +0x00
};

struct SelectionList {
    ObjRef* mpBegin;                                // +0x00
    ObjRef* mpEnd;                                  // +0x04
    u32     GetCount() const { return (u32)(mpEnd - mpBegin); }
    ObjRef& operator[](u32 i) { return mpBegin[i]; }
};

struct cSelection {
    char          pad00[0x40];
    SelectionList mSelected;                        // +0x40
};

// ---------------------------------------------------------------------------------------------
// External helpers implemented in other slices.
// ---------------------------------------------------------------------------------------------
extern u8 gHerdNeedsUpdate;                         // 0x0169dc20  herd update flag

cSelection* __cdecl GetEditSelection();             // 0x00d1bf00

struct cNounManager {
    void RemoveNoun(void* pNoun);                   // 0x00b225d0
};
cNounManager* __cdecl NounManager();                // 0x00b3d300

void __cdecl FUN_00ba1280();                        // 0x00ba1280

// ---------------------------------------------------------------------------------------------
// SP::cGameEditInputStrategy
// ---------------------------------------------------------------------------------------------
namespace SP {
struct cGameEditInputStrategy {
    char         pad00[0x4c];
    ISelectable* mpSelectedObject;                  // +0x04c
    void DeleteSelected();
};
}  // namespace SP

// @ 0x00d164d0
void SP::cGameEditInputStrategy::DeleteSelected()
{
    SelectionList& selection = GetEditSelection()->mSelected;
    ISelectable* pOld;

    for (u32 i = 0; i < selection.GetCount(); ) {
        // retail reads the member twice: once as the compare's memory operand,
        // once more (a real reload) for the && test; pOld is that second load
        if (selection[i].mpObject == this->mpSelectedObject &&
            (pOld = *(ISelectable* volatile*)&this->mpSelectedObject)) {
            this->mpSelectedObject = 0;
            pOld->Release();
        }

        ISelectable* pSelected = selection[i].mpObject;
        if (pSelected) {
            cGameplayMarkerCmd* pMarker = pSelected->Cast(0x36be278);
            if (pMarker) {
                if (!pMarker->part.IsPlaced()) {
                    ++i;
                    continue;
                }

                // swap-remove selection[i]: the last entry takes its place and
                // both the replaced entry and the vacated slot lose a reference
                ObjRef* pSlot = &selection.mpBegin[i];
                ISelectable* pOldElement = pSlot->mpObject;
                ISelectable* pNewElement = selection.mpEnd[-1].mpObject;
                if (pNewElement != pOldElement) {
                    if (pNewElement)
                        pNewElement->AddRef();
                    pSlot->mpObject = pNewElement;
                    if (pOldElement)
                        pOldElement->Release();
                }
                --selection.mpEnd;
                ISelectable* pVacated = selection.mpEnd->mpObject;
                if (pVacated)
                    pVacated->Release();

                cMarkerHandler* pHandler = pMarker->mpField1a4;
                if (pHandler) {
                    gHerdNeedsUpdate |= (u8)(pHandler->v20() == 0x1be418e);
                    NounManager()->RemoveNoun(pHandler);
                }
                NounManager()->RemoveNoun(pMarker);
                continue;
            }
        }
        ++i;
    }

    FUN_00ba1280();
}
