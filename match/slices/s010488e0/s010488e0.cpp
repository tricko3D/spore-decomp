// Slice s010488e0 -- SP::cStarMap helper rebuild (partial).
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include <new>
#include "types.h"

namespace SP {

// ---- collaborators of FilterHelperHome (0x010488e0) ----
struct cStarRecordRef {
    void* GetCommContext();                                  // 0x00ce6950
};

struct VEffect {
    virtual void AddRef();                                   // +0x00
    virtual void Release();                                  // +0x04
    virtual void SetVisible(bool visible);                   // +0x08
    virtual void Destroy(bool now);                          // +0x0c
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

struct RBTree {
    uint32_t mAlloc;                                         // +0x00
    void*    mpAnchorLeft;                                   // +0x04
    void*    mpAnchorRight;                                  // +0x08
    void*    mpRoot;                                         // +0x0c
    uint32_t mField10;                                       // +0x10
    uint32_t mnSize;                                         // +0x14
    uint32_t mPad18;                                         // +0x18

    void Insert(bool* inserted, const void* value, bool left);   // 0x01045490
};

struct VisualCtx {
    VEffect* mpEffect;                                       // +0x00
    uint32_t mField04;                                       // +0x04
    uint32_t mField08;                                       // +0x08
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

class cPlayer : public cEmpire {
};

class cPropView {
 public:
    cPropView();                                             // 0x01046e40
    void* FindVisual(void* key, uint32_t id);                 // 0x01045120
};

struct cStarMap {
    uint8_t mPad00;                                          // +0x00
    uint8_t mRequestZoom;                                    // +0x01
    uint8_t mPad02[2];                                       // +0x02 .. +0x03
    int32_t mFilterStates[8];                                // +0x04 .. +0x23
    uint8_t mPad24[0x34];                                    // +0x24 .. +0x57
    RBTree  mObjectVisuals;                                  // +0x58
    uint32_t mVisualCount;                                   // +0x74

