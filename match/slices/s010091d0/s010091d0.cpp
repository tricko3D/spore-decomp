// Flags: /O2 /Ob2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
//        (no /EHsc: retail has no unwind frame even though the inlined push_back
//         builds an intrusive-pointer temporary)
//
// SP::cSPSimulatorSpaceGame::SolarSystemSpawnNPC_UFOs(float fChance)
//
// Spawns at most one NPC UFO in the solar system the player is in.
//
// Everything below compiles to the retail instruction stream except the stack
// slots (retail is 662 bytes, ours is 662 bytes with 56 non-masked bytes
// differing, all stack displacements).  Retail's frame (0x14) is:
//   [esp+0x04]  fChance, later reused for the cPlanet* out parameter
//   [esp-0x08]  pointer to mNPC_UFOs (reused for the ResourceKey in the
//               "no UFO type yet" branch)
//   [esp-0x14]  cGameData* pCiv (reused for the Vec3 ambush offset and the
//               intrusive-pointer temporary of push_back)
// i.e. cl coloured the 8-byte ResourceKey temporary first (top slot) and the
// 12-byte Vec3 second, which lets pCiv merge into the Vec3 slot and leaves the
// dead fChance slot for pPlanet.  Our build colours the two temporaries in the
// opposite order (Vec3 at -0xc, ResourceKey at -0x14), which pushes pCiv to
// +0x04, pPlanet to -0x18 and the frame to 0x18.  See
// work/scratch/codex-pass1/investigations/0x010091d0/keep.md for the levers
// already tried.
#include "types.h"

#define nullptr 0

// ---------------------------------------------------------------------------
// small engine types
// ---------------------------------------------------------------------------
struct Vec3 { float x, y, z; };

// EA::RefCountVTemplate<int> -- virtual AddRef/Release.
class IRefCount {
public:
    virtual void AddRef();          // +0x00
    virtual void Release();         // +0x04
};

// EA::IRefCountPtr: an intrusive pointer constructed out of line (0x00572660)
// and released by an inline destructor.
class AutoRefCount {
public:
    IRefCount* mpObject;                    // +0x00

    AutoRefCount(IRefCount* pObject);       // 0x00572660

    ~AutoRefCount()
    {
        if (mpObject != nullptr)
            mpObject->Release();
    }
};

// EA::ResourceMan::Key
struct ResourceKey {
    uint32_t mnInstanceId;          // +0x00
    uint16_t mnGroupId;             // +0x04
    uint16_t mnTypeId;              // +0x06

    ResourceKey(uint32_t nInstanceId, uint16_t nGroupId, uint16_t nTypeId)
        : mnInstanceId(nInstanceId), mnGroupId(nGroupId), mnTypeId(nTypeId) {}
};

// eastl sp_vector of AutoRefCount -- only push_back is called here.
class SPVector {
public:
    AutoRefCount* mpBegin;          // +0x00
    AutoRefCount* mpEnd;            // +0x04
    AutoRefCount* mpCapacity;       // +0x08

    void PushBack(const AutoRefCount& rValue);      // 0x00e1c7f0
};

template <class TVector>
inline int32_t VectorCount(const TVector& rVector)
{
    return (int32_t)(rVector.mpEnd - rVector.mpBegin);
}

// ---------------------------------------------------------------------------
// game data nouns
// ---------------------------------------------------------------------------
class cEmpire;
class cPlanet;

// The star-record / civilisation noun (state tested by the draw loop at +0x28).
class cGameData {
public:
    uint8_t mPad00[0x28];           // +0x000
    int32_t mnField28;              // +0x028
    uint8_t mPad2c[0x50 - 0x2c];    // +0x02c

    int32_t GetAvatar();            // 0x00b1fdb0
    bool IsColonizable();           // 0x00b8d970
};

class cEmpire {
public:
    uint32_t GetUFOKey();           // 0x00c326b0 (thiscall, no stack argument)
};

// Holds the star's list of planet records (returned by reference).
class cStarRecord {
public:
    SPVector& GetPlanetRecords();   // 0x00bba790
};

// The local civilisation context: a star record pointer at +0x48.
class cLocalCivContext {
public:
    uint8_t mPad00[0x48];           // +0x000
    cStarRecord* mpStarRecord;      // +0x048
};

