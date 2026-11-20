// @ 0x01047440
// Slice s01047440 -- SP::cStarMap::FilterHelperRelationships, the pass that
// walks the star manager's empire map, keeps the empires the player is at war
// with or allied to, and builds one relationship visual per star of theirs.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include <new>
#include "types.h"

namespace SP {

// ---- collaborators of FilterHelperRelationships (0x01047440) ----

class VEffect {
 public:
    virtual void AddRef();                                   // +0x00
    virtual void Release();                                  // +0x04
    virtual void SetVisible(bool visible);                   // +0x08
    virtual void v0c();                                      // +0x0c
    virtual void v10();                                      // +0x10
    virtual void v14();                                      // +0x14
    virtual void ApplyParam18(void* param);                  // +0x18
    virtual void v1c();                                      // +0x1c
    virtual void ApplyParam20(void* param);                  // +0x20
    virtual void v24();                                      // +0x24
    virtual void v28();                                      // +0x28
    virtual void v2c();                                      // +0x2c
    virtual void v30();                                      // +0x30
    virtual void v34();                                      // +0x34
    virtual void v38();                                      // +0x38
    virtual void v3c();                                      // +0x3c
    virtual void v40();                                      // +0x40
    virtual void SetValue44(int kind, void* value, int flag); // +0x44
};

// The manager that owns the relationship-screen effect (a field of the
// object returned by 0x00b3d470).
class VisualManager {
 public:
    virtual void m00();                                      // +0x00
    virtual void m04();                                      // +0x04
    virtual bool Create(uint32_t id, int flags, VEffect** out); // +0x08
};

class EffectHost {
 public:
    uint8_t mPad00[0xac];                                    // +0x00 .. +0xab
    VisualManager* mpVisualManager;                          // +0xac
};

class cStarRecord {
 public:
    uint8_t mPad00[0x70];                                    // +0x00 .. +0x6f
    uint32_t mKey;                                           // +0x70
};

struct StarVec {
    cStarRecord** mpBegin;
    cStarRecord** mpEnd;
};

class CEmpire {
 public:
    uint8_t mPad00[0x70];                                    // +0x00 .. +0x6f
    uint32_t mKey;                                           // +0x70

    StarVec* GetStars();                                     // 0x00c308d0
    bool HasStar(cStarRecord* rec);                          // 0x00c31470
};

struct EmpNode {
    uint8_t mPad00[0x14];                                    // +0x00 .. +0x13
    CEmpire* mpEmpire;                                       // +0x14
};

// eastl-style map of empires: the anchor/sentinel sits at +0x04, the leftmost
// node pointer at +0x08.
struct EmpireMap {
    uint32_t mField00;                                       // +0x00
    uint32_t mSentinel;                                      // +0x04 (anchor)
    EmpNode* mpLeftmost;                                     // +0x08
};

class cRelationship {
 public:
    uint8_t IsAtWar();                                       // 0x00d01ff0
    uint8_t IsAlly();                                        // 0x00d01f50
};

class cStarManager {
 public:
    EmpireMap* GetEmpireMap();                               // 0x00ba6430
};

class cPropView {
 public:
    cPropView();                                             // 0x01046e40
    VEffect* FindVisual(uint32_t key, uint32_t id);          // 0x01045120
};

struct Matrix33 {
    Matrix33(const Matrix33& src);                           // 0x0041cb40
    float m[9];
};

// The transform/colour block handed to the effect.
struct VisualParam {
    uint16_t mKind;                                          // +0x00
    uint16_t mIndex;                                         // +0x02
    float    mR;                                             // +0x04
    float    mG;                                             // +0x08
    float    mB;                                             // +0x0c
    float    mA;                                             // +0x10
    Matrix33 mMatrix;                                        // +0x14
};

struct RBTree {
    uint32_t mAlloc;
    void*    mpAnchorLeft;
    void*    mpAnchorRight;
    void*    mpRoot;
    uint32_t mField10;
    uint32_t mnSize;
    uint32_t mPad18;
};

class cStarMap {
 public:
    uint8_t mPad00[0x54];                                    // +0x00 .. +0x53
    VEffect* mpRelationshipScreenEffect;                     // +0x54
    RBTree  mObjectVisuals;                                  // +0x58
    uint32_t mVisualCount;                                   // +0x74

