// Slice s00d4a930 -- Spore Simulator drama-event / VerbTray cluster.
//
// Modelled types (retail member offsets are read from the disassembly; the PDB
// layout differs, so objects are modelled locally with role-named members and
// explicit padding for the untouched gaps).
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-

typedef unsigned int   u32;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef int            i32;
typedef unsigned int   size_t;

#define FORCEINLINE __forceinline

extern "C" void *operator_new(u32 n, const char *pFile, int a, int b, int c, int d); // 0xf473a0
extern "C" void  operator_delete_array(void *p);                                     // 0xf47380

// --------------------------------------------------------------- extern decls
extern "C" void  *SP_GetCurrentGameMode();          // 0x00b5b800
extern "C" void  *SP_NounManager();                 // 0x00b3d300
extern "C" void  *FUN_00d2e490();                   // 0x00d2e490 (retail pushes no argument)
extern "C" void  *FUN_00d4a5f0(void *w);            // 0x00d4a5f0
extern "C" void   sActivatePanel(void *a, bool b);// 0x00d49300
extern "C" void  *EA_UTFWin_GetManager();           // 0x00957f30
extern "C" void  *cTribeModeStrategy_Instance();    // 0x00cd40b0
extern "C" void  *FUN_00d39000();                   // 0x00d39000
extern "C" void  *FUN_00cc8990();                   // 0x00cc8990
extern "C" void  *FUN_00ef0dc0(void *a);            // 0x00ef0dc0
extern "C" void  *FUN_00d1e830(u32 id);             // 0x00d1e830
extern "C" bool   FUN_00d201f0(void *o);            // 0x00d201f0
extern "C" void  *FUN_00d49270(u32 id);             // 0x00d49270
extern "C" void  *FUN_00d1e610();                   // 0x00d1e610
extern "C" float  SP_ScoreAbilityRange(i32 id);     // 0x00d200a0
extern "C" void  *FUN_00d99500(void *o);            // 0x00d99500
extern "C" u8     FUN_00c0b770(void *o);            // 0x00c0b770
extern "C" void  *FUN_00c04590(void *o);            // 0x00c04590
extern "C" u8     FUN_00c0b860(void *a, void *b);   // 0x00c0b860
extern "C" void   FUN_00d99ac0();                   // 0x00d99ac0
extern "C" void   FUN_00bc97f0(int a, int b, float c, void *d); // 0x00bc97f0
extern "C" i32    FUN_00bca0c0(int a, int b, int c, int d); // 0x00bca0c0
extern "C" void  *FUN_00b1fdb0(void *o);            // 0x00b1fdb0 cGameNounManager::GetAvatar
extern "C" void  *FUN_00b1f9c0(void *o);            // 0x00b1f9c0 cGameNounManager::GetPlayerTribe
extern "C" void  *FUN_00bfc5f0(void *o);            // 0x00bfc5f0 cGameData::SetGameDataOwner2
extern "C" void  *FUN_00c0bbd0(void *o);            // 0x00c0bbd0 cSPCreatureBase::GetSpeciesProfile
extern "C" void  *PropertyManager();                // 0x0067de30
extern "C" u8     FUN_006a10c0(void *plist, u32 hash, void *out); // 0x006a10c0 GetPropertyAsVector2

extern "C" void RBTreeInsert(void *node, void *a, void *b, void *side); // 0x9216a0
extern "C" void *RBTreeIncrement(void *node);                            // 0x921580

extern const float kOne;             // 0x1485720 = 1.0f
extern const float k3600f;           // 0x147256c = 3600.0f
extern const float kCheckForMate;    // 0x1582f88 (mCheckForMateTimer)
extern float gVec0169e440[3];        // 0x0169e440
extern u8    gModeA;                 // 0x01654c01
extern u8    gModeB;                 // 0x01654c02
extern u32   kVtblA;                 // 0x0147b108
extern u32   kVtblB;                 // 0x013ec458
extern u32   kVtblC;                 // 0x0147b278

// ============================================================== refcount base
// Generic refcounted object: vtable at +0x00, the AutoRefCount path releases via
// vtable slot 1 (the intrusive Release thunk).  Only the touched members are named.
typedef void (__fastcall *ReleaseThunk)(void *self);

