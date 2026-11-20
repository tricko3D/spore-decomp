// Slice s01070290: SP::cSPUISpace saved-games UI.
//   01070890  SP::cSPUISpace::ShowSavedGames   (thiscall)  - one row per star-manager civ that is
//             not the player's home planet: pull the "saved game" property from the effects manager,
//             stamp a 0x38-byte city record and append the property to the +0x308 pointer list.
//   01070af0  eastl::vector<SP::cSPUISpace::cFoodwebSlotInfo>::resize(uint)  (thiscall, ret 4)
// Everything that is not a plain field hangs off a vtable slot, so the vcall targets are declared as
// virtual functions of small classes whose layouts put them at the original offsets.
#include "types.h"
#include <stddef.h>

#define CHK(cls, name, off) typedef char chk_##cls##_##name[(offsetof(cls, name) == (off)) ? 1 : -1]

struct cProperty;
struct cSavedGameRec;

// 12-byte resource key (memory order: three dwords).
struct Key {
    uint32_t a, b, c;
};
extern Key gNullKey;                                    // 0x016e224c

struct Vec3 { float x, y, z; };
struct Matrix3 {
    float m[9];
    Matrix3() {}
    // User-defined copy ctor: retail copies the 3x3 through a stack temporary one float at a time
    // before storing it into the record.
    Matrix3(const Matrix3& o) {
        m[0] = o.m[0]; m[1] = o.m[1]; m[2] = o.m[2];
        m[3] = o.m[3]; m[4] = o.m[4]; m[5] = o.m[5];
        m[6] = o.m[6]; m[7] = o.m[7]; m[8] = o.m[8];
    }
};

// Default contents of a saved-game record: the identity-ish city transform, copied out of the
// engine's constant tables (Havok _HK_REAL_ONE for the w component).
extern const Vec3 kSavedGamePos;                        // 0x016e2824
extern const Matrix3 kSavedGameRot;                     // 0x016e2800
extern const float kOne;                                // 0x01485720  (_HK_REAL_ONE)

// ---------------------------------------------------------------------------
// 0x38-byte city record handed to cProperty::v14.
// ---------------------------------------------------------------------------
struct cSavedGameRec {
    uint16_t mFlags;                                    // +0x00
    uint16_t mCount;                                    // +0x02
    Vec3 mPos;                                          // +0x04
    float mW;                                           // +0x10
    Matrix3 mRot;                                       // +0x14
    // The matrix arrives by reference; retail copies it into a local temporary first (the class has
    // a user copy constructor) and then bit-copies that temporary into the record.
    cSavedGameRec(const Vec3& pos, const Matrix3& rot)
        : mFlags(0), mCount(0), mPos(pos), mW(kOne) { mRot = rot; }
};
CHK(cSavedGameRec, mFlags, 0x00);
CHK(cSavedGameRec, mCount, 0x02);
CHK(cSavedGameRec, mPos, 0x04);
CHK(cSavedGameRec, mW, 0x10);
CHK(cSavedGameRec, mRot, 0x14);
typedef char chk_cSavedGameRec_size[(sizeof(cSavedGameRec) == 0x38) ? 1 : -1];

// Refcounted property value. Only the slots this slice calls are named.
struct cProperty {
    virtual void AddRef();                              // +0x00
    virtual void Release();                             // +0x04
    virtual void V08(int);                              // +0x08
    virtual void V0c();                                 // +0x0c
    virtual void V10();                                 // +0x10
    virtual void V14(cSavedGameRec* rec);               // +0x14
};