    VEffect* CreateVisual(cStarRecord* rec, int kind, int key, int group,
                          bool flagA, bool flagB);           // 0x01045740
    void FilterHelperRelationships(uint32_t which);          // 0x01047440
};

class cPropertyRecord {
 public:
    uint8_t mPad00[0x12];                                    // +0x00 .. +0x11
    uint16_t mKind;                                          // +0x12

    float* GetPropertyAsFloat();                             // 0x0041ea70
};

class cPropertyBag {
 public:
    virtual void p00();                                      // +0x00
    virtual void p04();                                      // +0x04
    virtual void p08();                                      // +0x08
    virtual void p0c();                                      // +0x0c
    virtual void p10();                                      // +0x10
    virtual void p14();                                      // +0x14
    virtual void p18();                                      // +0x18
    virtual void p1c();                                      // +0x1c
    virtual void p20();                                      // +0x20
    virtual bool Find(uint32_t key, cPropertyRecord** out);  // +0x24
};

struct PropertyRoot {
    uint8_t mPad00[0x10];                                    // +0x00 .. +0x0f
    cPropertyBag* mpBag;                                     // +0x10
};

// ---- free functions and globals used by the filter ----

CEmpire* __cdecl GetPlayerEmpire();                          // 0x01021300
cStarManager* __cdecl StarManager();                         // 0x00b3d2a0
cRelationship* __stdcall RelationshipManager(CEmpire* a, CEmpire* b); // 0x00b3d2c0
EffectHost* __cdecl GetEffectHost();                         // 0x00b3d470
void* __cdecl operator_new6(size_t n, const char* file, int a, int b,
                            int c, int d);                   // 0x00f473a0
EmpNode* __cdecl RBTreeIncrement(EmpNode* node);             // 0x00921580

extern cPropView* g_PropView;                                // 0x016e00d4
extern PropertyRoot* g_pPropertyRoot;                        // 0x016dc798
extern float g_1485720;                                      // 0x01485720 (1.0f)
extern float g_1486374;                                      // 0x01486374 (war tint)
extern float g_16e06b4;                                      // 0x016e06b4 (white r)
extern float g_16e06b8;                                      // 0x016e06b8 (white g)
extern float g_16e06bc;                                      // 0x016e06bc (white b)
extern Matrix33 g_mtx16e0690;                                // 0x016e0690 (identity)
extern const char kAllocatorFile[];                          // 0x013f09b4

// The property that carries the relationship alpha multiplier.
const uint32_t kRelationshipProperty = 0x055e7dad;
const uint16_t kFloatPropertyKind = 0x000d;

// Effect groups of the two relationship visuals (war and ally variants).
const uint32_t kWarVisualA = 0x83179ac8;
const uint32_t kWarVisualB = 0x95c018d7;
const uint32_t kAllyVisualA = 0x59084e74;
const uint32_t kAllyVisualB = 0xbbba442f;

// The relationship-screen effect looked up/created at the end of a pass.
const uint32_t kRelationshipScreenEffect = 0xc06a7f22;

// Bind one relationship visual to its instanced effect in the property view.
// The original repeats this block once per visual group.
static __forceinline bool ConfigureRelationshipVisual(
    VEffect* visual, cStarRecord* rec, float base, float prop)
{
    if (g_PropView == 0) {
        void* mem = operator_new6(0x78, kAllocatorFile, 0, 0, 0, 0);
        g_PropView = (mem != 0) ? (cPropView*)new (mem) cPropView : 0;
    }
    VEffect* inst = g_PropView->FindVisual(rec->mKey, (uint32_t)visual);
    if (inst == 0)
        return false;

    float value = base * prop;
    inst->SetValue44(1, &value, 1);

    VisualParam param = { 0, 0,
                          g_16e06b4, g_16e06b8, g_16e06bc, g_1485720,
                          g_mtx16e0690 };

    inst->ApplyParam20(&param);
    param.mIndex++;
    param.mA = prop;
    inst->ApplyParam18(&param);
    return true;
}

}  // namespace SP

