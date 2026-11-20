// Decompiled slice s00c5e260 -- Simulator/SP mission objects.
// Reconstructed from the shipped binary; helper addresses noted per call.
#include "types.h"

// ---------------------------------------------------------------------------
// shared shapes
// ---------------------------------------------------------------------------
struct Vtbl { void* p[64]; };

typedef int   (__thiscall* FnI0)(void*);
typedef char  (__thiscall* FnC0)(void*);
typedef void* (__thiscall* FnP1)(void*, int);
typedef void  (__thiscall* FnV1)(void*, int);
typedef void  (__thiscall* FnV2)(void*, void*, int);
typedef void  (__thiscall* FnV3)(void*, void*, int, int);
typedef void  (__thiscall* FnVoid)(void*);

struct Planet {
  uint8_t mPad000[0x13c];
  int     mField13c;      // +0x13c
};

struct PlanetFilter {
  int  mField0;           // +0x00
  int  mPad04[0x11];      // +0x04 .. +0x47
  void FUN_00ba7bd0();    // 0x00ba7bd0 (constructor)
  void FUN_00ba8220();    // 0x00ba8220 (destructor)
};

struct TargetObj {
  void FUN_00c74690();    // 0x00c74690
};

struct Item {
  int mField0;            // +0x00
  int mField4;            // +0x04
  int mField8;            // +0x08
};

struct Inventory {
  void FUN_00ff45e0(Item* item);  // 0x00ff45e0
};

struct cSPSimulatorSpaceGame {
  Inventory* GetPlayerInventory(); // 0x00a1ad60
};

struct cGameNounManager {
  void* GetPlayerTribe();          // 0x00bfc5f0
};

struct TribeObj {
  void FUN_00fe5430();             // 0x00fe5430
};

// Small vector returned by the +0x180 object's slot 0x28 method.
struct SomeVec {
  void*    mpField0;      // +0x00
  int      mPad04;        // +0x04
  int      mField8;       // +0x08
  int      mPad0C;        // +0x0c
  uint16_t mField10;      // +0x10
  uint16_t mField12;      // +0x12
};

// ---------------------------------------------------------------------------
// mission base object (cGenericPressureEvent layout shared by both missions)
// ---------------------------------------------------------------------------
struct Mission {
  Vtbl*   mpVtable;       // +0x000
  void*   mpField04;      // +0x004
  uint8_t mPad008[0x2c];  // +0x008 .. +0x033
  void*   mpField34;      // +0x034
  uint8_t mPad038[0x40];  // +0x038 .. +0x077
  int     mField078;      // +0x078
  uint8_t mPad07C[0xb4];  // +0x07c .. +0x12f
  uint32_t mField130;     // +0x130
  uint8_t mPad134[0x3c];  // +0x134 .. +0x16f
  uint8_t mField170[0x10];// +0x170 .. +0x17f
  void*   mpField180;     // +0x180
  uint8_t mPad184[0x6c];  // +0x184 .. +0x1ef
  int     mField1f0;      // +0x1f0
  void*   mpField1f4;     // +0x1f4
  int     mField1f8;      // +0x1f8
  int     mField1fc;      // +0x1fc

  // base helpers (addresses from the shipped binary)
  void  TriggerUIUpdate();        // 0x00c47d80
  void* GetTargetPlanet();        // 0x00970b30
  void  FUN_00c44b80();           // 0x00c44b80
  void  FUN_00c44bd0();           // 0x00c44bd0
  void  FUN_00c49f90();           // 0x00c49f90
  void  FUN_00c472e0();           // 0x00c472e0
  void  FUN_00c471c0();           // 0x00c471c0
  int   FUN_00c44cb0();           // 0x00c44cb0
  bool  IsActive();               // 0x00c44c80
  void  GetText(int id, int arg); // 0x00c487d0
  void  SetSerializer();          // 0x00c2e4e0
  bool  Init();                   // 0x00c4ad50
  int   ReadTargetPlanetType();   // 0x00c45b40
  void* GetPlanetRecordFromTargetType(int t); // 0x00c474b0
  void  FUN_00c47180(void* p);    // 0x00c47180
  int   ReadTargetAnimalSpeciesType(void* p); // 0x00c45b60
  void* GetAnimalSpeciesFromType(int t);      // 0x00c47610
  void* GetTargetPlanet(void* p, int a, int b); // 0x00970b30 (3-arg)
  void* PickPlanet(void* filter); // 0x00c47e20
  int   FUN_00c45b40();           // 0x00c45b40 (alias: ReadTargetPlanetType)
  void  FUN_00c4a0c0();           // 0x00c4a0c0
  char  FUN_00c5ea60();           // 0x00c5ea60 (see body)

