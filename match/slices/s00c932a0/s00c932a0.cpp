// @ 0x00c932a0
// 0x00c932a0 CreateTribeForSPG
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// SP::CreateTribeForSPG -- create the SPG player tribe noun, bind it to the
// scenario and species profile, name it after the species, place its starting
// tools on the planet surface and register it with the terraforming manager.
//
// Slice s00c932a0.  The object the noun manager returns is reached through the
// noun interface (0x4f396a66); the member offsets below are the ones the retail
// function uses.  Fields the function does not touch are kept as padding.
typedef unsigned int uint32_t;

// Vec3 is a small value class, not a bare aggregate: retail constructs the
// struct a spatial getter returns directly in the caller's slot (the callee
// fills the buffer passed as the hidden return pointer and the caller reads it
// through eax; no copy is made).  A trivially copyable aggregate makes cl 15
// copy the temporary into a second slot instead, which also renumbers every
// stack slot in the frame.  The operators likewise have to be inlined so that
// "pBase + facing * scale" produces retail's per-component mulss/addss chain.
struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(const Vec3& o) { x = o.x; y = o.y; z = o.z; }
};
Vec3 operator*(const Vec3& v, float s) {
    Vec3 r;
    r.x = v.x * s; r.y = v.y * s; r.z = v.z * s;
    return r;
}
Vec3 operator+(const Vec3& a, const Vec3& b) {
    Vec3 r;
    r.x = a.x + b.x; r.y = a.y + b.y; r.z = a.z + b.z;
    return r;
}
struct Quat {
    float x, y, z, w;
};

// eastl::basic_string<wchar_t> has three pointers: begin, end, capacity.
struct WString {
    void* mpBegin;
    void* mpEnd;
    void* mpCapacity;
};

extern wchar_t gEmptyString16[];   // 0x01667bac eastl::gEmptyString16
extern float   gSpawnOffsetScale;  // 0x013f8698 scale applied to the hut facing

// ---------------------------------------------------------------------------
// External callees (addresses from the original call sites; the call
// relocations are masked by the checker, the calling shape is what matters).
// ---------------------------------------------------------------------------
void* __cdecl FUN_00401090();       // cSPEditorSpeciesManager accessor
void* __cdecl FUN_01021090();       // SP::cSPLivingUniverse::GetPlayerEmpireID
void* __cdecl FUN_00b3d300();       // SP::NounManager
void* __cdecl FUN_01021260();       // SP::cSPLivingUniverse::GetActivePlanet
void* __cdecl FUN_00b6e050();       // allocates the tribe planner
void* __cdecl FUN_00b3d350();       // SP::PlanetModel
void* __cdecl FUN_00b3d430();       // SP::TerraformingManager
void* __cdecl FUN_010407c0();       // current app mode
void  __cdecl FUN_00f47380(void*);  // operator delete[]

struct cSpeciesProfile;
struct cSPEditorSpeciesManager {
    const cSpeciesProfile* GetAvatarProfile() const;  // 0x004df420
};
struct cSpeciesProfile {
    void GetUiName(WString*) const;                   // 0x004da330
};
struct NounManager {
    void* CreateNoun(uint32_t classId) const;         // 0x00b20c60
};
// The scenario object only exposes these four accessors to this function.
struct Scenario {
    int GetA();       // 0x00fc8380
    int GetB(int);    // 0x00ff0420
    int GetC(int);    // 0x007f54d0
    int GetD(int);    // 0x006e64f0
};
struct RandomGlobal {
    uint32_t RandomUint32Uniform(uint32_t range) const;  // 0x00a68fb0
};
extern RandomGlobal gRandomGlobal;  // 0x01601760 cRandomGlobal
struct PlanetModel {
    Vec3 DirectionToSurfacePosition(const Vec3&) const;  // 0x00b815a0
    Quat BuildSurfaceOrientation(const Vec3&) const;     // 0x00b7f190
};
struct TerraformingManager {
    void Update();  // 0x00bbc3f0
};

