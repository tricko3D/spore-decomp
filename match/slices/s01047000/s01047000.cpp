// Slice s01047000 -- SP::cStarMap::FilterHelper, the master pass that walks the
// galaxy's star records and rebuilds the "special" map visual for each record.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include <new>
#include "types.h"

namespace SP {

// ---- collaborators of FilterHelper (0x01047000) ----

struct VEffect {
    virtual void AddRef();                                   // +0x00
    virtual void Release();                                  // +0x04
    virtual void SetVisible(bool visible);                   // +0x08
    virtual void Destroy(bool now);                          // +0x0c
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void SetParam(int kind, void* data, int flag);   // +0x40
};

class EffectsMgr {
 public:
    virtual void e00();
    virtual void e04();
    virtual void e08();
    virtual void e0c();
    virtual void e10();
    virtual void e14();
    virtual void e18();
    virtual void e1c();
    virtual void e20();
    virtual void e24();
    virtual void e28();
    virtual bool Create(uint32_t id, int flags, VEffect** out);   // +0x2c
};

// Query block handed to the player inventory's record enumerator.
struct StarQuery {
    uint32_t mField00;                                       // 0x1fff
    uint32_t mField04;                                       // 0x20
    uint32_t mField08;                                       // 0
    float    mField0c;
    float    mField10;
    float    mField14;
    uint32_t mField18;                                       // 0
};

class cStarRecord {
 public:
    virtual void r00();                                      // +0x00
    virtual void Release();                                  // +0x04
    uint8_t mPad08[0x6c];                                    // +0x04 .. +0x6f
    void* mpInstance;                                        // +0x70
};

// Reference-counted element of the record list.
struct cStarRecordRef {
    cStarRecord* mpRecord;                                   // +0x00

    void Release()
    {
        if (mpRecord != 0)
            mpRecord->Release();
    }
};

void __cdecl operator_free(void* p);                         // 0x00f47380

struct RecordList {
    cStarRecordRef* mpBegin;
    cStarRecordRef* mpEnd;
    cStarRecordRef* mpCap;

    // eastl::vector<cStarRecordRef> teardown
    ~RecordList()
    {
        for (cStarRecordRef* p = mpBegin; p < mpEnd; ++p)
            p->Release();
        if (mpBegin != 0 && ((uint32_t*)mpBegin)[-1] != 0)
            operator_free(mpBegin);
    }
};

class StarEnumerator {
 public:
    virtual void s00();
    virtual void s04();
    virtual void s08();
    virtual void s0c();
    virtual void s10();
    virtual void s14();
    virtual void s18();
    virtual void s1c();
    virtual void s20();
    virtual void s24();
    virtual void s28();
    // Fills `out` with every record matching `query`; returns the record the
    // player has selected.
    virtual cStarRecord* GetRecords(StarQuery* query, RecordList* out);   // +0x2c
};

class cPlayerInventory {
 public:
    uint8_t mPad00[0x34];
    StarEnumerator mRecords;                                 // +0x34
};

class cSPSimulatorSpaceGame {
 public:
    float GetMaxZ();                                         // 0x00ffbfd0
    cPlayerInventory* GetPlayerInventory();                  // 0x00a1ad60
};

class cPlayerEmpire {
 public:
    uint8_t mPad00[0x84];
    void*   mpHomeRecord;                                    // +0x84
};

struct RBTree {
    uint32_t mAlloc;                                         // +0x00
    void*    mpAnchorLeft;                                   // +0x04
    void*    mpAnchorRight;                                  // +0x08
    void*    mpRoot;                                         // +0x0c
    uint32_t mField10;                                       // +0x10
    uint32_t mnSize;                                         // +0x14
    uint32_t mPad18;                                         // +0x18

    void DoInsertValueImpl(void* result, const void* value, bool left); // 0x01045490
};

// Return slot of an rbtree insertion (the resulting iterator).
struct RBInsertResult {
    void* mpNode;
    void* mpField;
};

struct VisualCtx {
    VEffect* mpEffect;                                       // +0x00
    uint32_t mField04;                                       // visual id
    uint32_t mField08;                                       // visual group
    uint32_t mKind;                                          // +0x0c
    uint8_t  mFlags;                                         // +0x10
};

struct StarMapVisual {
    void*    mpRef;                                          // +0x00
    VEffect* mpEffect;                                       // +0x04
    uint32_t mField08;                                       // +0x08
    uint32_t mField0c;                                       // +0x0c
    uint32_t mField10;                                       // +0x10
    uint8_t  mFlags;                                         // +0x14
};

struct cSPColorRGB {
    uint32_t mR, mG, mB;
};

class cEmpire {
 public:
    cSPColorRGB GetColor();                                  // 0x00c32cd0
};

class cStarManager {
 public:
    void* GetRecords();                                      // 0x00bb1080
    cEmpire* GetPlayerEmpire();                              // 0x00ba9370
};

class cPropView {
 public:
    cPropView();                                             // 0x01046e40
    void* FindVisual(void* key, uint32_t id);                // 0x01045120
};

struct cStarMap {
    uint8_t mPad00[0x58];                                    // +0x00 .. +0x57
    RBTree  mObjectVisuals;                                  // +0x58
    uint32_t mVisualCount;                                   // +0x74

