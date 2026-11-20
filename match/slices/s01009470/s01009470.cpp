// SP::cSPSimulatorSpaceGame::DestroyNPC_UFOs
//
// Layout note: field offsets are taken from this build's disassembly (the
// neighbour slice s01008910 documents the same class): the NPC-UFO
// SPVector<AutoRefCount> sits at this+0x78, the UFO state at +0x714 and the
// ambush target at +0x7b0.
#include "types.h"

#define nullptr 0

// EA::RefCountVTemplate<int> -- virtual AddRef/Release.
class IRefCount {
public:
    virtual void AddRef();          // +0x00
    virtual void Release();         // +0x04
};

struct AutoRefCount {
    IRefCount* mpObject;            // +0x00
};

struct VectorAllocHeader {
    int32_t mnCapacity;             // stored in front of an eastl sp_vector block
};

void __cdecl operator_delete_array(void* pBlock);           // 0x00f47380

// SPVector<EA::AutoRefCount<T> >
class SPVector {
public:
    AutoRefCount* mpBegin;          // +0x00
    AutoRefCount* mpEnd;            // +0x04
    AutoRefCount* mpCapacity;       // +0x08
    // The shipped container is 0x14 bytes: retail's local frame is 0x1c with
    // only temp (4), the saved list pointer (4) and this block (0x14) in it,
    // so two trailing pointer slots belong to the class even though the
    // inlined insert/destroy paths never touch them.
    void* mpAllocator;              // +0x0c
    void* mpDeleter;                // +0x10

    void DoInsertPtr(AutoRefCount* pPos, const AutoRefCount& rValue);   // 0x00aea5d0
    void DestroyListedUFOs(SPVector* pList);                            // 0x01008600

    void Destroy()
    {
        for (AutoRefCount* p = mpBegin; p < mpEnd; ++p) {
            if (p->mpObject != nullptr)
                p->mpObject->Release();
        }
        if (mpBegin != nullptr &&
            reinterpret_cast<VectorAllocHeader*>(mpBegin)[-1].mnCapacity != 0) {
            operator_delete_array(mpBegin);
        }
    }
};

class cGameData {                   // cStarRecord noun
public:
    uint8_t mPad00[0x70];           // +0x000
    int32_t mnStarId;               // +0x070
};

class cStarMap {
public:
    void RemoveVisual();                                        // 0x01045ae0
};

// The UFO game data object (state at +0x714, ambush target at +0x7b0).
class cSPGameDataUFO : public IRefCount {
public:
    uint8_t mPad04[0x714 - 0x04];   // +0x004
    int32_t mnState;                // +0x714
    uint8_t mPad718[0x7b0 - 0x718]; // +0x718
    cGameData* mpTarget;            // +0x7b0
};

void __cdecl DestroyCreature(cSPGameDataUFO* pUFO, int32_t nFlags);     // 0x0102c340
cStarMap* __stdcall GetStarMap4(int32_t nStarId, uint32_t nKey,
                                int32_t nFlags, int32_t nOne);          // 0x01046fc0

namespace SP {

class cSPSimulatorSpaceGame {
public:
    void DestroyNPC_UFOs(char bParam2, char bParam3, int32_t nFlags);   // 0x01009470

private:
    uint8_t mPad00[0x78];           // +0x000
    SPVector mNPC_UFOs;             // +0x078
};

} // namespace SP

// Append a UFO to a UFO list, exactly as the inlined SPVector insert does.
// rEnd keeps the list's end pointer in a register; the vector's own field is
// written back on every insert because the list is passed on afterwards.
static void InsertUFO(SPVector& rVector, AutoRefCount*& rEnd, AutoRefCount& temp,
                      cSPGameDataUFO* pUFO)
{
    temp.mpObject = pUFO;
    pUFO->AddRef();
    if (rEnd < rVector.mpCapacity) {
        AutoRefCount* pSlot = rEnd;
        rEnd = pSlot + 1;
        rVector.mpEnd = rEnd;
        if (pSlot != nullptr) {
            pSlot->mpObject = pUFO;
            pUFO->AddRef();
        }
    } else {
        rVector.DoInsertPtr(rEnd, temp);
        rEnd = rVector.mpEnd;
    }
    if (temp.mpObject != nullptr)
        temp.mpObject->Release();
}

// ---------------------------------------------------------------------------
// 0x01009470 -- drop NPC UFOs (optionally collecting them into a list first)
// ---------------------------------------------------------------------------
// @ 0x01009470
void SP::cSPSimulatorSpaceGame::DestroyNPC_UFOs(char bParam2, char bParam3, int32_t nFlags)
{
    SPVector vecDestroy;
    vecDestroy.mpBegin = nullptr;
    vecDestroy.mpEnd = nullptr;
    vecDestroy.mpCapacity = nullptr;
    AutoRefCount temp;
    AutoRefCount* pEnd = vecDestroy.mpEnd;

    for (AutoRefCount* pElem = mNPC_UFOs.mpBegin; pElem != mNPC_UFOs.mpEnd; ++pElem) {
        cSPGameDataUFO* pUFO = (cSPGameDataUFO*)pElem->mpObject;

        switch (pUFO->mnState) {
        case 8:
            if (bParam2 != 0) {
                InsertUFO(vecDestroy, pEnd, temp, pUFO);
                continue;
            }
            if (pUFO->mpTarget != nullptr) {
                int32_t nStarId = pUFO->mpTarget->mnStarId;
                GetStarMap4(nStarId, 0xb7735c3, 0, 1)->RemoveVisual();
            }
            break;

        case 3:
            if (bParam3 != 0) {
                InsertUFO(vecDestroy, pEnd, temp, pUFO);
                continue;
            }
            break;
        }

        DestroyCreature(pUFO, nFlags);
    }

    mNPC_UFOs.DestroyListedUFOs(&vecDestroy);
    vecDestroy.Destroy();
}