  void  FUN_00c5ed90(int v);      // 0x00c5ed90
  void  FUN_00c5edb0();           // 0x00c5edb0
  void  FUN_00c5ee40();           // 0x00c5ee40
  void  FUN_00c5eed0(int arg);    // 0x00c5eed0
  void  FUN_00c5efb0(int arg);    // 0x00c5efb0
  int   FUN_00c5f090();           // 0x00c5f090
  int   FUN_00c5f0f0(int a, int b);// 0x00c5f0f0
  int   FUN_00c5f150();           // 0x00c5f150
  void  FUN_00c5f250();           // 0x00c5f250
  void  FUN_00c5f280();           // 0x00c5f280
  void  FUN_00c5f2c0();           // 0x00c5f2c0
  void  FUN_00c5f360();           // 0x00c5f360
  int   FUN_00c5f3d0();           // 0x00c5f3d0
  int   FUN_00c5f4e0(int id);     // 0x00c5f4e0
};

struct MissionScan : Mission {
  MissionScan();                  // 0x00c5e740
};

struct MissionStory201 : Mission {
  MissionStory201();              // 0x00c5f1b0
};

namespace SP {
  void* __stdcall SpaceGameGet(int a, int b); // 0x01002bd0
  void* __stdcall SpaceGameGet();             // 0x01002bd0 (no-arg overload)
}

void* MessageServer();              // 0x0067dcc0
void  FUN_00c47a10();               // 0x00c47a10
void* FUN_01021370();               // 0x01021370
int   FUN_01021240();               // 0x01021240
int   FUN_01021080();               // 0x01021080

struct HomePlanetObj {
  int FUN_00b8de30();               // 0x00b8de30
};

// The shared base constructor lives outside this translation unit.
void __fastcall MissionBaseCtor(void* self);

// ===========================================================================
// 0x00c5e740 -- Simulator::cMissionScan::cMissionScan
// ===========================================================================
// @ 0x00c5e740
MissionScan::MissionScan()
{
  MissionBaseCtor(this);
  mpVtable  = (Vtbl*)0x146fc00;
  mpField04 = (void*)0x146dfb4;
  mpField34 = (void*)0x146fbf0;
  mField1f0 = 0;
  mpField1f4 = (void*)*(int*)0x16925c0;
  mField1f8 = *(int*)0x16925c4;
  mField1fc = *(int*)0x16925c8;
}