class cStarManager {
public:
    cEmpire* GetEmpireByID(int32_t nEmpireId);                      // 0x00ba9370
    void GetOrActivatePlanet(cGameData* pRecord, cPlanet** ppOut);  // 0x00bb59b0
};

// cPlanet's vtable is wide; only the +0xc0 slot (ReleaseObject) is used here.
class cPlanetVtblPrefix {
public:
    virtual void V000(); virtual void V004(); virtual void V008(); virtual void V00c();
    virtual void V010(); virtual void V014(); virtual void V018(); virtual void V01c();
    virtual void V020(); virtual void V024(); virtual void V028(); virtual void V02c();
    virtual void V030(); virtual void V034(); virtual void V038(); virtual void V03c();
    virtual void V040(); virtual void V044(); virtual void V048(); virtual void V04c();
    virtual void V050(); virtual void V054(); virtual void V058(); virtual void V05c();
    virtual void V060(); virtual void V064(); virtual void V068(); virtual void V06c();
    virtual void V070(); virtual void V074(); virtual void V078(); virtual void V07c();
    virtual void V080(); virtual void V084(); virtual void V088(); virtual void V08c();
    virtual void V090(); virtual void V094(); virtual void V098(); virtual void V09c();
    virtual void V0a0(); virtual void V0a4(); virtual void V0a8(); virtual void V0ac();
    virtual void V0b0(); virtual void V0b4(); virtual void V0b8(); virtual void V0bc();
};

class cPlanet : public cPlanetVtblPrefix {
public:
    virtual void ReleaseObject();   // +0x0c0
};

class cUFOFactory {
public:
    virtual void V00();             // +0x00
    virtual void V04();             // +0x04
    virtual void V08();             // +0x08
    virtual void V0c();             // +0x0c
    virtual void V10();             // +0x10
    virtual void V14();             // +0x14
    virtual void V18();             // +0x18
    virtual bool HasUFOTypeA(uint32_t nKey, int32_t nZero);     // +0x1c
    virtual void V20();             // +0x20
    virtual void V24();             // +0x24
    virtual void V28();             // +0x28
    virtual void V2c();             // +0x2c
    virtual void V30();             // +0x30
    virtual void V34();             // +0x34
    virtual void V38();             // +0x38
    virtual bool HasUFOTypeB(uint32_t nKey);                    // +0x3c
    virtual void V40();             // +0x40
    virtual void V44();             // +0x44
    virtual void V48();             // +0x48
    virtual void SetUFOTypeC(uint32_t nKey, const ResourceKey& rKey);  // +0x4c
};

// The UFO game data object: state at +0x714, position copy at +0x718.
class cSPGameDataUFO : public IRefCount {
public:
    uint8_t mPad04[0x6bc - 0x04];       // +0x004
    Vec3 mVec6bc;                       // +0x6bc
    uint8_t mPad6c8[0x714 - 0x6c8];     // +0x6c8
    int32_t mnState;                    // +0x714
    Vec3 mVec718;                       // +0x718

    void SetDestination(cPlanet* pPlanet);              // 0x00c3c190
    void SetLocalPosition(const Vec3* pPosition);       // 0x00c3dae0
};

// The universe the simulator looks at: the civilisation list sits at +0x7c.
class cSpaceStageUniverse {
public:
    uint8_t mPad00[0x7c];           // +0x000
    SPVector mCivs;                 // +0x07c
};

class RandomLinearCongruential {
public:
    double RandomDoubleUniform();                   // 0x009360d0
    uint32_t RandomUint32Uniform(uint32_t nRange);  // 0x00a68fb0
};

// ---------------------------------------------------------------------------
// engine accessors / free routines (absolute call targets are relocated)
// ---------------------------------------------------------------------------
cStarManager* __cdecl StarManager();                        // 0x00b3d2a0
cUFOFactory* __cdecl UFOFactory();                          // 0x00401010
cLocalCivContext* __cdecl GetLocalCivContext();             // 0x01021230
int32_t __cdecl GetPlayerEmpireID();                        // 0x01021090
void __cdecl GetAmbushOffset(Vec3* pOut);                   // 0x00ebbb40
void* __cdecl SP_CreateUFOForSolarOrGalaxy(int32_t nType, cEmpire* pEmpire);    // 0x0102ac50

