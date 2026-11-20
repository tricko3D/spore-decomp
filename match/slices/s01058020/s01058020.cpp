// s01058020.cpp - reconstructed slice
// Compiler: MSVC 2008 SP1 (x86)
// Flags: /O2 /Ob1 /MD /Gy /EHsc /TP /arch:SSE /GS-
// ---------------------------------------------------------------------------------------------
// 0x01058020  SP::cSPRepairAOEToolStrategy::StartTool  (retail 1088 bytes, `ret 0xc`)
//
// Starts the repair-AoE tool: refreshes the charged target item, rolls a repair amount inside
// the strategy's [min, max] window, asks the item's query receiver for every object inside the
// area radius and hands each repair target (energy component / owner interface) its order.
// ---------------------------------------------------------------------------------------------
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int i32;
typedef float f32;
typedef double f64;

struct cVec3 { f32 mX; f32 mY; f32 mZ; };

// 0x016e1b14: the (0, 0, 0) origin passed with every effect kick-off.
extern const cVec3 gZeroPoint;
// Values baked into the notice record (0x016e20fc / 0x016e2100 / 0x016e2104 / 0x01485720).
extern const f32 kNotifyParamA;
extern const f32 kNotifyParamB;
extern const f32 kNotifyParamC;
extern const f32 kNotifyParamD;
// Localized "target repaired" text id (0x016e20d8).
extern const u32 kRepairedTextKey;

// 0x00c389c0: thiscall on the item (ecx) with two stack arguments.
struct cItemStateHelper { void SetState(i32 nState, i32 bOn); };

// Global random source (0x01601760); NextFloat leaves the value in st(0).
struct cRandomSource { f32 NextFloat(); };
extern cRandomSource gRandomSource;

// ---------------------------------------------------------------------------------------------
// Callees, each with the retail address it stands for.
// ---------------------------------------------------------------------------------------------
bool __stdcall StartTool_568b0(void* pSelf, i32 nTarget, i32 nFlags);   // 0x010568b0
u32  __cdecl   GetAreaId();                                             // 0x01021090
f64  __fastcall GetAmountMin(void* pSelf);                              // 0x00ce6940
f64  __fastcall GetAmountMax(void* pSelf);                              // 0x0104bdc0
f32  __fastcall GetAreaRadius(void* pSelf);                             // 0x0104be30
void* __cdecl  GetApp();                                                // 0x00b3d240
void* __stdcall ResolveOwnerOf(void* pObject);                          // 0x00b3d2a0
void* __stdcall ResolveOwnerOfId(u32 nId, u32 nAreaId);                 // 0x00b3d2c0
void* __fastcall FindOwner(void* pObject);                              // 0x00ba9370
bool __fastcall OwnerIsArea(void* pOwner);                              // 0x00d01fb0
void* __cdecl  GetPlayerOwner();                                        // 0x01021300
void* __cdecl  GetNotifySource();                                       // 0x00a206f0
void __cdecl  EmitRepairNote(u32 nKey, void* pValue);                   // 0x00435ed0
void* __cdecl  GetMessageServer();                                      // 0x0067ddd0
void __cdecl  ReleaseQueryBuffer(void* pBuffer);                        // 0x00f47380

// ---------------------------------------------------------------------------------------------
// Small object model (only the members this function touches).
// ---------------------------------------------------------------------------------------------
// Retail reserves 0x14 bytes for the query result list: three used pointers and
// two further words that this function never touches (retail's notice sits at +0x38,
// right after the list's 0x14 bytes at +0x24).
struct cObjList { void** mpBegin; void** mpEnd; void** mpCap; void* mpAlloc; void* mpCtx; };

// Receives the area query at +0x34 of a repair target.
struct cQueryReceiver
{
    virtual void v00();     // +0x00
    virtual void v01();     // +0x04
    virtual void v02();     // +0x08
    virtual void v03();     // +0x0c
    virtual void v04();     // +0x10
    virtual void v05();     // +0x14
    virtual void v06();     // +0x18
    virtual void v07();     // +0x1c
    virtual void v08();     // +0x20
    virtual void v09();     // +0x24
    virtual void v0a();     // +0x28
    virtual void* Query(f32 fRadius, cObjList* pOut, i32 nA, i32 nB, i32 nC);   // +0x2c
};

// Kicks off the visual repair effect at +0x508 of a repair target.
struct cEffectHost
{
    virtual void v00();     // +0x00
    virtual void v01();     // +0x04
    virtual void v02();     // +0x08
    virtual void v03();     // +0x0c
    virtual void v04();     // +0x10
    virtual void v05();     // +0x14
    virtual void Start(f32 fScale, u32 nAreaId, i32 nKind, const cVec3* pOrigin, i32 nUnused);   // +0x18
};

struct cRepairTarget
{
    u8 mPad000[0x34];           // +0x000
    cQueryReceiver mQuery;      // +0x034
    u8 mPad038[0x508 - 0x38];   // +0x038
    cEffectHost mEffect;        // +0x508
};