struct RefObj {
    void **m_vtbl;              // +0x00
};

FORCEINLINE void ReleaseObj(void *obj)
{
    RefObj *o = (RefObj *)obj;
    o->m_vtbl[1] = o->m_vtbl[1];                 // no-op to keep a vtable ref
    ((ReleaseThunk)o->m_vtbl[1])(o);
}

// AutoRefCount<T>::operator=(T*): addref new, store, release old.
FORCEINLINE void AutoRefAssign(void **ppField, void *pNew)
{
    void *pOld = *ppField;
    if (pNew != pOld) {
        if (pNew != 0) {
            ((RefObj *)pNew)->m_vtbl[0] = ((RefObj *)pNew)->m_vtbl[0];
            ((ReleaseThunk)((RefObj *)pNew)->m_vtbl[0])((RefObj *)pNew);
        }
        *ppField = pNew;
        if (pOld != 0) {
            ((ReleaseThunk)((RefObj *)pOld)->m_vtbl[1])((RefObj *)pOld);
        }
    }
}

FORCEINLINE void AutoRefRelease(void **ppField)
{
    void *pOld = *ppField;
    if (pOld != 0) {
        *ppField = 0;
        ((ReleaseThunk)((RefObj *)pOld)->m_vtbl[1])((RefObj *)pOld);
    }
}

// --------------------------------------------------------------- small leaf fns
struct ModeSetter {
    u8  m_pad00[0x44];
    u32 m_value44;     // +0x44
    void SetValue(u32 v);
};

// @ 0x00d4af70
void ModeSetter::SetValue(u32 v)
{
    if (v <= 3) {
        m_value44 = v;
    }
}

struct Thresholded {
    u8    m_pad00[0x2c];
    float m_value2c;   // +0x2c
    bool IsBelowZero();
};

// @ 0x00d4bbb0
bool Thresholded::IsBelowZero()
{
    return 0.0f > m_value2c;
}

// ================================================================ eastl rbtree
struct RBNode {
    RBNode *mpNodeLeft;    // +0x00
    RBNode *mpNodeRight;   // +0x04
    RBNode *mpNodeParent;  // +0x08
    u32     mColor;        // +0x0c
    u32     mKey;          // +0x10
    void   *mValue;        // +0x14
};

// the multimap header: an anchor node at the object base, mpNodeRight = root
struct RBTreeHead {
    RBNode  mAnchor;       // +0x00 (anchor.mpNodeRight = root)
    u32     mCount;        // +0x14
};

// eastl rbtree: `this` points 4 bytes before the anchor node; the root lives at
// this+0xc (the anchor's mpNodeParent slot in this build's layout).
struct RBTree {
    u32     m_gap0;        // +0x00
    RBNode  mAnchor;       // +0x04  anchor node (left=+0x04, right=+0x08, parent=+0x0c)
    u32     mCount;        // +0x18

    void equal_range(RBNode **out, u32 *key);   // 0x00d4b450
};

// @ 0x00d4b450
void RBTree::equal_range(RBNode **out, u32 *key)
{
    RBNode *const root = mAnchor.mpNodeParent;
    RBNode *lo = &mAnchor;
    for (RBNode *n = root; n != 0; ) {
        if (*key < n->mKey) {
            lo = n;
            n = n->mpNodeRight;
        } else {
            n = n->mpNodeLeft;
        }
    }
    RBNode *hi = &mAnchor;
    for (RBNode *n = root; n != 0; ) {
        if (*key > n->mKey) {
            n = n->mpNodeRight;
        } else {
            hi = n;
            n = n->mpNodeLeft;
        }
    }
    out[0] = lo;
    out[1] = hi;
}

// value objects are visited through their vtable slot 0x24
typedef void (__thiscall *VisitFn)(void *self, void *param);

struct TreeValue {
    void **m_vtbl;         // +0x00
};



