// s01057d50.cpp - reconstructed slice
// Compiler: MSVC 2008 SP1 (x86)
// Flags: /O2 /Ob1 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-
//   (/fp:fast is required: retail uses SSE scalar addss/mulss/comiss here, while the default
//    fp model makes cl 15 emit x87 fadd/fmul for the same expressions.)
// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------
typedef unsigned char      u8;
typedef unsigned int       u32;
typedef int                i32;
typedef float              f32;

// Object with a virtual method table (only the vtable pointer matters here).
struct cVObj {
    void** vftable;                     // 0x00
};

// Space-token translator global (0x16e0d08).
struct cSpaceTokenTranslator {
    u8    mPad00[0x14];
    i32   mField14;                     // 0x14
    u8    mPad18[0x24 - 0x18];
    void* mpField24;                    // 0x24 (species profile)
};
extern "C" char g_spaceTokenTranslator[];    // 0x16e0d08

// Creature-base object: embedded species-profile sub-record and vtable.
struct cSpeciesSubRec {
    u8    mPad000[0x504];
};
struct cSPCreatureBase {
    void** vftable;                     // 0x00
    u8     mPad04[0x0b20 - 0x04];
    void*  mpField0b20;                 // 0x0b20 (active-planet-ish object)

    void* GetSpeciesRecord();           // 0x00c0bbd0 -> returns species profile
};

// Species profile handed back by GetSpeciesRecord.
struct cSpeciesProfile {
    u8    mPad000[0x504];
    i32   mField504;                    // 0x504
    i32   mField508;                    // 0x508
    i32   mField50c;                    // 0x50c
};

// ---------------------------------------------------------------------------------------------
// Opaque extern callees referenced by absolute address.
// ---------------------------------------------------------------------------------------------
void __cdecl NotifyScannedCreatureSpecies(void* speciesRec, i32 flag);   // 0x01058460
void* __cdecl GetActivePlanet();                                         // 0x01021260

// Text-helper object with a cString at a given sub-offset.
struct cTextHolder {
    u8 mPad[4];
};
wchar_t* __cdecl TextHolder_GetText(void* holder);                       // 0x006b55c0

void* __cdecl GetIconTrayObj();                                          // 0x010666a0

// ShowScanEventLog(v3, v1, v2, fn, text) -- 5 dwords passed by value/push.
void __cdecl ShowScanEventLog(i32 v3, i32 v1, i32 v2, void* fn, void* text);   // 0x010531b0

struct cPlanetVt {
    void** vftable;
};

// Vtable slot call: slot index = byteoffset/4. Returns u32.
static inline u32 VT_0C_0(void* p, void* arg)
{
    void** vt = *(void***)p;
    void* fn = vt[3];
    return ((u32 (__thiscall*)(void*, void*))fn)(p, arg);
}

u8   __cdecl cPlanet_SetSpeciesAsScanned(void* planet, void* sub);         // 0x00c73ea0
void __cdecl cPlanet_ResetSpecies(void* planet, void* sub, i32 a, i32 b);  // 0x00c74690
void* __cdecl Text_GetText(void* strObj);                                  // 0x006b55c0

// ---------------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------------
// @ 0x01058660
// Forwards to NotifyScannedCreatureSpecies with the object's embedded species record.
void __cdecl ScanBuildingSpecies(void* obj, i32 flag)
{
    NotifyScannedCreatureSpecies((char*)*(void**)((char*)obj + 0x0b20) + 0x504, flag);
}

// @ 0x01058680
// Handles the "scan building" action for a creature base object.
extern "C" u32 __cdecl ScanBuilding(cSPCreatureBase* creature)
{
    u32 ok = 0;
    i32 v1;
    i32 v2;
    if (creature != 0) {
        if (VT_0C_0(creature, (void*)0xd0036e08) != 0) {
            void* planet = GetActivePlanet();
            cSpeciesProfile* species = (cSpeciesProfile*)creature->GetSpeciesRecord();
            cSpeciesSubRec* sub = (cSpeciesSubRec*)((char*)species + 0x504);

            v1 = species->mField508;
            v2 = species->mField50c;
            i32 v3 = species->mField504;

            char* tok = *(char**)g_spaceTokenTranslator;
            *(i32*)(tok + 0x14) = 0;
            *(void**)(tok + 0x24) = species;

            if (cPlanet_SetSpeciesAsScanned(planet, sub) == 0) {
                cPlanet_ResetSpecies(planet, sub, 1, 1);
                void* tray = GetIconTrayObj();
                void* text = Text_GetText((char*)tray + 0x44);
                ShowScanEventLog(v3, v1, v2, (void*)0x01052fd0, text);
                NotifyScannedCreatureSpecies((char*)creature->mpField0b20 + 0x504, 1);
                ok = 1;
            } else {
                void* tray = GetIconTrayObj();
                void* text = Text_GetText((char*)tray + 0x58);
                ShowScanEventLog(v3, v1, v2, 0, text);
            }
        }
    }
    return ok;
}