// ===========================================================================
// 0x00c5ea60 -- SP::cSPMissionScan::Init
// ===========================================================================
// @ 0x00c5ea60
char Mission::FUN_00c5ea60()
{
  if (!Init())
    return 0;

  int t = FUN_00c45b40();
  void* rec = GetPlanetRecordFromTargetType(t);
  if (rec != 0)
    FUN_00c47180(rec);

  uint32_t raw = *(volatile uint32_t*)&mField130;
  raw >>= 3;
  if ((raw & 1) != 0 && (uint32_t)mField078 == 0x89238409u) {
    void* p = (void*)((char*)this + 0x170);
    int species = ReadTargetAnimalSpeciesType(p);
    GetAnimalSpeciesFromType(species);
    void* obj = GetTargetPlanet(p, 0, 0);
    ((TargetObj*)obj)->FUN_00c74690();
  }

  if (GetTargetPlanet() == 0) {
    PlanetFilter filter;
    filter.FUN_00ba7bd0();
    filter.mField0 = 0x38;
    void* picked = PickPlanet(&filter);
    if (picked == 0) {
      filter.FUN_00ba8220();
      return 0;
    }
    FUN_00c47180(picked);
    filter.FUN_00ba8220();
  }

  if (((FnC0)mpVtable->p[0x128 / 4])(this) == '\0')
    return 0;
  FUN_00c4a0c0();
  ((FnVoid)mpVtable->p[0x170 / 4])(this);
  return 1;
}
// ===========================================================================
// 0x00c5ed90 -- set the mission state and refresh the UI
// ===========================================================================
// @ 0x00c5ed90
void Mission::FUN_00c5ed90(int v)
{
  if (mField1f0 != v) {
    mField1f0 = v;
    TriggerUIUpdate();
  }
}

// ===========================================================================
// 0x00c5edb0 -- install six message handlers, then register with the server
// ===========================================================================
// @ 0x00c5edb0
void Mission::FUN_00c5edb0()
{
  FUN_00c44b80();
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0x248975f);
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0x2489766);
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0x248976a);
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0x44f1189);
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0xf46093da);
  ((FnV1)mpVtable->p[0x14c / 4])(this, 0x6441936);

  void* dst = (void*)&mpField34;
  void* ms = MessageServer();
  ((FnV2)(*(Vtbl**)ms)->p[0x20 / 4])(ms, dst, 0x2489760);
}

// ===========================================================================
// 0x00c5ee40 -- remove six message handlers, then deregister
// ===========================================================================
// @ 0x00c5ee40
void Mission::FUN_00c5ee40()
{
  FUN_00c44bd0();
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0x248975f);
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0x2489766);
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0x248976a);
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0x44f1189);
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0xf46093da);
  ((FnV1)mpVtable->p[0x150 / 4])(this, 0x6441936);

  void* dst = (void*)&mpField34;
  void* ms = MessageServer();
  ((FnV3)(*(Vtbl**)ms)->p[0x2c / 4])(ms, dst, 0x2489760, 0xffffd8f1);
}

// ===========================================================================
// 0x00c5eed0 -- medium UI description
// ===========================================================================
// @ 0x00c5eed0
void Mission::FUN_00c5eed0(int arg)
{
  if (!IsActive())
    return;

  switch (mField1f0) {
  case 0:
    GetText(0xb98c20c5, arg);
    return;
  case 1:
    GetText(0x2f8f74df, arg);
    return;
  case 2: {
    void* p = FUN_01021370();
    int a = ((HomePlanetObj*)p)->FUN_00b8de30();
    int b = FUN_01021240();
    if (a == b && FUN_01021080() != 2)
      GetText(0xed4b67a7, arg);
    else
      GetText(0xd0fbdf85, arg);
    return;
  }
  case 3:
    GetText(0xa29d2048, arg);
    return;
  case 4:
    GetText(0x81128ae1, arg);
    return;
  }
}

// ===========================================================================
// 0x00c5efb0 -- long UI description
// ===========================================================================
// @ 0x00c5efb0
void Mission::FUN_00c5efb0(int arg)
{
  if (!IsActive())
    return;

  switch (mField1f0) {
  case 0:
    GetText(0xda9e8dc6, arg);
    return;
  case 1:
    GetText(0xc28f4e24, arg);
    return;
  case 2: {
    void* p = FUN_01021370();
    int a = ((HomePlanetObj*)p)->FUN_00b8de30();
    int b = FUN_01021240();
    if (a == b && FUN_01021080() != 2)
      GetText(0xc404af28, arg);
    else
      GetText(0x48cc6b76, arg);
    return;
  }
  case 3:
    GetText(0x2dc5caa7, arg);
    return;
  case 4:
    GetText(0xba93258e, arg);
    return;
  }
}