// ========================================================= creature / world object
// cSPCreatureAnimal: only the offsets touched by this cluster are named.
struct cSPCreatureAnimal {
    void **m_vtbl;              // +0x00
    u8    m_pad04[0x131];
    u8    m_b135;               // +0x135
    u8    m_pad136[0xa12];
    u32   m_idB48;              // +0xb48
    void *m_pB4C;               // +0xb4c
    u8    m_padB50[8];
    u32   m_flagsB58;           // +0xb58
    u8    m_padB5C[2];
    u8    m_b5e;                // +0xb5e
};

struct cSPSubObject {
    u8  m_pad00[0x5fc];
    u32 m_flags5fc;             // +0x5fc
};

// ============================================================ cSPDramaManager
struct cSPDramaManager {
    void         **m_vtbl;              // +0x00
    u8             m_pad04[0x10];
    void          *m_mateTeacher14;     // +0x14  AutoRefCount<creature>
    u8             m_pad18[0x18];
    void          *m_stream30;          // +0x30
    void          *m_tuning34;          // +0x34
    void          *m_mate38;            // +0x38  AutoRefCount<creature>
    float          m_checkMate3c;       // +0x3c
    void          *m_tuning40;          // +0x40
    float          m_progress48;        // +0x48
    u8             m_pad4c[8];
    u8             m_tree54[0x20];      // +0x54  eastl::multimap<u32,AutoRefCount<...>>

    void  MatedWith(cSPCreatureAnimal *mate);   // 0x00d4b180
    void  ClearMate(cSPCreatureAnimal *mate);   // 0x00d4b1d0
    void  PostLoad(void *p);                    // 0x00d4b410
    int   IsValidBabyTeacher(cSPCreatureAnimal *c); // 0x00d4b2f0
};

// ================================================================= cSPDramaManager tree
// @ 0x00d4b410
void cSPDramaManager::PostLoad(void *p)
{
    RBTreeHead *tree = (RBTreeHead *)((u8 *)this + 0x54);
    RBNode *end = &tree->mAnchor;
    for (RBNode *n = tree->mAnchor.mpNodeRight; n != end; n = (RBNode *)RBTreeIncrement(n)) {
        TreeValue *v = (TreeValue *)n->mValue;
        ((VisitFn)v->m_vtbl[0x24 / 4])(v, p);
    }
}

// ------------------------------------------------------------------ vector body
// A vector of 8-byte entries whose +4 slot is a refcounted pointer.
struct RefEntry {
    u32   m_pad00;
    void *m_obj;           // +0x04
};

// cDramaEventInstanceData begins with its eastl-vector body {begin,end}.
struct cDramaEventInstanceData {
    RefEntry *m_pBegin;    // +0x00
    RefEntry *m_pEnd;      // +0x04
    u8        m_pad08[8];
    u32       m_bits10;    // +0x10
    u8        m_pad14[4];
    u32       m_field18;   // +0x18
    u32       m_field1c;   // +0x1c
    float     m_vec20[2];  // +0x20 (cSPVector2)
    float     m_field28;   // +0x28
    float     m_field2c;   // +0x2c
    u8        m_flag30;    // +0x30
    u8        m_pad31[0x0f];
    void     *m_propList40; // +0x40  cPropertyList
    u8        m_pad44[0x30];

    void DestroyMembers();          // 0x00d4b4b0
    void Construct();               // 0x00d4ba40
    void InitFromTuning(int unused); // 0x00d4af80
};

// @ 0x00d4ba40
void cDramaEventInstanceData::Construct()
{
    cDramaEventInstanceData *p = this;
    *(void **)((u8 *)p + 0x04) = (void *)&kVtblB;
    *(u32 *)((u8 *)p + 0x08) = 0;
    *(u8  *)((u8 *)p + 0x0c) = 0;
    *(u32 *)((u8 *)p + 0x14) = 0;
    *(u32 *)((u8 *)p + 0x18) = 0;
    *(u32 *)((u8 *)p + 0x1c) = 0;
    *(float *)((u8 *)p + 0x28) = 0.0f;
    *(float *)((u8 *)p + 0x2c) = 0.0f;
    *(u8  *)((u8 *)p + 0x30) = 1;
    *(u32 *)((u8 *)p + 0x34) = 0;
    *(u32 *)((u8 *)p + 0x38) = 0;
    *(u32 *)((u8 *)p + 0x3c) = 0;
    *(u32 *)((u8 *)p + 0x40) = 0;
    *(u32 *)((u8 *)p + 0x44) = 0;
    *(u32 *)((u8 *)p + 0x48) = 0;
    *(u32 *)((u8 *)p + 0x4c) = 0;
    *(u32 *)((u8 *)p + 0x5c) = 0;
    *(u32 *)((u8 *)p + 0x60) = 0;
    *(u32 *)((u8 *)p + 0x64) = 0;
    *(void **)((u8 *)p + 0x00) = (void *)&kVtblA;
    *(void **)((u8 *)p + 0x04) = (void *)&kVtblC;
    *(float *)((u8 *)p + 0x74) = gVec0169e440[0];
    *(float *)((u8 *)p + 0x78) = gVec0169e440[1];
    *(float *)((u8 *)p + 0x7c) = gVec0169e440[2];
}