// ---------------------------------------------------------------------------
// SP::cSPUISpace::cFoodwebSlotInfo - 0x28-byte row of the foodweb slot table. The default row is
// what vector::resize(n) fills new slots with.
// ---------------------------------------------------------------------------
struct cFoodwebSlotInfo {
    cProperty* mpWindow;                                // +0x00  refcounted window (AddRef/Release)
    Key mKey2;                                          // +0x04  secondary key, gNullKey by default
    int mKind;                                          // +0x10
    uint32_t mKey;                                      // +0x14
    int mField18;                                       // +0x18
    uint8_t mFlag1c;                                    // +0x1c
    int mIndex;                                         // +0x20
    int mFlag24;                                        // +0x24
    cFoodwebSlotInfo()
        : mpWindow(0), mKey2(gNullKey), mKind(0), mKey(0x3a8be428u),
          mField18(1), mFlag1c(0), mIndex(1), mFlag24(1) {}
};
CHK(cFoodwebSlotInfo, mpWindow, 0x00);
CHK(cFoodwebSlotInfo, mKey2, 0x04);
CHK(cFoodwebSlotInfo, mKind, 0x10);
CHK(cFoodwebSlotInfo, mKey, 0x14);
CHK(cFoodwebSlotInfo, mField18, 0x18);
CHK(cFoodwebSlotInfo, mFlag1c, 0x1c);
CHK(cFoodwebSlotInfo, mIndex, 0x20);
CHK(cFoodwebSlotInfo, mFlag24, 0x24);
typedef char chk_cFoodwebSlotInfo_size[(sizeof(cFoodwebSlotInfo) == 0x28) ? 1 : -1];

// eastl::vector<cFoodwebSlotInfo, sp_vector_allocator>: begin/end/capacity only (the allocator is
// stateless and never materialised by these functions).
struct cFoodwebSlotVec {
    cFoodwebSlotInfo* mpBegin;                          // +0x00
    cFoodwebSlotInfo* mpEnd;                            // +0x04
    cFoodwebSlotInfo* mpCapacity;                       // +0x08
    unsigned size() const { return (unsigned)(mpEnd - mpBegin); }
    void insert(cFoodwebSlotInfo* pos, unsigned count, const cFoodwebSlotInfo& value);  // 0x0106ebb0
    void erase(cFoodwebSlotInfo* first, cFoodwebSlotInfo* last);                        // 0x0106eb40
    void resize(unsigned count);
};
CHK(cFoodwebSlotVec, mpBegin, 0x00);
CHK(cFoodwebSlotVec, mpEnd, 0x04);
CHK(cFoodwebSlotVec, mpCapacity, 0x08);

// eastl pointer vector sitting at cSPUISpace+0x308 (saved-game property list).
struct cPropertyVec {
    cProperty** mpBegin;                                // +0x00
    cProperty** mpEnd;                                  // +0x04
    cProperty** mpCapacity;                             // +0x08
    void insert(cProperty** pos, cProperty* const& value);   // 0x00a0a360 (grow path)
};
CHK(cPropertyVec, mpBegin, 0x00);
CHK(cPropertyVec, mpEnd, 0x04);
CHK(cPropertyVec, mpCapacity, 0x08);

// ---------------------------------------------------------------------------
// Engine objects reached through the free functions below.
// ---------------------------------------------------------------------------
struct EffectsManager {
    virtual void V00();  virtual void V04();  virtual void V08();  virtual void V0c();
    virtual void V10();  virtual void V14();  virtual void V18();  virtual void V1c();
    virtual void V20();  virtual void V24();  virtual void V28();
    virtual bool GetProperty(uint32_t key, int arg, cProperty** out);   // +0x2c
};
EffectsManager* EffectsManagerPtr();                    // 0x0067ddd0

struct cPointerVec {                                     // eastl::vector<void*, sp_vector_allocator>
    void** mpBegin;                                     // +0x00
    void** mpEnd;                                       // +0x04
    void** mpCapacity;                                  // +0x08
    unsigned size() const { return (unsigned)(mpEnd - mpBegin); }
};
struct StarManager {
    char pad0[0xf0];
    cPointerVec mCivs;                                  // +0xf0 (one entry per empire/civ)
};
CHK(StarManager, mCivs, 0xf0);
typedef char chk_StarManager_mCivEnd[(offsetof(StarManager, mCivs) + 4 == 0xf4) ? 1 : -1];
StarManager* StarManager();                             // 0x00b3d2a0

struct cPlanet {
    void* SortBanners();                                // 0x00b8de30
};
cPlanet* GetPlayerHomePlanet();                         // 0x01021370

struct cCivData {
    Vec3* GetCities();                                  // 0x005c65e0
};