// ===========================================================================
// 0x00c5f090 -- conversation group id for the current state
// ===========================================================================
// @ 0x00c5f090
int Mission::FUN_00c5f090()
{
  if (((FnI0)mpVtable->p[0x60 / 4])(this) == 3) {
    switch (mField1f0) {
    case 0: return 0x2864b01f;
    case 1: return 0x0501dd96;
    case 2: return 0x1d3ce6e6;
    case 3: return 0x7b0daf98;
    case 4: return 0x11965a4d;
    }
  }
  return FUN_00c44cb0();
}

// ===========================================================================
// 0x00c5f0f0 -- dynamic response id for the current state
// ===========================================================================
// @ 0x00c5f0f0
int Mission::FUN_00c5f0f0(int a, int b)
{
  if (((FnI0)mpVtable->p[0x60 / 4])(this) == 3) {
    switch (mField1f0) {
    case 0: return 0xcb268f51;
    case 1: return 0x1e1135ac;
    case 2: return 0xc04fa4e8;
    case 3: return 0x2d28ce76;
    }
  }
  return -1;
}

// ===========================================================================
// 0x00c5f150 -- true while the mission is in state 0 or 1
// ===========================================================================
// @ 0x00c5f150
int Mission::FUN_00c5f150()
{
  if (mField1f0 != 0 && mField1f0 != 1)
    return 0;
  return 1;
}

// ===========================================================================
// 0x00c5f190 -- release the player tribe's game data
// ===========================================================================
// @ 0x00c5f190
void FUN_00c5f190()
{
  FUN_00c47a10();
  cGameNounManager* m = (cGameNounManager*)SP::SpaceGameGet(0x11, 1);
  TribeObj* t = (TribeObj*)m->GetPlayerTribe();
  t->FUN_00fe5430();
}

// ===========================================================================
// 0x00c5f1b0 -- Simulator::cMissionStory201::cMissionStory201
// ===========================================================================
// @ 0x00c5f1b0
MissionStory201::MissionStory201()
{
  MissionBaseCtor(this);
  int zero = 0;
  *(volatile int*)&mField1f0 = zero;
  *(volatile int*)&mpVtable  = (int)0x146fe60;
  *(volatile int*)&mpField04 = (int)0x146fe4c;
  *(volatile int*)&mpField34 = (int)0x146fe3c;
  *(volatile int*)&mpField1f4 = zero;
  *(volatile int*)&mField1f8 = zero;
  *(volatile int*)&mField1fc = zero;
}

// ===========================================================================
// 0x00c5f250 -- release the target planet object, then reset
// ===========================================================================
// @ 0x00c5f250
void Mission::FUN_00c5f250()
{
  void* p = mpField1f4;
  if (p != 0) {
    mpField1f4 = 0;
    ((FnVoid)(*(Vtbl**)p)->p[0xc0 / 4])(p);
  }
  FUN_00c49f90();
}

// ===========================================================================
// 0x00c5f280 -- SP::cSPMissionStory201::ReturnForRewards
// ===========================================================================
// @ 0x00c5f280
void Mission::FUN_00c5f280()
{
  if (mField1f0 != 4) {
    ((FnVoid)mpVtable->p[0x6c / 4])(this);
    if (mField1f0 != 4) {
      mField1f0 = 4;
      TriggerUIUpdate();
    }
    FUN_00c472e0();
    FUN_00c471c0();
  }
}