// --------------------------------------------------------- property-list lookup
// The tuning list's vtable slot 0x24 finds a property by hash; the found property
// carries a 16-bit type tag at +0x12 and a typed accessor (GetUInt/GetFloat/GetBool).
typedef u32   *(__thiscall *GetUIntP)(void *self);    // 0x41ea00
typedef float *(__thiscall *GetFloatP)(void *self);   // 0x41ea70
typedef u8    *(__thiscall *GetBoolP)(void *self);    // 0x41e920

// @ 0x00d4af80
void cDramaEventInstanceData::InitFromTuning(int unused)
{
    typedef bool (__thiscall *FindFn)(void *self, u32 hash, void **ppOut);
    void *found;
    u32 bits;

    if (m_propList40 != 0) {
        found = 0;
        if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0xc55c98bb, &found)
            && *(u16 *)((u8 *)found + 0x12) == 10) {
            u32 *p = ((GetUIntP)(*(void ***)found)[0])(found);
            bits = (*p != 0) ? (1u << *p) : 0u;
        } else {
            bits = 0;
        }
        m_bits10 = bits;

        if (m_propList40 != 0) {
            found = 0;
            if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0x06bfe2ed, &found)
                && *(u16 *)((u8 *)found + 0x12) == 10) {
                u32 *p = ((GetUIntP)(*(void ***)found)[0])(found);
                *(u32 *)((u8 *)this + 0x1c) = *p;
            }
        }
    }

    FUN_006a10c0(m_propList40, 0xd2e804ee, (u8 *)this + 0x20);

    if (m_propList40 != 0) {
        found = 0;
        if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0x346dbe16, &found)
            && *(u16 *)((u8 *)found + 0x12) == 1) {
            u8 *p = ((GetBoolP)(*(void ***)found)[0])(found);
            *(u8 *)((u8 *)this + 0x30) = *p;
        }
    }

    if (unused != 0) return;

    if (m_propList40 != 0) {
        found = 0;
        if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0x332c289d, &found)
            && *(u16 *)((u8 *)found + 0x12) == 10) {
            u32 *p = ((GetUIntP)(*(void ***)found)[0])(found);
            *(u32 *)((u8 *)this + 0x18) = *p;
        }
    }

    if (m_propList40 != 0) {
        found = 0;
        if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0x920a2318, &found)
            && *(u16 *)((u8 *)found + 0x12) == 13) {
            float *p = ((GetFloatP)(*(void ***)found)[0])(found);
            *(float *)((u8 *)this + 0x28) = *p;
        }
    }

    if (m_propList40 != 0) {
        found = 0;
        if (((FindFn)(*(void ***)m_propList40)[0x24 / 4])(m_propList40, 0xb1f42539, &found)
            && *(u16 *)((u8 *)found + 0x12) == 13) {
            float *p = ((GetFloatP)(*(void ***)found)[0])(found);
            *(float *)((u8 *)this + 0x2c) = *p;
        }
    }
}

struct RefRange {
    RefEntry *m_begin;     // +0x00
    RefEntry *m_end;       // +0x04
};
typedef void (__thiscall *NotifyFn)(void *self);
typedef void (__thiscall *NotifyFnP)(void *self, void *param);