    bool ChecksVisible(cStarRecord* rec);                    // 0x01044bf0
    void* FindEntryByLongId(uint32_t key, uint32_t lo, uint32_t hi); // 0x01045190
    void BuildVisual(VisualCtx* ctx, void* object);          // 0x01045200
    void FilterHelper();                                     // 0x01047000
};

// ---- free helpers used by FilterHelper ----
cSPSimulatorSpaceGame* __cdecl GetUFOSimulator();            // 0x00ffbe50
bool   __cdecl SelectionOk(cStarRecord* rec);                // 0x00c8bb00
float  __cdecl GetFilterRatio(cStarRecord* rec, void** param); // 0x00c8c5d0
void*  __stdcall StarManager(cStarRecord* rec);              // 0x00b3d2a0
cPlayerEmpire* __cdecl GetPlayerEmpire();                    // 0x01021300
EffectsMgr* __cdecl EffectsManagerPtr();                     // 0x0067ddd0
void*  __cdecl operator_new6(size_t n, const char* file, int a, int b,
                             int c, int d);                  // 0x00f473a0
extern const char kAllocatorFile[];                          // 0x013f09b4
extern void* g_PropView;                                     // 0x016e00d4
extern float g_13eb1bc;                                      // 0x013eb1bc

// The effects manager id of the special filter visual.
const uint32_t kFilterEffectId = 0xff8bbb2b;

}  // namespace SP

// @ 0x01047000
void SP::cStarMap::FilterHelper()
{
    SP::cSPSimulatorSpaceGame* sim = SP::GetUFOSimulator();

    SP::RecordList records;
    records.mpBegin = 0;
    records.mpEnd = 0;
    records.mpCap = 0;

    SP::StarQuery query;
    query.mField00 = 0x1fff;
    query.mField04 = 0x20;
    query.mField08 = 0;
    query.mField0c = (float)SP::g_13eb1bc;
    query.mField10 = (float)SP::g_13eb1bc;
    query.mField14 = (float)SP::g_13eb1bc;
    query.mField18 = 0;
    query.mField10 = sim->GetMaxZ();

    SP::cPlayerInventory* inventory = sim->GetPlayerInventory();
    SP::cStarRecord* selected = inventory->mRecords.GetRecords(&query, &records);
    SP::cStarManager* manager = (SP::cStarManager*)SP::StarManager(selected);
    manager->GetRecords();

    void* homeRecord = SP::GetPlayerEmpire()->mpHomeRecord;

    SP::cStarRecordRef* end = records.mpEnd;
    for (SP::cStarRecordRef* it = records.mpBegin; it != end; ++it) {
        SP::cStarRecord* rec = it->mpRecord;

        if (!SP::SelectionOk(rec))
            continue;

        float ratio = SP::GetFilterRatio(rec, &homeRecord);
        if (!(ratio > 0.0f))
            continue;

        if (FindEntryByLongId((uint32_t)rec->mpInstance, SP::kFilterEffectId,
                              0xffffffffu) != 0)
            continue;

        SP::VisualCtx ctx;
        ctx.mField04 = mVisualCount++;
        ctx.mpEffect = 0;
        ctx.mField08 = 0xafd54838;
        ctx.mKind = 1;
        ctx.mFlags = 0;

        uint32_t visualId = 0;

        if (ChecksVisible(rec)) {
            ctx.mFlags |= 2;

            SP::EffectsMgr* mgr = SP::EffectsManagerPtr();
            if (ctx.mpEffect != 0) {
                SP::VEffect* stale = ctx.mpEffect;
                ctx.mpEffect = 0;
                stale->Release();
            }
            if (mgr->Create(SP::kFilterEffectId, 0, &ctx.mpEffect)) {
                ctx.mpEffect->SetVisible(false);
                BuildVisual(&ctx, rec->mpInstance);

                SP::StarMapVisual visual;
                visual.mpRef = rec->mpInstance;
                visual.mpEffect = ctx.mpEffect;
                if (visual.mpEffect != 0)
                    visual.mpEffect->AddRef();
                visual.mField08 = ctx.mField04;
                visual.mField0c = ctx.mField08;
                visual.mField10 = ctx.mKind;
                visual.mFlags = ctx.mFlags;

                bool inserted = false;
                SP::RBInsertResult result;
                mObjectVisuals.DoInsertValueImpl(&result, &visual, inserted);

                if (visual.mpEffect != 0)
                    visual.mpEffect->Release();
                if (ctx.mpEffect != 0)
                    ctx.mpEffect->Release();

                visualId = ctx.mField04;
            } else if (ctx.mpEffect != 0) {
                ctx.mpEffect->Release();
            }
        } else if (ctx.mpEffect != 0) {
            ctx.mpEffect->Release();
        }

        if (SP::g_PropView == 0) {
            void* mem = SP::operator_new6(0x78, SP::kAllocatorFile, 0, 0, 0, 0);
            SP::g_PropView = (mem != 0) ? (void*)new (mem) SP::cPropView : 0;
        }

        void* found = ((SP::cPropView*)SP::g_PropView)
                          ->FindVisual(rec->mpInstance, visualId);
        if (found == 0)
            continue;

        SP::cEmpire* empire =
            ((SP::cStarManager*)SP::StarManager((SP::cStarRecord*)homeRecord))
                ->GetPlayerEmpire();
        if (empire == 0)
            continue;

        SP::cSPColorRGB color = empire->GetColor();
        ((void (__thiscall*)(void*, int, void*, int))
             (*(void***)found)[0x40 / 4])(found, 5, &color, 1);
    }
}