// ===========================================================================
// 0x00c5f2c0 -- give every entry of the player's inventory reward item
// ===========================================================================
// @ 0x00c5f2c0
void Mission::FUN_00c5f2c0()
{
  void* obj = mpField180;
  SomeVec* v = (SomeVec*)((FnP1)(*(Vtbl**)obj)->p[0x28 / 4])(obj, 0xedcaee05);

  uint16_t mask = (uint16_t)(v->mField10 & 0x30);
  int count;
  int* items;

  if (mask != 0) {
    count = v->mField8;
    if (count <= 0)
      return;
  } else {
    if (v->mField12 == 0)
      return;
    count = 1;
  }

  if (mask != 0)
    items = (int*)v->mpField0;
  else {
    uint16_t f12 = *(volatile uint16_t*)&v->mField12;
    items = (f12 != 0) ? (int*)v : 0;
  }

  Inventory* inv = ((cSPSimulatorSpaceGame*)SP::SpaceGameGet())->GetPlayerInventory();
  if (inv == 0 || count <= 0)
    return;

  do {
    Item item;
    item.mField0 = items[0];
    item.mField4 = 0;
    item.mField8 = 0;
    inv->FUN_00ff45e0(&item);
    items += 3;
  } while (--count != 0);
}

// ===========================================================================
// 0x00c5f360 -- initialize and hand out rewards if still active
// ===========================================================================
// @ 0x00c5f360
void Mission::FUN_00c5f360()
{
  SetSerializer();
  FUN_00c5f2c0();
  if (mField1f0 != 4) {
    ((FnVoid)mpVtable->p[0x6c / 4])(this);
    if (mField1f0 != 4) {
      mField1f0 = 4;
      TriggerUIUpdate();
    }
    FUN_00c472e0();
    FUN_00c471c0();
  }
}

// ===========================================================================
// 0x00c5f3d0 -- target planet's field 0x13c
// ===========================================================================
// @ 0x00c5f3d0
int Mission::FUN_00c5f3d0()
{
  if (GetTargetPlanet() != 0) {
    Planet* p = (Planet*)GetTargetPlanet();
    return p->mField13c;
  }
  return 0;
}

// ===========================================================================
// 0x00c5f4e0 -- compare the current and target planet's field 0x13c
// ===========================================================================
// @ 0x00c5f4e0
int Mission::FUN_00c5f4e0(int id)
{
  int v0 = (mpField1f4 != 0) ? ((Planet*)mpField1f4)->mField13c : 0;

  if (v0 != id) {
    int v1 = (GetTargetPlanet() != 0)
               ? ((Planet*)GetTargetPlanet())->mField13c
               : 0;
    if (v1 != id)
      return 0;
  }
  return 1;
}

// ===========================================================================
// 0x00c5e260 -- per-tick mission step: mission id / state dispatch plus the
//               UI notification that goes with the state it settles on.
// ===========================================================================
struct Obj84 {
  uint8_t mPad000[0x84];
  int     mField84;      // +0x84
};

struct Vec3 {
  float x, y, z;
  Vec3(const Vec3& o) : x(o.x), y(o.y), z(o.z) {}
};

// Engine objects reached through the mission.  Only the calls this routine
// makes are declared; the bodies live elsewhere in the executable.
struct SysA {
  Obj84* FUN_00ba9370();    // 0x00ba9370
};

struct SysB {
  int FUN_00885c90();       // 0x00885c90
};

struct PlanetNode : Planet {
  int FUN_00c70e00();       // 0x00c70e00
  int FUN_00c71e30();       // 0x00c71e30
};

struct HomeObj {
  HomeObj* FUN_00b8de30();  // 0x00b8de30
  int      FUN_00b1fdb0();  // 0x00b1fdb0
};

struct EaText {
  EaText* FUN_00dd8640();   // 0x00dd8640
  void*   FUN_00dd6df0();   // 0x00dd6df0
  void*   FUN_00dd67d0();   // 0x00dd67d0
};

struct ManagerObj {
  void FUN_010151a0(int home, int value);  // 0x010151a0
};