// Virtual dispatch through raw vtable offsets (the headers do not describe the
// retail vtable for every one of these interfaces).
#define VF(p, off) (*(void**)((char*)(*(void**)p) + (off)))

// +0x230: name holder of the tribe (0x00b6f380 LoadNames).
struct NameHolder {
    void LoadNames(WString*);
};
// +0x238: id -> entry map used by the tribe (0x00e5c780 rbtree find).
struct Node;
struct KeyMap {
    Node** find(Node*** pOut, const uint32_t& key);
};

struct Tribe {
    char pad000[0x2d8];
    Scenario* mpScenario;                  // +0x2d8
    char pad2dc[0x18cc - 0x2dc];
    const cSpeciesProfile* mpSpecies;      // +0x18cc
    void* GetToolOfType(int type) const;   // 0x00c8f6e0
};

namespace SP {

Tribe* CreateTribeForSPG(Scenario* pScenario, const cSpeciesProfile* pSpecies) {
    if (pScenario == 0)
        return 0;

    if (pSpecies == 0) {
        pSpecies = ((cSPEditorSpeciesManager*)FUN_00401090())->GetAvatarProfile();
        if (pSpecies == 0)
            return 0;
    }

    Tribe* pTribe = 0;
    FUN_01021090();  // player empire id (result not needed here)
    NounManager* pNouns = (NounManager*)FUN_00b3d300();
    void* pNoun = pNouns->CreateNoun(0x18c6d19u);
    if (pNoun == 0)
        return pTribe;

    pTribe = (Tribe*)((void* (__thiscall*)(void*, uint32_t))VF(pNoun, 0x0c))(
        pNoun, 0x4f396a66u);
    if (pTribe == 0)
        return pTribe;

    pTribe->mpScenario = pScenario;
    FUN_01021260();  // active planet
    pTribe->mpSpecies = pSpecies;

    ((void (__thiscall*)(void*, int))VF(pTribe, 0x48))(pTribe, pScenario->GetA());
    *(void**)((char*)pTribe + 0x234) = FUN_00b6e050();

    int a = pScenario->GetB(1);
    int b = pScenario->GetC(a);
    int c = pScenario->GetD(b);
    ((void (__thiscall*)(void*, int))VF(pTribe, 0x8c))(pTribe, c);

    WString name;
    name.mpBegin = gEmptyString16;
    name.mpEnd = gEmptyString16;
    name.mpCapacity = gEmptyString16 + 1;
    pSpecies->GetUiName(&name);
    ((NameHolder*)((char*)pTribe + 0x230))->LoadNames(&name);

    uint32_t key = 0;
    Node** pOut;
    Node** pFound = ((KeyMap*)((char*)pTribe + 0x238))->find(&pOut, key);
    char* pFoundNode = (char*)*pFound;
    int entry = *(int*)(pFoundNode + 0x14);
    void* pTribeInner = (char*)pTribe + 0x34;
    ((void (__thiscall*)(void*, int))VF(pTribeInner, 0))((char*)pTribe + 0x34, entry);

    // The first fire tool: stand it on the surface next to the tribe.
    void* pFire = pTribe->GetToolOfType(10);
    if (pFire == 0) {
        pFire = ((void* (__thiscall*)(void*, int))VF(pTribe, 0xb4))(pTribe, 10);
        if (pFire != 0) {
            ((void (__thiscall*)(void*, void*))VF(pFire, 0x64))(pFire, pTribe);

            void* pA = ((void* (__thiscall*)(void*))VF(pTribe, 0xac))(pTribe);
            void* pB = ((void* (__thiscall*)(void*))VF(pTribe, 0xac))(pTribe);
            void* pSpatialA = (char*)pA + 0x34;
            void* pSpatialB = (char*)pB + 0x34;

            Vec3 offset =
                ((Vec3 (__thiscall*)(void*))VF(pSpatialA, 0x5c))(pSpatialA) * gSpawnOffsetScale;

            const Vec3* pBase =
                (const Vec3*)((void* (__thiscall*)(void*))VF(pSpatialB, 0x2c))(pSpatialB);

            Vec3 target = *pBase + offset;

            PlanetModel* pPlanet = (PlanetModel*)FUN_00b3d350();
            Vec3 surface = pPlanet->DirectionToSurfacePosition(target);
            Quat orient =
                ((PlanetModel*)FUN_00b3d350())->BuildSurfaceOrientation(surface);

            void* pSpatialTool = (char*)pFire + 0x34;
            ((void (__thiscall*)(void*, const Vec3*))VF(pSpatialTool, 0x38))(
                pSpatialTool, &surface);
            ((void (__thiscall*)(void*, const Quat*))VF(pSpatialTool, 0x3c))(
                pSpatialTool, &orient);
        }
    }

    if (*(unsigned char*)((char*)FUN_010407c0() + 0x4c) != 0) {
        void* pFireTool = pTribe->GetToolOfType(10);

        if (pTribe->GetToolOfType(8) == 0) {
            void* pTool8 = ((void* (__thiscall*)(void*, int))VF(pTribe, 0xb4))(pTribe, 8);
            if (pTool8 != 0) {
                ((void (__thiscall*)(void*, void*))VF(pTool8, 0x64))(pTool8, pTribe);
                void* pDstSpatial = (char*)pTool8 + 0x34;
                void* pSrcSpatial = (char*)pFireTool + 0x34;
                const Vec3* pPos =
                    (const Vec3*)((void* (__thiscall*)(void*))VF(pSrcSpatial, 0x2c))(
                        pSrcSpatial);
                ((void (__thiscall*)(void*, const Vec3*))VF(pDstSpatial, 0x38))(
                    pDstSpatial, pPos);
                const Quat* pOri =
                    (const Quat*)((void* (__thiscall*)(void*))VF(pSrcSpatial, 0x30))(
                        pSrcSpatial);
                ((void (__thiscall*)(void*, const Quat*))VF(pDstSpatial, 0x3c))(
                    pDstSpatial, pOri);
            }
        }

        if (pTribe->GetToolOfType(1) == 0 && pTribe->GetToolOfType(2) == 0 &&
            pTribe->GetToolOfType(3) == 0) {
            int type;
            switch (gRandomGlobal.RandomUint32Uniform(3)) {
                case 0:
                    type = 1;
                    break;
                case 1:
                    type = 2;
                    break;
                case 2:
                    type = 3;
                    break;
                default:
                    goto done;
            }
            void* pNewTool = ((void* (__thiscall*)(void*, int))VF(pTribe, 0xb4))(pTribe, type);
            if (pNewTool != 0) {
                ((void (__thiscall*)(void*, void*))VF(pNewTool, 0x64))(pNewTool, pTribe);
                void* pDstSpatial = (char*)pNewTool + 0x34;
                void* pSrcSpatial = (char*)pFireTool + 0x34;
                const Vec3* pPos =
                    (const Vec3*)((void* (__thiscall*)(void*))VF(pSrcSpatial, 0x2c))(
                        pSrcSpatial);
                ((void (__thiscall*)(void*, const Vec3*))VF(pDstSpatial, 0x38))(
                    pDstSpatial, pPos);
                const Quat* pOri =
                    (const Quat*)((void* (__thiscall*)(void*))VF(pSrcSpatial, 0x30))(
                        pSrcSpatial);
                ((void (__thiscall*)(void*, const Quat*))VF(pDstSpatial, 0x3c))(
                    pDstSpatial, pOri);
            }
        }
    }

done:
    ((TerraformingManager*)FUN_00b3d430())->Update();

    // eastl::basic_string's destructor frees a heap buffer it owns.  The length
    // is compared as a *signed* int: retail ends the test with jle, not jbe.
    int nameBytes = (int)((char*)name.mpEnd - (char*)name.mpBegin) & ~1;
    if (nameBytes > 2 && name.mpBegin != 0)
        FUN_00f47380(name.mpBegin);

    return pTribe;
}

}  // namespace SP