struct cSPUISpace {
    char pad0[0x308];
    cPropertyVec mSavedGameList;                        // +0x308
    void F0106f380();                                   // 0x0106f380
    void ShowSavedGames();
};
CHK(cSPUISpace, mSavedGameList, 0x308);
typedef char chk_cSPUISpace_mListEnd[(offsetof(cSPUISpace, mSavedGameList) + 4 == 0x30c) ? 1 : -1];
typedef char chk_cSPUISpace_mListCap[(offsetof(cSPUISpace, mSavedGameList) + 8 == 0x310) ? 1 : -1];

inline void* operator new(size_t, void* p) { return p; }
inline void operator delete(void*, void*) {}
void* __cdecl operator new(size_t, const char*, int, int, int, int);   // 0x00f473a0

// ---------------------------------------------------------------- vtable helpers
template <int N, class R> inline R VCall0(void* p)
{ return ((R(__thiscall*)(void*))(*(void***)p)[N])(p); }
template <int N, class R, class A> inline R VCall1(void* p, A a)
{ return ((R(__thiscall*)(void*, A))(*(void***)p)[N])(p, a); }
template <int N, class R, class A, class B> inline R VCall2(void* p, A a, B b)
{ return ((R(__thiscall*)(void*, A, B))(*(void***)p)[N])(p, a, b); }

template <int N, class R, class A, class B, class C> inline R VCall3(void* p, A a, B b, C c)
{ return ((R(__thiscall*)(void*, A, B, C))(*(void***)p)[N])(p, a, b, c); }

struct Obj { char pad[4]; };

template <int A, int R> __forceinline void AssignRef(Obj*& dst, Obj* v)
{
    Obj* old = dst;
    if (v != old) {
        if (v) VCall0<A, void>(v);
        dst = v;
        if (old) VCall0<R, void>(old);
    }
}

// ---------------------------------------------------------------- stub types
typedef bool (__thiscall *InitFn)(void*, void*, Key);  // vtable slot 7: Init(buffer, key)
#define INIT_CALL(o, buf, key) (((InitFn)(*(void***)(o))[7])((o), (buf), (key)))
extern Key      g_15b92ac;                           // 0x015b92ac
extern unsigned g_15b910c;                           // 0x015b910c
extern Key      g_15b942c;                           // 0x015b942c
extern Key      g_15b9438;                           // 0x015b9438

struct Property {
    char pad[0x12];
    unsigned short type;                             // +0x12
    unsigned* GetUInt();                             // 0x0041ea00
    bool*     GetBool();                             // 0x0041e920
};

// Owning pointer to a property list (AddRef slot 0 / Release slot 1). operator& resets, like
// the out-parameter smart pointers of the original.
struct RcPtr {
    Obj* p;
    RcPtr() : p(0) {}
    ~RcPtr() { if (p) VCall0<1, void>(p); }
    Obj** operator&()
    {
        Obj* t = *(Obj* volatile*)&p;
        if (t) { p = 0; VCall0<1, void>(t); }
        return &p;
    }
};

struct PropManager {
    // vtable slot 11 (+0x2c)
    bool GetList(unsigned id, unsigned group, Obj** out)
    { return VCall3<11, bool, unsigned, unsigned, Obj**>(this, id, group, out); }
};
PropManager* __cdecl PropertyManager();             // 0x0067de30

bool __cdecl GetPropArray(Obj* list, unsigned id, int* count, Key** arr);       // 0x006a0ae0
inline bool GetProp(Obj* list, unsigned id, Property** out)
{ return VCall2<9, bool, unsigned, Property**>(list, id, out); }                // vtable slot 9 (+0x24)

struct cString {
    char pad[0x1c];
    cString();                                       // 0x006b5060
    ~cString();                                      // 0x006b5240
    const wchar_t* GetText();                        // 0x006b55c0
};
bool __cdecl GetPropertyAsText(Obj* list, unsigned id, cString* out);           // 0x006a1360
void __cdecl SetTooltipText(Obj* window, const wchar_t* text, int a, int b);    // 0x00806de0