// The strategy: +0x114 holds the item the tool is charged against.
struct cRepairAOEToolStrategy
{
    u8 mPad000[0x114];          // +0x000 (vftable and tool strategy state)
    void* mpTargetItem;         // +0x114
};

// Localized text object built in place (0x0041cb40 takes it in ecx).
struct cLocalizedText
{
    u8 mBytes[0x18];
    void Init(const u32* pKey);     // 0x0041cb40
};

struct cRepairNotice
{
    u16 mFlags;                 // +0x00
    u16 mCount;                 // +0x02
    cVec3 mPos;                 // +0x04
    f32 mParam;                 // +0x10
    cLocalizedText mText;       // +0x14
    // Retail's frame ends at +0x70 with the notice at +0x38, so the record is 0x38
    // bytes; the tail is unused by this function.
    u8 mTail[0xc];              // +0x2c
};

// ---------------------------------------------------------------------------------------------
// Virtual-slot helpers (each mirrors one `call [vptr + slot]` in the original).
// ---------------------------------------------------------------------------------------------

// Cast an object to an interface id through slot 0xb8.
static inline void* CastTo(void* pObject, u32 nType)
{
    void* pfn = ((void**)*(void***)pObject)[0xb8 / 4];
    return ((void* (__thiscall*)(void*, u32))pfn)(pObject, nType);
}

// Slot 0x4c: the object's owner handle.
static inline void* GetOwnerHandle(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0x4c / 4];
    return ((void* (__thiscall*)(void*))pfn)(pObject);
}

// Slot 0x10: the object's index inside its owner.
static inline i32 GetObjectIndex(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0x10 / 4];
    return ((i32 (__thiscall*)(void*))pfn)(pObject);
}

// Slot 0x18 of an owner interface: kicks off its repair effect.
static inline void StartRepairEffectOn(void* pObject, f32 fScale, u32 nAreaId, i32 nKind, const cVec3* pOrigin)
{
    void* pfn = ((void**)*(void***)pObject)[0x18 / 4];
    ((void (__thiscall*)(void*, f32, u32, i32, const cVec3*, i32))pfn)(pObject, fScale, nAreaId, nKind, pOrigin, 0);
}

// Slot 0x2c of a notifiable object: its world position.
static inline cVec3* GetObjectPosition(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0x2c / 4];
    return ((cVec3* (__thiscall*)(void*))pfn)(pObject);
}

// Slot 0x2c of the message server: creates the notice object for nType.
static inline void CreateMessage(void* pServer, u32 nType, i32 nUnused, void** ppOut)
{
    void* pfn = ((void**)*(void***)pServer)[0x2c / 4];
    ((void (__thiscall*)(void*, u32, i32, void**))pfn)(pServer, nType, nUnused, ppOut);
}

// Slots 0x04 / 0x08 / 0x18 / 0x20 / 0xc0 of the message objects.
static inline void ReleaseObject(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0x04 / 4];
    ((void (__thiscall*)(void*))pfn)(pObject);
}

static inline void SetObjectDone(void* pObject, i32 nValue)
{
    void* pfn = ((void**)*(void***)pObject)[0x08 / 4];
    ((void (__thiscall*)(void*, i32))pfn)(pObject, nValue);
}

static inline void AcceptNotice(void* pObject, cRepairNotice* pNotice)
{
    void* pfn = ((void**)*(void***)pObject)[0x18 / 4];
    ((void (__thiscall*)(void*, cRepairNotice*))pfn)(pObject, pNotice);
}

static inline void* GetNotifyValue(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0x20 / 4];
    return ((void* (__thiscall*)(void*))pfn)(pObject);
}

static inline void ReleaseListItem(void* pObject)
{
    void* pfn = ((void**)*(void***)pObject)[0xc0 / 4];
    ((void (__thiscall*)(void*))pfn)(pObject);
}

// Notification keys, one per repair-target kind (jump table at 0x0105843c).
__forceinline u32 NoticeKeyForKind(i32 nKind, void* pOwner)
{
    u32 nKey = 0;
    switch (nKind) {
    case 0:  nKey = 0x4f837058; break;
    case 1:  nKey = 0x45534a7b; break;
    case 2:  nKey = 0xbda4a060; break;
    case 3:  nKey = 0x826551e1; break;
    case 4:  nKey = 0x81411ef3; break;
    case 5:  nKey = 0x4f56cf50; break;
    case 6:  nKey = 0x677e3a03; break;
    case 7:  nKey = 0xc45964f3; break;
    case 8:  nKey = 0xcabe7750; break;
    default:
        if (pOwner == GetPlayerOwner())
            nKey = 0xd8349db6;
        break;
    }
    return nKey;
}