// ---------------------------------------------------------------------------------------------
// 0x01057d50 -- SP::cSPArtilleryWeaponStrategy::StartTool
//
// Retail debug name: SP::cSPArtilleryWeaponStrategy::StartTool.  Like its siblings
// (StartTool_568b0 and the other tool-strategy starters) it is compiled with the strategy
// pointer as the first stack argument, so it is written here as a __stdcall helper.
// ---------------------------------------------------------------------------------------------
struct cVec3 {
    f32 x;                                          // +0x00
    f32 y;                                          // +0x04
    f32 z;                                          // +0x08

    cVec3() {}
    cVec3(f32 ax, f32 ay, f32 az) : x(ax), y(ay), z(az) {}
    cVec3(const cVec3& v) : x(v.x), y(v.y), z(v.z) {}
    cVec3& operator=(const cVec3& v) { x = v.x; y = v.y; z = v.z; return *this; }
    cVec3 operator+(const cVec3& b) const { return cVec3(x + b.x, y + b.y, z + b.z); }
    cVec3 operator*(f32 s) const { return cVec3(x * s, y * s, z * s); }
};

struct cBox {
    cVec3 mMin;                                     // +0x00
    cVec3 mMax;                                     // +0x0c
};

struct cSphere {
    cVec3 mCenter;                                  // +0x00
    f32   mfRadius;                                 // +0x0c

    cSphere(const cVec3& center, f32 fRadius) : mCenter(center), mfRadius(fRadius) {}
};

extern const cVec3  gZeroPoint;                     // 0x016e1b14 (0, 0, 0)
extern const f32    kHalf;                          // 0x01471064 (0.5f)

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

class cSpatialObject;

// Interface returned by casting the strategy's owner object (0x13f94d4).
class cOwnerObject {
public:
    virtual void v00();                             // +0x00
    virtual void v01();                             // +0x04
    virtual cSpatialObject* GetSpatial();           // +0x08
    virtual void v03();                             // +0x0c
    virtual i32 GetIndex();                         // +0x10
};

class cSpatialObject {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v0a();
    virtual void v0b();
    virtual void v0c();
    virtual void v0d();
    virtual void v0e();
    virtual void v0f();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v1a();
    virtual const cBox* GetBoundingBox(cBox* pOut); // +0x6c
};

// Ammo record looked up through the noun manager; only its amount field is touched.
class cAmmoObject {
public:
    u8  mPad000[0x98];                              // +0x00
    f32 mfAmount;                                   // +0x98

    void SetAmount(f32 fAmount);                    // 0x00bef710
};

class cGameNounManager {
public:
    cAmmoObject* GetAmmoObject(i32 nIndex);         // 0x00b25f40
    void*        GetType(u32 nTypeID);              // 0x00b20c60
};

class cEffectsManager {
public:
    f32 GetEffectValue();                           // 0x00b7e490
};

// Type object fetched for 0x18c9380; slot +0x0c creates the effect instance for 0x1428a48.
class cTypeFactory {
public:
    virtual void v00();                             // +0x00
    virtual void v01();                             // +0x04
    virtual void v02();                             // +0x08
    virtual void* Create(u32 nTypeID);              // +0x0c
};

class cEffectInstance {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v0a();
    virtual void v0b();
    virtual void v0c();
    virtual void v0d();
    virtual void v0e();
    virtual void v0f();
    virtual void v10();
    virtual void v11();
    virtual void Attach(void* pOwner);              // +0x48
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void Start(void* pSelf, i32 nSecond, i32 nThird, u32 nMode, void* pOwner);  // +0x58
    void SetPosition(const cVec3* pPosition);       // 0x00cb7040
};