// @ 0x01047440
void SP::cStarMap::FilterHelperRelationships(uint32_t which)
{
    SP::CEmpire* player = SP::GetPlayerEmpire();
    SP::EmpireMap* empires = SP::StarManager()->GetEmpireMap();

    bool changed = false;

    // Alpha multiplier read from the open screen's property bag; left
    // untouched when the property is absent, exactly like the original.
    float prop;
    SP::cPropertyBag* bag = SP::g_pPropertyRoot->mpBag;
    if (bag != 0) {
        SP::cPropertyRecord* propRec;
        if (bag->Find(SP::kRelationshipProperty, &propRec) &&
            propRec->mKind == SP::kFloatPropertyKind) {
            prop = *propRec->GetPropertyAsFloat();
        }
    }

    SP::EmpNode* node = empires->mpLeftmost;
    SP::EmpNode* end = (SP::EmpNode*)&empires->mSentinel;
    if (node != end) {
        for (;;) {
            SP::CEmpire* empire = node->mpEmpire;
            SP::StarVec* stars = empire->GetStars();
            int count = (int)(stars->mpEnd - stars->mpBegin);

            // Two independent short-circuit expressions: the original tests
            // "empire != player" once for each relationship query.
            bool atWar = empire != player &&
                         SP::RelationshipManager(empire, player)->IsAtWar();
            bool ally = empire != player &&
                        SP::RelationshipManager(empire, player)->IsAlly();

            if (atWar || ally) {
                int i = 0;
                if (count > 0) {
                    do {
                        SP::cStarRecord* rec = stars->mpBegin[i];
                        if (empire != player && empire->HasStar(rec)) {
                            float base;
                            uint32_t groupA;
                            uint32_t groupB;
                            if (atWar) {
                                base = SP::g_1486374;
                                groupA = SP::kWarVisualA;
                                groupB = SP::kWarVisualB;
                            } else {
                                base = SP::g_1485720;
                                groupA = SP::kAllyVisualA;
                                groupB = SP::kAllyVisualB;
                            }

                            SP::VEffect* visualA =
                                CreateVisual(rec, 0, (int)which, (int)groupA, false, true);
                            SP::VEffect* visualB =
                                CreateVisual(rec, 0, (int)which, (int)groupB, false, true);

                            // Reuse (or lazily create) the property view and
                            // bind each visual to its instanced effect.
                            if (visualA != 0) {
                                if (SP::ConfigureRelationshipVisual(
                                        visualA, rec, base, prop))
                                    changed = true;
                            }
                            if (visualB != 0) {
                                SP::ConfigureRelationshipVisual(
                                    visualB, rec, base, prop);
                            }
                        }
                        ++i;
                    } while (i < count);
                }
            }

            node = SP::RBTreeIncrement(node);
            if (node == end)
                break;
        }
    }

    if (changed) {
        SP::VisualManager* visuals = SP::GetEffectHost()->mpVisualManager;
        SP::VEffect* old = this->mpRelationshipScreenEffect;
        if (old != 0) {
            this->mpRelationshipScreenEffect = 0;
            old->Release();
        }

        visuals->Create(SP::kRelationshipScreenEffect, 0,
                        &this->mpRelationshipScreenEffect);
        this->mpRelationshipScreenEffect->SetVisible(false);
    }
}