// Fields the scanned-mission objects add on top of the shared Mission layout.
struct MissionExt : Mission {
  uint8_t  mObj200[0x0c];  // +0x200 .. +0x20b (UI object handed to the vtables)
  uint8_t  mField20c[4];   // +0x20c
  uint8_t  mField210[4];   // +0x210
  uint8_t  mField214[1];   // +0x214
  uint8_t  mField215;      // +0x215
  uint8_t  mPad216[0x06];  // +0x216 .. +0x21b
  uint8_t  mField21c;      // +0x21c
  uint8_t  mPad21d;        // +0x21d
  uint8_t  mField21e;      // +0x21e
  uint8_t  mPad21f;        // +0x21f
  uint32_t mField220;      // +0x220
  Vec3     mPos;           // +0x224 .. +0x22f
  uint32_t mField230;      // +0x230
  uint8_t  mField234;      // +0x234
  uint8_t  mPad235[0x03];  // +0x235 .. +0x237
  int      mField238[4];   // +0x238 (state block)

  int  FUN_00c478b0(int deltaMs);   // 0x00c478b0
  int  FUN_00c5e260(int deltaMs);
};

typedef char (__thiscall* FnC2i)(void*, int, void*);
typedef char (__thiscall* FnC2p)(void*, void*, int);

void* __stdcall FUN_00b3d380();
// 0x00b3d2a0: the shipped call sites disagree about the argument list (the
// first pushes the mission key, the second pushes nothing), so both shapes
// are declared against the one address.
void* __stdcall FUN_00b3d2a0(uint32_t owner);
void* __stdcall FUN_00b3d2a0_cur();
void* __cdecl   FUN_0041ea70();
void* __cdecl   FUN_00401010();
char  __cdecl   FUN_00c5da10(void* self, void* pState238);
void  __cdecl   FUN_00c5c310(void* pState238, void* p1f8, void* p1fc,
                             void* pOut, int a, int b);
void  __cdecl   FUN_00c5d140(void* self, void* pState238, int index, void* pUi200,
                             Vec3 pos, int flag234, int flag21c);
void  __cdecl   FUN_00c5c860(void* pState238, void* pPlanet, int field230,
                             void* p1f8, void* p1fc, void* pTarget180,
                             void* p20c, void* p210, void* p214, int deltaMs);
void  __cdecl   FUN_00410370(void* obj, int id, void* pOut);
void  __cdecl   FUN_0040cf10(void* obj, int id, void* pOut);
int   __cdecl   FUN_01021260();
int   __cdecl   FUN_0102c0c0(void* planet);
bool  __cdecl   FUN_00c8b920(int planetField13c, int field230);
// 0x00b3d3e0 text builder: the shipped call sites push two arguments and, from
// the formatting site, six; both shapes are declared against the one address.
EaText* __stdcall FUN_00b3d3e0(int a, int b);
EaText* __stdcall FUN_00b3d3e0_fmt(int a, int b, int c, int d, int e, int f);
void  __cdecl   FUN_00f47380(void* p);