// ---------------------------------------------------------------------------------------------
// 0x01058020  SP::cSPRepairAOEToolStrategy::StartTool
// ---------------------------------------------------------------------------------------------
// @ 0x01058020
bool __stdcall StartTool_58020(cRepairAOEToolStrategy* pSelf, i32 nTarget, i32 nFlags)
{
    // Shared prologue; the original ignores its result here.
    StartTool_568b0(pSelf, nTarget, nFlags);

    u32 nAreaId = GetAreaId();

    // Repair amount: a value inside [min, max] driven by the random source, then clamped.
    volatile f32 fAmount;     // retail re-reads the amount from its frame slot at every use
    f64 dMin = GetAmountMin(pSelf);                                     // x87 stack: [0x14]
    f64 dMax = GetAmountMax(pSelf);                                     // x87 stack: [0x1c]
    f32 fRoll = gRandomSource.NextFloat();
    f64 dAmount = dMin + (dMax - dMin) * fRoll;
    if (dAmount >= dMax) dAmount = dMax;
    else if (dAmount < dMin) dAmount = dMin;
    fAmount = (f32)dAmount;

    // Refresh the charged item itself.
    cRepairTarget* pItem;
    if (pSelf->mpTargetItem != 0) {
        pItem = (cRepairTarget*)CastTo(pSelf->mpTargetItem, 0xb033b403);
        if (pItem != 0) {
            pItem->mEffect.Start(fAmount, nAreaId, 0xa, &gZeroPoint, 0);
            ((cItemStateHelper*)pItem)->SetState(2, 1);
        }
    } else {
        pItem = 0;
    }

    // Ask the item's query receiver for the objects inside the area radius.
    cObjList candidates;
    candidates.mpBegin = 0;
    candidates.mpEnd = 0;
    candidates.mpCap = 0;

    void* pApp = GetApp();
    void* pResult = pItem->mQuery.Query(GetAreaRadius(pSelf), &candidates, 0, 0, 0);

    // Hand the query result to the application callback at +0x48.
    ((void (__thiscall*)(void*, void*))*(void**)((char*)pApp + 0x48))(pApp, pResult);

    cRepairNotice notice;
    i32 nCount = (i32)(candidates.mpEnd - candidates.mpBegin);
    for (i32 i = 0; i < nCount; ++i) {
        void* pCandidate = candidates.mpBegin[i];
        if (pCandidate == pSelf->mpTargetItem)
            continue;
        if (pCandidate == 0)
            continue;

        // Energy components carry a repair state and notify their owner.
        cRepairTarget* pEnergy = (cRepairTarget*)CastTo(pCandidate, 0xb033b403);
        if (pEnergy != 0) {
            void* pOwnerObj = FindOwner(ResolveOwnerOf(GetOwnerHandle(pEnergy)));
            if (pOwnerObj != GetPlayerOwner()) {
                void* pOwner = ResolveOwnerOfId((u32)GetOwnerHandle(pEnergy), nAreaId);
                if (!OwnerIsArea(pOwner))
                    continue;
            }
            pEnergy->mEffect.Start(fAmount, nAreaId, 0xa, &gZeroPoint, 0);
            ((cItemStateHelper*)pEnergy)->SetState(2, 1);

            if (pOwnerObj != 0) {
                u32 nKey = NoticeKeyForKind(*(i32*)((char*)pOwnerObj + 0x58), pOwnerObj);
                void* pSource = GetNotifySource();
                EmitRepairNote(nKey, pSource != 0 ? GetNotifyValue(pSource) : 0);
            }
            continue;
        }

        // Everything else goes through the owner interface at 0x13f94d4.
        void* pOwner = CastTo(pCandidate, 0x13f94d4);
        if (pOwner == 0)
            continue;
        i32 nIndex = GetObjectIndex(pOwner);
        if (nIndex != (i32)nAreaId) {
            void* pOther = ResolveOwnerOfId((u32)nIndex, nAreaId);
            if (!OwnerIsArea(pOther))
                continue;
        }
        StartRepairEffectOn(pOwner, fAmount, nAreaId, 0xa, &gZeroPoint);

        // Build the "repaired" notice and hand it to the message object.
        void* pNotice = 0;
        void* pServer = GetMessageServer();
        if (pNotice != 0) {
            void* pOld = pNotice;
            pNotice = 0;
            ReleaseObject(pOld);
        }
        CreateMessage(pServer, 0x58451561, 0, &pNotice);
        if (pNotice != 0) {
            notice.mPos.mX = kNotifyParamA;
            notice.mPos.mY = kNotifyParamB;
            notice.mPos.mZ = kNotifyParamC;
            notice.mCount = 0;
            notice.mParam = kNotifyParamD;
            notice.mFlags = 0;
            notice.mText.Init(&kRepairedTextKey);

            notice.mPos = *GetObjectPosition(pCandidate);
            notice.mFlags |= 4;
            notice.mCount++;

            AcceptNotice(pNotice, &notice);
            SetObjectDone(pNotice, 0);
            ReleaseObject(pNotice);
        }
    }

    // Release the gathered objects, then the query buffer.
    for (void** pp = candidates.mpBegin; pp < candidates.mpEnd; ++pp) {
        if (*pp != 0)
            ReleaseListItem(*pp);
    }
    if (candidates.mpBegin != 0 && *(void**)((char*)candidates.mpBegin - 4) != 0)
        ReleaseQueryBuffer(candidates.mpBegin);

    return true;
}