// @ 0x00d4b390
u8 FUN_00d4b390(void *param, RefRange *range)
{
    RefEntry *end = range->m_end;
    for (RefEntry *it = range->m_begin; it != end; it++) {
        void *obj = it->m_obj;
        if (obj != 0) {
            void **vtbl = *(void ***)obj;
            ((NotifyFnP)vtbl[0x1c / 4])(obj, param);
        }
    }
    return 1;
}

// @ 0x00d4b4b0
void cDramaEventInstanceData::DestroyMembers()
{
    RefEntry *end = m_pEnd;
    for (RefEntry *it = m_pBegin; it < end; it++) {
        void *obj = it->m_obj;
        if (obj != 0) {
            void **vtbl = *(void ***)obj;
            ((NotifyFn)vtbl[4 / 4])(obj);
        }
    }
    if (m_pBegin != 0) {
        operator_delete_array(m_pBegin);
    }
}

// @ 0x00d4b180
void cSPDramaManager::MatedWith(cSPCreatureAnimal *mate)
{
    if (m_mate38 != 0) {
        ((cSPCreatureAnimal *)m_mate38)->m_flagsB58 &= ~0x10u;
        ((cSPSubObject *)((cSPCreatureAnimal *)m_mate38)->m_pB4C)->m_flags5fc &= ~0x4000u;
        AutoRefRelease(&m_mate38);
        m_checkMate3c = kCheckForMate;
    }
}

// @ 0x00d4b1d0
void cSPDramaManager::ClearMate(cSPCreatureAnimal *mate)
{
    mate->m_flagsB58 &= ~0x10u;
    ((cSPSubObject *)mate->m_pB4C)->m_flags5fc &= ~0x4000u;
    if (mate == (cSPCreatureAnimal *)m_mate38) {
        AutoRefRelease(&m_mate38);
        m_checkMate3c = kCheckForMate;
    }
}

// @ 0x00d4b2f0
int cSPDramaManager::IsValidBabyTeacher(cSPCreatureAnimal *c)
{
    if (c != 0
        && !((u8 (__thiscall *)(void *))(*(void ***)c)[0x2c / 4])(c)
        && c->m_b135 != 0
        && c->m_b5e == 0
        && ((u8 (__thiscall *)(void *))FUN_00c0b770)(c)
        && ((void *(__thiscall *)(void *))FUN_00c0bbd0)(c) == ((void *(__thiscall *)(void *))FUN_00c0bbd0)(((void *(__thiscall *)(void *))FUN_00b1fdb0)(SP_NounManager()))
        && !((u8 (__thiscall *)(void *))(*(void ***)((u8 *)c + 0xc0))[0x58 / 4])((u8 *)c + 0xc0)
        && c != (cSPCreatureAnimal *)m_mateTeacher14
        && ((int (__thiscall *)(void *, int, int, int, int))FUN_00bca0c0)((void *)c->m_idB48, 3, 1, 0, 0) == -1)
    {
        return 1;
    }
    return 0;
}

// @ 0x00d4b3c0
// A tuning-owner object (its own vtable; three tuning slots and one rebuilt here).
struct TuningOwner {
    void        **m_vtbl;       // +0x00
    u8            m_pad04[0x30];
    void         *m_tuning34;   // +0x34
    void         *m_field38;    // +0x38
    void         *m_tuning3c;   // +0x3c
    void         *m_tuning40;   // +0x40

    void ReloadTuning(int unused);    // 0x00d4b3c0
};

// @ 0x00d4b3c0
void TuningOwner::ReloadTuning(int unused)
{
    void *pm = PropertyManager();
    if (m_tuning40 != 0) {
        void *old = m_tuning40;
        m_tuning40 = 0;
        void **vtbl = *(void ***)old;
        ((void (__thiscall *)(void *))vtbl[4 / 4])(old);
    }
    void **pvtbl = *(void ***)pm;
    ((void (__thiscall *)(void *, void *, void *, void **))pvtbl[0x2c / 4])(pm, m_tuning34, m_tuning3c, &m_tuning40);
    void **vtbl = *(void ***)this;
    ((void (__thiscall *)(void *, int))vtbl[0x28 / 4])(this, 1);
}