struct GlobalUI {
    Obj* FindWindowByID(unsigned id);                // 0x00e012b0 (ret 4)
    void* GetBuffer();                               // 0x0093b6c0
};

struct SpaceGame { Obj* GetPlayerInventory(); };    // 0x00a1ad60
SpaceGame* __cdecl SpaceGameGet();                   // 0x01002bd0

struct Dialog : Obj {                                // cConnectionDialog-like panel, 0x18 bytes
    char pad[0x14];
    Dialog();                                        // 0x00810000
    void Init(const Key* k, int a, int b);           // 0x008120d0 (ret 0xc)
};
struct ToolObjA : Obj { char pad[0x13c]; ToolObjA(); };   // 0x0106da70 (0x140)
struct ToolObjB : Obj { char pad[0x15c]; ToolObjB(); };   // 0x0106e2d0 (0x160)

struct MapResult { void* it; bool ok; };
struct MapEntry {
    unsigned key;
    Obj*     val;
    MapEntry(unsigned k, Obj* v) : key(k), val(v) { if (v) VCall0<0, void>(v); }
    ~MapEntry() { if (val) VCall0<1, void>(val); }
};
struct ObjMap { char pad[0x20]; void Insert(MapResult* r, const MapEntry* e, bool b); };        // 0x00ad9090 (ret 0xc)
struct UIntSet { char pad[0x18]; void Insert(MapResult* r, const unsigned* v); };               // 0x00554020 (ret 8)

struct UIntVec {
    unsigned* mpBegin; unsigned* mpEnd; unsigned* mpCapacity;
    void DoInsertValue(unsigned* pos, const unsigned& v);                       // 0x004558a0 (ret 8)
    __forceinline void push_back(const unsigned& v)
    {
        if (mpEnd < mpCapacity) {
            unsigned* p = mpEnd++;
            if (p) *p = v;
        } else {
            DoInsertValue(mpEnd, v);
        }
    }
};

class Space {
public:
    char     pad0[0x224];
    GlobalUI* ui;             // +0x224
    char     pad228[0x10];
    Obj*     inventory;       // +0x238
    char     pad23c[4];
    UIntVec  tools;           // +0x240
    char     pad24c[8];
    Obj*     dialog;          // +0x254 (AddRef 1 / Release 2)
    char     pad258[4];
    ObjMap   objMap;          // +0x25c
    UIntSet  set27c;          // +0x27c
    Obj*     p294;            // +0x294
    Obj*     p298;            // +0x298
    Obj*     p29c;            // +0x29c

    void ToggleToolUI(unsigned id);                  // 0x0106e3e0 (ret 4)
    void Setup();                                    // 0x01070290
};

// ===========================================================================
// 0x01070890
// ===========================================================================
// @ 0x01070890
void cSPUISpace::ShowSavedGames()
{
    F0106f380();
    unsigned count = StarManager()->mCivs.size();
    for (unsigned i = 0; i < count; ++i) {
        cProperty* value = 0;
        void* civ = StarManager()->mCivs.mpBegin[i];
        if (civ != GetPlayerHomePlanet()->SortBanners()) {
            EffectsManager* effects = EffectsManagerPtr();
            if (value != 0) {
                cProperty* old = value;
                value = 0;
                old->Release();
            }
            if (effects->GetProperty(0xf0eb627c, 0, &value))
                value->V08(0);
            if (value != 0) {
                Matrix3 rot = kSavedGameRot;
                cSavedGameRec rec(kSavedGamePos, rot);
                rec.mPos = *((cCivData*)StarManager()->mCivs.mpBegin[i])->GetCities();
                rec.mFlags |= 4;
                rec.mCount += 1;
                value->V14(&rec);
            }
            if (mSavedGameList.mpEnd < mSavedGameList.mpCapacity) {
                *mSavedGameList.mpEnd++ = value;
                if (value != 0)
                    value->AddRef();
            } else {
                mSavedGameList.insert(mSavedGameList.mpEnd, value);
            }
        }
        if (value != 0)
            value->Release();
    }
}