// @ 0x00c5e260
int __thiscall MissionExt::FUN_00c5e260(int deltaMs)
{
  if (FUN_00c478b0(deltaMs) != 3)
    return 3;

  // Only missions that are owned by the local player get a mission key.
  int flags = *(int*)((char*)FUN_00b3d380() + 0x48);
  if ((flags & 1) != 0)
    deltaMs = 0;

  float delay = *(float*)0x1473c70;
  if (mpField180 != 0) {
    SomeVec* pInfo;
    if (((FnC2i)(*(Vtbl**)mpField180)->p[0x24 / 4])(mpField180, 0x3a38abe, &pInfo)
        && pInfo->mField12 == 0xd)
      delay = *(float*)FUN_0041ea70();
  }

  bool isOwner = false;
  if (FUN_01021260() == (int)GetTargetPlanet() && FUN_01021080() == 0)
    isOwner = true;

  unsigned int now = (unsigned int)(delay * *(float*)0x13ec5b4);

  if (mField215 != 0) {
    if (!(*(uint32_t*)((char*)this + 0x7c) > now || isOwner))
      return 3;

    void* pSystem = FUN_00401010();
    if (!((FnC2p)(*(Vtbl**)pSystem)->p[0x1c / 4])(pSystem, (void*)mObj200, 0))
      return 3;

    if (mField215 != 0) {
      Obj84* pOwner = ((SysA*)FUN_00b3d2a0(mField230))->FUN_00ba9370();
      int index = (pOwner != 0) ? pOwner->mField84 : -1;

      int argA = 1;
      int argB = 1;
      if (mpField180 != 0) {
        FUN_00410370(mpField180, 0x339bbc1, &argA);
        FUN_00410370(mpField180, 0x339bbc5, &argB);
      }

      int extra = 0;
      FUN_00c5c310(&mField238, &mField1f8, &mField1fc, &extra, argA, argB);

      if (FUN_01021080() == 0 && FUN_01021260() == (int)GetTargetPlanet())
        FUN_00c5d140(this, &mField238, index, (void*)mObj200, mPos,
                     (int)mField234, (int)mField21c);
      mField215 = 0;
    }
  }

  bool stateChanged =
      mField230 == (uint32_t)((SysB*)FUN_00b3d2a0_cur())->FUN_00885c90();

  if (isOwner && mField220 == 0)
    mField220 = *(uint32_t*)((char*)this + 0x7c);

  if (!(mField21c != 0 && mField220 != 0 &&
        *(uint32_t*)((char*)this + 0x7c) > mField220 &&
        *(uint32_t*)((char*)this + 0x7c) - mField220 > *(uint32_t*)0x1576768)) {
    if (!FUN_00c5da10(this, &mField238)) {
      if (mField078 == 0xd646c543 || mField078 == 0x77184bae ||
          mField078 == 0xa4b0a2df || mField078 == 0xc4956aa7) {
        if (!isOwner)
          return 6;
      }
    } else {
      if (((PlanetNode*)GetTargetPlanet())->FUN_00c70e00() != 5)
        return 6;

      HomeObj* pHome = (HomeObj*)((Planet*)GetTargetPlanet())->mField13c;
      if (FUN_00c8b920((int)pHome, (int)mField230))
        return 6;
      if (pHome->FUN_00b8de30()->FUN_00b1fdb0() !=
          *(int*)((char*)this + 0x94))
        return 6;

      float window = *(float*)0x1473c70;
      if (mpField180 != 0)
        FUN_0040cf10(mpField180, 0x2b9e524, &window);

      if (*(uint32_t*)((char*)this + 0x7c) <
          now - (unsigned int)(window * *(float*)0x1440738)) {
        FUN_00c5c860(&mField238, GetTargetPlanet(), (int)mField230, &mField1f8,
                     &mField1fc, mpField180, &mField20c, &mField210, &mField214,
                     deltaMs);
        return 3;
      }

      FUN_0102c0c0(GetTargetPlanet());
      if (stateChanged) {
        int id = ((Planet*)GetTargetPlanet())->mField13c;
        ManagerObj* pManager = *(ManagerObj**)0x16dc798;
        pManager->FUN_010151a0(
            id, ((PlanetNode*)GetTargetPlanet())->FUN_00c71e30());
      }
      return 6;
    }
  }

  if (mField21e != 0 || isOwner) {
    const char* names[3];
    names[0] = (const char*)0x1667bac;
    names[1] = names[0];
    names[2] = (const char*)0x1667bae;
    GetText(0x25594ea8, (int)names);

    EaText* text = FUN_00b3d3e0_fmt(0xcc0a183b, 0x131a9f54, 0, 0, 1, 0);
    text = text->FUN_00dd8640();
    FUN_00b3d3e0((int)text, (int)names[0])->FUN_00dd6df0();
    FUN_00b3d3e0((int)text, 1)->FUN_00dd67d0();

    int span = ((int)names[2] - (int)names[0]) & ~1;
    if (span > 2 && names[0] != 0)
      FUN_00f47380((void*)names[0]);
  }

  if (stateChanged) {
    int id = ((Planet*)GetTargetPlanet())->mField13c;
    ManagerObj* pManager = *(ManagerObj**)0x16dc798;
    pManager->FUN_010151a0(id, ((PlanetNode*)GetTargetPlanet())->FUN_00c71e30());
  }
  return 5;
}