    void BuildVisual(VisualCtx* ctx, void* object);          // 0x01045200
    void FilterHelperRebuild(uint32_t index);                // 0x01048c10
    void FilterHelper();                                     // 0x01047000
    void FilterHelperRebuildAll();
    void FilterHelperAdd(uint32_t which);
    void FilterHelperEmpires(uint32_t which);                // 0x01047850
    void FilterHelperRelationships(uint32_t which);          // 0x01047440
    void FilterHelperVisited(uint32_t which);                // 0x01045f90
    void FilterHelperWormholes(uint32_t which);              // 0x010463b0
    void FilterHelperTravelTrail(uint32_t which);            // 0x01045c80
    void FilterHelperHome(uint32_t which);                   // 0x010488e0
    void FilterHelperSeti(uint32_t which);                   // 0x01046160
    void Helper468f0(uint32_t which);                        // 0x010468f0
    void Helper47300(uint32_t which);                        // 0x01047300
    void FilterHelperLayer(int32_t index, uint32_t value);   // 0x01044a10
    void FilterHelperSpecial(uint32_t value);                // 0x01044df0
    void FilterHelperRemove(int32_t index);                  // 0x01045c30
    void FilterHelper(int32_t index, int32_t mode);
};

cPlayer* __cdecl GetPlayerEmpire();                          // 0x01021300
cStarRecordRef* __fastcall cEmpireGetHomePlanet(cPlayer* self);   // 0x00c31730
EffectsMgr* __cdecl EffectsManagerPtr();                     // 0x0067ddd0
void* __cdecl operator_new6(size_t n, const char* tag,
                            int a, int b, int c, int d);     // 0x00f473a0
extern void* g_PropView;                                     // 0x016e00d4

} // namespace SP

extern "C" void GetUniverseContext();                        // 0x01021080 (thiscall)

// @ 0x01048aa0
void SP::cStarMap::FilterHelperAdd(uint32_t which)
{
    mRequestZoom = 1;
    switch (which) {
    case 0:
        FilterHelperEmpires(0);
        return;
    case 1:
        FilterHelperEmpires(1);
        return;
    case 2:
        FilterHelperRelationships(2);
        return;
    case 3:
        FilterHelperVisited(3);
        FilterHelperWormholes(3);
        return;
    case 4:
        FilterHelperTravelTrail(4);
        return;
    case 5:
        Helper468f0(5);
        return;
    case 6:
        FilterHelperHome(6);
        Helper47300(6);
        FilterHelperSeti(6);
        return;
    }
}

// @ 0x01048b40
void SP::cStarMap::FilterHelper(int32_t index, int32_t mode)
{
    const int32_t kFilterOff = 0;
    const int32_t kFilterOn = 1;
    const int32_t kFilterPreview = 2;
    const int32_t kFilterOnPreview = 3;
    const int32_t kFilterOffPreview = 4;

    bool changed = false;
    uint32_t value = 0;

    switch (mode) {
    case kFilterOff:
        if (mFilterStates[index] == kFilterOnPreview) {
            changed = true;
            value = 1;
            mode = kFilterOn;
        }
        break;
    case kFilterOn:
        changed = true;
        value = 1;
        if (mFilterStates[index] == kFilterOnPreview)
            mode = kFilterOnPreview;
        break;
    case kFilterPreview:
        changed = true;
        value = 1;
        if (mFilterStates[index] == kFilterOff) {
            mode = kFilterOffPreview;
        } else if (mFilterStates[index] == kFilterOn) {
            mode = kFilterOnPreview;
        } else {
            return;
        }
        break;
    }

    int32_t oldState = mFilterStates[index];
    mFilterStates[index] = mode;

    if (index >= 0) {
        if (index <= 1)
            FilterHelperLayer(index, value);
        else if (index == 5)
            FilterHelperSpecial(value);
    }

    if (changed) {
        if (oldState == kFilterOff)
            FilterHelperAdd((uint32_t)index);
    } else {
        FilterHelperRemove(index);
    }
}

// @ 0x01048ce0
void SP::cStarMap::FilterHelperRebuildAll()
{
    if (((void* (__thiscall*)(SP::cStarMap*))GetUniverseContext)(this)) {
        for (uint32_t i = 0; i < 7; ++i)
            FilterHelperRebuild(i);
        FilterHelper();
    }
}

// @ 0x010488e0
void SP::cStarMap::FilterHelperHome(uint32_t which)
{
    SP::cPlayer* player = SP::GetPlayerEmpire();
    SP::cStarRecordRef* home =
        (SP::cStarRecordRef*)SP::cEmpireGetHomePlanet(player);

    SP::VisualCtx ctx;
    ctx.mField04 = mVisualCount++;
    ctx.mField08 = which;
    ctx.mpEffect = 0;
    ctx.mFlags = 0;

    void* commCtx;
    if (home != 0) {
        ctx.mKind = 0;
        commCtx = home->GetCommContext();
    } else {
        ctx.mKind = 1;
        commCtx = *(void**)0x70;
    }
    ctx.mFlags |= 3;

    SP::EffectsMgr* mgr = SP::EffectsManagerPtr();
    if (ctx.mpEffect != 0) {
        SP::VEffect* stale = ctx.mpEffect;
        ctx.mpEffect = 0;
        stale->Release();
    }
    if (mgr->Create(0x7c2917e7u, 0, &ctx.mpEffect)) {
        ctx.mpEffect->SetVisible(false);
        BuildVisual(&ctx, commCtx);

        SP::StarMapVisual visual;
        visual.mpRef = commCtx;
        visual.mpEffect = ctx.mpEffect;
        if (visual.mpEffect != 0)
            visual.mpEffect->AddRef();
        visual.mField08 = ctx.mField04;
        visual.mField0c = ctx.mField08;
        visual.mField10 = ctx.mKind;
        visual.mFlags = ctx.mFlags;

        bool inserted = false;
        mObjectVisuals.Insert(&inserted, &visual, inserted);

        uint32_t visualId = ctx.mField04;
        if (visual.mpEffect != 0)
            visual.mpEffect->Release();
        if (ctx.mpEffect != 0)
            ctx.mpEffect->Release();

        if (visualId != 0) {
            SP::cPropView* view = (SP::cPropView*)SP::g_PropView;
            if (view == 0) {
                void* mem = SP::operator_new6(0x78, "Simulator", 0, 0, 0, 0);
                view = (mem != 0) ? (SP::cPropView*)new (mem) SP::cPropView : 0;
                SP::g_PropView = view;
            }
            void* found = view->FindVisual(home->GetCommContext(), visualId);
            if (found != 0) {
                SP::cSPColorRGB color = SP::GetPlayerEmpire()->GetColor();
                ((void (__thiscall*)(void*, int, void*, int))
                    (*(void***)found)[0x40 / 4])(found, 5, &color, 1);
            }
        }
    } else if (ctx.mpEffect != 0) {
        ctx.mpEffect->Release();
    }
}