// Transform component embedded at +0x34 of the effect instance.
class cEffectXform {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v0a();
    virtual void v0b();
    virtual void v0c();
    virtual void v0d();
    virtual void SetPosition(const cVec3* pPosition);   // +0x38
};

// The tool strategy object (this function only reads +0x114 directly).
class cStrategy {
public:
    virtual void v00();                             // +0x00
    u32 GetClassID();                               // 0x00ce6950
    f32 GetField1b4();                              // 0x00ce6940 (fld [this+0x1b4])
    i32 GetAmmoMax();                               // 0x01050020
    f32 GetPrimaryRadius();                         // 0x0104be30
    f32 GetField1b8();                              // 0x0104bdc0 (fld [this+0x1b8])

    u8  mPad004[0x114 - 0x004];                     // +0x004
    cOwnerObject* mpOwnerObject;                    // +0x114
};

// +0xb8 slot of the owner-object interface: cast a component to 0x13f94d4.
static inline void* CastOwnerObject(void* pObject, u32 nTypeID)
{
    void* pfn = ((void**) *(void***)pObject)[0xb8 / 4];
    return ((void* (__thiscall*)(void*, u32))pfn)(pObject, nTypeID);
}

bool __stdcall FUN_010568b0(cStrategy* pSelf, const cVec3* pPosition, i32 nFlags);   // 0x010568b0
cGameNounManager* __cdecl NounManager();                                            // 0x00b3d300
cEffectsManager* __cdecl EffectsManager();                                          // 0x00b3d350
bool __cdecl FUN_00cb46f0(cVec3 center, cSphere sphere, cVec3* pOut, cVec3 origin);  // 0x00cb46f0

// @ 0x01057d50
// Starts the artillery strike tool: refreshes the owner's ammunition counter, works out
// where on the owner the shot originates and spawns the strike effect there.
bool __stdcall StartTool_57d50(cStrategy* pSelf, const cVec3* pPosition, i32 nFlags)
{
    if (FUN_010568b0(pSelf, pPosition, nFlags)) {
        if (!pSelf->mpOwnerObject)
            return false;
        cOwnerObject* pOwner = (cOwnerObject*)CastOwnerObject(pSelf->mpOwnerObject, 0x13f94d4);
        if (!pOwner)
            return false;
        {
            u32 nClass = pSelf->GetClassID();
            if (nClass == 0x8a8b6f35 || nClass == 0x94e412ed || nClass == 0x1d128dd5) {
                if (pSelf->GetAmmoMax()) {
                    if (pOwner->GetIndex() != -1) {
                        cAmmoObject* pAmmo = NounManager()->GetAmmoObject(pOwner->GetIndex());
                        if (pAmmo) {
                            f32 fAmount = pAmmo->mfAmount;
                            if (fAmount >= (f32)pSelf->GetAmmoMax())
                                pAmmo->SetAmount((f32)pSelf->GetAmmoMax());
                        }
                    }
                }

                cBox box;
                cSpatialObject* pSpatial = pOwner->GetSpatial();
                const cBox* pBounds = pSpatial->GetBoundingBox(&box);

                cVec3 center = (pBounds->mMin + pBounds->mMax) * kHalf;

                cVec3 position = *pPosition;
                f32 fScale = EffectsManager()->GetEffectValue();
                f32 fRadius = (f32)sqrt(-(pSelf->GetPrimaryRadius() * fScale));

                cSphere sphere(position, fRadius);
                cVec3 spawnPos;
                if (FUN_00cb46f0(center, sphere, &spawnPos, gZeroPoint)) {
                    cTypeFactory* pFactory = (cTypeFactory*)NounManager()->GetType(0x18c9380);
                    cEffectInstance* pEffect = pFactory ? (cEffectInstance*)pFactory->Create(0x1428a48) : 0;

                    u32 nMode;
                    if (pSelf->GetClassID() == 0x8a8b6f35)
                        nMode = 0;
                    else
                        nMode = (pSelf->GetClassID() != 0x94e412ed) ? 4 : 3;

                    pEffect->Start(pSelf,
                                   (i32)pSelf->GetField1b4(),
                                   (i32)pSelf->GetField1b8(),
                                   nMode,
                                   pOwner);

                    ((cEffectXform*)((char*)pEffect + 0x34))->SetPosition(&center);
                    pEffect->Attach((void*)pOwner->GetIndex());
                    pEffect->SetPosition(&spawnPos);
                }
            }
        }
    }
    return true;
}