// ------------------------------------------------------------------------ misc
// @ 0x00d4bbd0
struct Vec3 { float x, y, z; };

void *FUN_00d4bbd0(Vec3 *out)
{
    u8 *mode = (u8 *)SP_GetCurrentGameMode();
    if (mode == &gModeA || mode == &gModeB) {
        void *src = (mode == &gModeA)
            ? (u8 *)FUN_00b1fdb0(SP_NounManager()) + 0xc0
            : (u8 *)FUN_00bfc5f0(SP_NounManager()) + 0x120;
        void **vtbl = *(void ***)src;
        Vec3 *v = (Vec3 *)((void *(__thiscall *)(void *))vtbl[0x2c / 4])(src);
        out->x = v->x;
        out->y = v->y;
        out->z = v->z;
        return out;
    }
    out->x = gVec0169e440[0];
    out->y = gVec0169e440[1];
    out->z = gVec0169e440[2];
    return out;
}

// ============================================================== verb tray update
// The tray table sits behind a global pointer: five rows of five {id, active}
// entries, then the live row/column counts and the two per-row id bases.
struct VerbEntry {
    i32 m_id;                // +0x00
    u8  m_active;            // +0x04
    u8  m_pad05[3];
};

struct VerbGrid {
    VerbEntry m_entries[5][5];   // +0x000
    i32       m_rowCount;        // +0xc8
    i32       m_colCount;        // +0xcc
    u32       m_base0;           // +0xd0
    u32       m_base1;           // +0xd4
};

extern VerbGrid *gVerbGrid;      // 0x0169e3ec
extern void     *gModeCContext;  // 0x016c7aa4

// The per-verb score rows looked up by id: 16-byte entries whose +4 slot is the
// float score, behind a {first,last} range inside the ability table.
struct ScoredVerb {
    u32   m_kind;            // +0x00
    float m_score;           // +0x04
    u8    m_pad08[8];        // +0x08
};

struct VerbScoreTable {
    u8          m_pad0000[0xc28];
    ScoredVerb *m_pFirst;    // +0xc28
    ScoredVerb *m_pLast;     // +0xc2c
};

extern "C" void *FUN_008105b0(void *mgr, u32 id, int flag); // noun lookup by id
extern "C" void *FUN_00d1e830(u32 id);                      // verb -> creature
extern "C" bool  FUN_00d201f0(void *o);                     // creature predicate
extern "C" void *FUN_00d49270(u32 id);                      // verb -> instance
extern "C" void *FUN_00d1e610();                            // ability/score table

// The species/game frame tag that SP_GetCurrentGameMode (0x00b5b800) reports.
enum ModeTag {
    kModeTagA = 0x1654c01,
    kModeTagB = 0x1654c02,
    kModeTagC = 0x1654c10
};

typedef void *(__thiscall *SlotArgPtrFn)(void *self, int a);
typedef void *(__thiscall *SlotPtrFn)(void *self);
typedef u8    (__thiscall *SlotBoolFn)(void *self);
typedef void  (__thiscall *SlotSetFn)(void *self, int a, bool b);
typedef void *(__thiscall *SlotHashFn)(void *self, u32 a);
typedef void  (__thiscall *SlotVoidFn)(void *self);
typedef void *(__thiscall *FindNounFn)(void *self, u32 id, int flag);
typedef void *(__thiscall *GetAvatarFn)(void *self);

// The id base of one tray row.  Retail re-reads the frame tag before picking
// the base, and the pick itself is a second dispatch on the row: row 3 (and
// anything past 4) falls straight through without touching the base.
FORCEINLINE void LoadRowBase(i32 row, u32 *pBase)
{
    SP_GetCurrentGameMode();
    switch (row) {
    case 0:
        *pBase = gVerbGrid->m_base0;
        break;
    case 1:
        *pBase = gVerbGrid->m_base1;
        break;
    case 2:
        *pBase = 0x1012030;
        break;
    case 4:
        *pBase = 0x1012040;
        break;
    }
}

// The state object behind a verb is reached through slot 0x0c and toggled
// through its own slot 0x28: (4, enabled).
FORCEINLINE void SetVerbState(void *verb, int enabled)
{
    void *state = ((SlotHashFn)(*(void ***)verb)[0x0c / 4])(verb, 0x8ed27e7a);
    ((SlotSetFn)(*(void ***)state)[0x28 / 4])(state, 4, enabled);
}