// ===========================================================================
// 0x01070af0
// ===========================================================================
// @ 0x01070af0
void cFoodwebSlotVec::resize(unsigned count)
{
    if (count > size()) {
        cFoodwebSlotInfo value;
        insert(mpEnd, count - size(), value);
    } else {
        erase(mpBegin + count, mpEnd);
    }
}

// @ 0x01070290
void Space::Setup()
{
    Obj* inv = SpaceGameGet()->GetPlayerInventory();
    Obj* inv2 = inv ? VCall1<3, Obj*, unsigned>(inv, 0x9073163b) : 0;
    AssignRef<0, 1>(inventory, inv2);

    Dialog* dlg = new ("UI", 0, 0, 0, 0) Dialog();
    AssignRef<1, 2>(dialog, (Obj*)dlg);
    ((Dialog*)dialog)->Init(&g_15b92ac, 0, 0x05b598fa);

    RcPtr list;
    PropManager* pm = PropertyManager();
    if (pm->GetList(0x22631cd0, 0x30608f0c, &list)) {
        int  count;
        Key* arr;
        if (GetPropArray(list.p, 0x98f1dfd8, &count, &arr)) {
            for (int i = 0; i < count; ++i) {
                RcPtr item;
                PropManager* pm2 = PropertyManager();
                if (pm2->GetList(arr[i].a, 0x30608f0c, &item)) {
                    Property* p1;
                    if (item.p && GetProp(item.p, 0x8f92bac3, &p1) && p1->type == 10) {
                        Obj* w = ui->FindWindowByID(*p1->GetUInt());
                        cString s;
                        if (w && GetPropertyAsText(item.p, 0x3068d95d, &s))
                            SetTooltipText(w, s.GetText(), -1, 1);
                    }
                    Property* p2;
                    if (item.p && GetProp(item.p, 0x7704db6f, &p2) && p2->type == 10) {
                        unsigned u = *p2->GetUInt();
                        tools.push_back(u);
                        Property* p3;
                        if (item.p && GetProp(item.p, 0x68f99898, &p3) && p3->type == 1 && *p3->GetBool()) {
                            MapResult r;
                            set27c.Insert(&r, &u);
                        }
                        int  count2;
                        Key* arr2;
                        if (GetPropArray(item.p, 0xf01ecafb, &count2, &arr2)) {
                            for (int j = 0; j < count2; ++j) {
                                Obj* o = (Obj*)new ("UI", 0, 0, 0, 0) ToolObjA();
                                if (o) VCall0<0, void>(o);
                                if (INIT_CALL(o, ui->GetBuffer(), arr2[j])) {
                                    MapResult r;
                                    bool added = false;
                                    MapEntry e(u, o);
                                    objMap.Insert(&r, &e, added);
                                    if (g_15b910c == arr2[j].a) {
                                        AssignRef<0, 1>(p294, o);
                                        Obj* t = p294;
                                        VCall1<12, void, int>(t, VCall0<34, int>(inventory));
                                    }
                                }
                                VCall0<1, void>(o);
                            }
                        }
                    }
                }
            }
        }
    }

    // the two preview/effect helpers
    {
        ToolObjA* d;
        if ((d = new ("UI", 0, 0, 0, 0) ToolObjA()) != 0) {
            *(unsigned*)d = 0x0149c33c;
            ((unsigned*)d)[1] = 0x0149c2d0;
            ((unsigned*)d)[3] = 0x0149c334;
        }
        AssignRef<0, 1>(p298, (Obj*)d);
        Obj* t = p298;
        if (INIT_CALL(t, ui->GetBuffer(), g_15b942c))
            VCall0<9, void>(p298);
        else if (p298) {
            Obj* q = p298;
            p298 = 0;
            VCall0<1, void>(q);
        }
    }
    {
        ToolObjB* d = new ("UI", 0, 0, 0, 0) ToolObjB();
        AssignRef<0, 1>(p29c, (Obj*)d);
        Obj* t = p29c;
        if (INIT_CALL(t, ui->GetBuffer(), g_15b9438))
            VCall0<9, void>(p29c);
        else if (p29c) {
            Obj* q = p29c;
            p29c = 0;
            VCall0<1, void>(q);
        }
    }
    ToggleToolUI(*tools.mpBegin);
}