extern "C" double __cdecl exp(double);      // expanded inline by clx (f2xm1/fscale)
extern "C" RandomLinearCongruential gRandom;    // 0x01601760

extern "C" float gfUFOAmbushScale;          // 0x015b68b0
extern "C" float gfSolarNPCDivisor;         // 0x015b68b4

// ---------------------------------------------------------------------------
// SP::cSPSimulatorSpaceGame
// ---------------------------------------------------------------------------
namespace SP {

class cSPSimulatorSpaceGame {
public:
    void SolarSystemSpawnNPC_UFOs(float fChance);   // 0x010091d0

private:
    uint8_t mPad00[0x30];               // +0x000
    cSpaceStageUniverse* mpUniverse;    // +0x030
    uint8_t mPad34[0x78 - 0x34];        // +0x034
    SPVector mNPC_UFOs;                 // +0x078
};

} // namespace SP

// @ 0x010091d0
void SP::cSPSimulatorSpaceGame::SolarSystemSpawnNPC_UFOs(float fChance)
{
    int32_t nUFOs = VectorCount(mNPC_UFOs);
    for (int32_t i = 0; i < nUFOs; ++i) {
        if (((cSPGameDataUFO*)mNPC_UFOs.mpBegin[i].mpObject)->mnState == 6)
            return;
    }

    SPVector* pRecords = &GetLocalCivContext()->mpStarRecord->GetPlanetRecords();
    if (pRecords->mpBegin == pRecords->mpEnd)
        return;

    double fRoll = gRandom.RandomDoubleUniform();
    double fSpawnChance = 1.0 - exp(fChance / gfSolarNPCDivisor * -0.30103f);
    if (!(fSpawnChance > fRoll))
        return;

    SPVector* pCivs = &mpUniverse->mCivs;
    int32_t nCivs = VectorCount(*pCivs);
    if (nCivs <= 1)
        return;

    cGameData* pCiv =
        (cGameData*)pCivs->mpBegin[gRandom.RandomUint32Uniform((uint32_t)nCivs)].mpObject;
    if (pCiv->GetAvatar() == GetPlayerEmpireID())
        return;

    int32_t nRecords = VectorCount(*pRecords);
    cGameData* pRecord;
    int32_t nTries = 9;
    do {
        uint32_t nIndex = gRandom.RandomUint32Uniform((uint32_t)nRecords);
        pRecord = (cGameData*)pRecords->mpBegin[nIndex].mpObject;
        if (pRecord->mnField28 != 0 && !pRecord->IsColonizable())
            break;
        --nTries;
    } while (nTries > 0);

    if (pRecord->mnField28 == 0 || pRecord->IsColonizable())
        return;

    cPlanet* pPlanet = nullptr;
    StarManager()->GetOrActivatePlanet(pRecord, &pPlanet);
    cEmpire* pEmpire = StarManager()->GetEmpireByID(pCiv->GetAvatar());

    if (!UFOFactory()->HasUFOTypeA(pEmpire->GetUFOKey(), 0)) {
        if (!UFOFactory()->HasUFOTypeB(pEmpire->GetUFOKey())) {
            // The key is built as a temporary in the caller's frame ("type C" = 1).
            UFOFactory()->SetUFOTypeC(pEmpire->GetUFOKey(), ResourceKey(0, 0, 1));
        }
    } else {
        cSPGameDataUFO* pUFO = (cSPGameDataUFO*)SP_CreateUFOForSolarOrGalaxy(6, pEmpire);
        Vec3 vecPos;
        GetAmbushOffset(&vecPos);
        vecPos.x = gfUFOAmbushScale * vecPos.x;
        vecPos.y = gfUFOAmbushScale * vecPos.y;
        // Retail multiplies by the float constant at 0x01485378, which is 0.0
        // (`mulss xmm0, dword ptr [0x01485378]`, not folded away).  A literal
        // 0.0f lands in the same constant-pool slot and compiles the same way.
        vecPos.z = gfUFOAmbushScale * 0.0f;
        pUFO->SetLocalPosition(&vecPos);
        pUFO->SetDestination(pPlanet);
        pUFO->mVec6bc = pUFO->mVec718;
        mNPC_UFOs.PushBack(pUFO);
    }

    if (pPlanet != nullptr)
        pPlanet->ReleaseObject();
}