// @ 0x00d4a930
void RefreshVerbTray()
{
    u32  base;
    i32  i;
    i32  selected = 1;
    bool shown = true;

    switch ((u32)(size_t)SP_GetCurrentGameMode()) {
    case kModeTagA:
    case kModeTagC:
        selected = (i32)FUN_00d2e490();
        break;
    case kModeTagB:
        selected = *(i32 *)((u8 *)cTribeModeStrategy_Instance() + 0x390);
        shown = false;
        break;
    }

    void *ui = EA_UTFWin_GetManager();
    void *panel = ((SlotArgPtrFn)(*(void ***)ui)[0x48 / 4])(ui, 1);
    if (panel != 0) {
        panel = ((SlotPtrFn)(*(void ***)panel)[0x1c / 4])(panel);
    }
    FUN_00d4a5f0(panel);

    for (i = 0; i < gVerbGrid->m_rowCount; i++) {
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 4:
            LoadRowBase(i, &base);
            break;
        default:
            continue;
        }

        for (i32 j = 0; j < gVerbGrid->m_colCount; j++) {
            VerbEntry *entry = &gVerbGrid->m_entries[i][j];
            if (entry->m_id == -1) {
                continue;
            }

            void *noun;
            switch ((u32)(size_t)SP_GetCurrentGameMode()) {
            case kModeTagA:
                noun = FUN_00d39000();
                break;
            case kModeTagB:
                noun = FUN_00cc8990();
                break;
            case kModeTagC:
                noun = ((void *(__thiscall *)(void *))FUN_00ef0dc0)(gModeCContext);
                break;
            default:
                noun = 0;
                break;
            }

            // The reactor id of a tray cell is its row's base plus the column.
            u32 id = base + (u32)j;
            void *verb = ((FindNounFn)FUN_008105b0)(noun, id, 1);
            if (verb == 0) {
                continue;
            }

            i32 verbId = entry->m_id;
            void *info = FUN_00d1e830((u32)verbId);
            if (info != 0) {
                bool want = FUN_00d201f0(info);
                bool has = ((SlotBoolFn)(*(void ***)verb)[0x28 / 4])(verb) & 1;
                if (i == selected && want) {
                    shown = true;
                }
                if (want != has) {
                    ((SlotSetFn)(*(void ***)verb)[0x7c / 4])(verb, 1, want);
                }
                if (want && FUN_00d49270((u32)verbId) == 0) {
                    SetVerbState(verb, 0);
                    goto scored;
                }
            }
            SetVerbState(verb, 1);

        scored:
            {
                i32 id2 = entry->m_id;
                float score = 0.0f;
                bool haveScore = false;

                VerbScoreTable *table = (VerbScoreTable *)FUN_00d1e610();
                if (table != 0) {
                    ScoredVerb *first = table->m_pFirst;
                    if (id2 >= 0 && id2 < (i32)(table->m_pLast - first)) {
                        score = first[id2].m_score;
                        haveScore = true;
                    } else if ((u32)(size_t)SP_GetCurrentGameMode() == kModeTagB) {
                        score = SP_ScoreAbilityRange(id2);
                        haveScore = true;
                    }
                }

                if (haveScore && score == kOne) {
                    if (entry->m_active != 0) {
                        ((SlotVoidFn)(*(void ***)verb)[0x90 / 4])(verb);
                    }
                } else {
                    entry->m_active = 1;
                    ((SlotVoidFn)(*(void ***)verb)[0x90 / 4])(verb);
                }
            }
        }
    }

    switch ((u32)(size_t)SP_GetCurrentGameMode()) {
    case kModeTagA:
    case kModeTagC:
        if (((GetAvatarFn)FUN_00b1fdb0)(SP_NounManager()) != 0) {
            sActivatePanel(FUN_00d2e490(), shown);
        }
        break;
    case kModeTagB:
        sActivatePanel(*(void **)((u8 *)cTribeModeStrategy_Instance() + 0x390), shown);
        break;
    }
}
