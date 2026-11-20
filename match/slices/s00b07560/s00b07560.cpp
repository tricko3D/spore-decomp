// s00b07560 - 0x00b07560 (1045 bytes, __thiscall, ret 0x1c, 7 stack args).
//
// Seats one plant on the planet.  It builds (or looks up) the plant's grid entry - a
// Simulator::cTreeData, 0x80 bytes, ctor 0x00b01770, see slice s00b01690 - snaps the
// plant's transform to the planet surface, gives the entry to the grid (0x00b02350) and
// then walks the species' model positions, appending one 0x38-byte element per position
// to the plant's element vector (plant+0xa4) and numbering the slots (plant+0x19c).
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (no /EHsc).  Names are Claude-coined; the
// manager class is not recovered, it is the same one as in s00b07980 (tree at
// this+0x1e5f0, grid at this+0x16500).
#include "types.h"

template<class T> static inline T& At(void* p, int off) { return *(T*)((char*)p + off); }

struct Vector3 { float x, y, z; };

// cSPTransform (0x4c bytes per request element)
struct Xform
{
    uint16_t flags;
    uint16_t counter;
    Vector3  pos;
    uint32_t pad[0x4c / 4 - 5];
    Xform& operator=(const Xform&);                 // 0x00537dc0 (thiscall, ret 4)
};

struct VfxRef                                        // EA::AutoRefCount<cIVisualEffect>
{
    void* p;
    void* assign(VfxRef* src);                       // 0x00b5f950
};

struct Model
{
    char     pad[0x6b8];
    uint8_t* elems;                                  // +0x6b8 array of 0x38-byte entries
    uint32_t GetCount();                             // 0x004d9070
};

struct Species
{
    virtual void pv0();
    virtual void pv1();
    virtual void pv2();
    virtual void pv3();
    virtual void pv4();
    virtual void pv5();
    virtual const Vector3* GetPosition();            // vtable +0x18
    Model*   GetModelWorld();                        // 0x00ad2830 (returns [this+0xa0])
    uint32_t GetKind();                              // 0x00b8f860
    char     pad[0x90];
    Vector3  radius;                                 // +0x94
};

struct PlanetModel
{
    Vector3* DirectionToSurfacePosition(Vector3* out, const Vector3* in);  // 0x00b815a0 (ret 8)
    void     Rebuild();                                                    // 0x00b88420
    uint32_t GetAverageRadius(const Vector3* pos);                         // 0x00b7e840 (ret 4)
};

// 0x80-byte grid entry created for the plant (ctor 0x00b01770).
struct TreeData
{
    void*    vtbl;                                   // +0x00
    void*    vtbl8;                                  // +0x04
    uint32_t f08;                                    // +0x08
    uint16_t f0c;                                    // +0x0c Xform flags
    uint16_t f0e;                                    // +0x0e Xform counter
    Vector3  f10;                                    // +0x10 Xform position
    float    f1c;                                    // +0x1c
    uint32_t m20[10];                                // +0x20
    uint32_t f48;                                    // +0x48 slot mask, low
    uint32_t f4c;                                    // +0x4c slot mask, high
    uint32_t f50;                                    // +0x50 key
    uint32_t f54;                                    // +0x54 slot index
    uint32_t f58;                                    // +0x58 species kind
    Vector3  f5c;                                    // +0x5c
    uint32_t f68;                                    // +0x68
    float    f6c;                                    // +0x6c grid radius
    float    f70;                                    // +0x70
    float    f74;                                    // +0x74
    uint32_t f78;                                    // +0x78
    uint8_t  f7c;                                    // +0x7c
    TreeData* init();                                // 0x00b01770 (ctor, returns this)
    void      SetSurface(const Vector3* pos);        // 0x00affde0
};

struct AutoRef {                                     // EA::AutoRefCount<TreeData>
    void* p;
    AutoRef(void* src);                              // 0x00572660
};

struct Bucket                                        // per-slot bucket, a vector of entries
{
    void** begin;                                    // +0x00
    void** end;                                      // +0x04
    void** cap;                                      // +0x08
    void   push(AutoRef* ref);                       // 0x00e1c7f0
};

struct Elem { void construct(const void* src); };    // 0x0040ce80 (thiscall)

struct ElemVec                                       // plant+0xa4, 0x38-byte elements
{
    void** begin; void** end; void** cap;
    void   reserve(uint32_t n);                      // 0x00b04c50
    void   grow(void* pos, const void* src);         // 0x00423960
};

struct IntVec                                        // plant+0x19c
{
    uint32_t* begin; uint32_t* end; uint32_t* cap;
    void      reserve(uint32_t n);                    // 0x004e0880
};

struct RBTree
{
    void* root;                                      // +0x00
    void* head;                                      // +0x04
    void* find(const void* key);                      // 0x00b03f90
};

struct NodeMask { uint32_t mask(uint32_t kind); };    // 0x00aff250 (this = tree node + 8)

struct Plant
{
    virtual int AddRef();
    virtual int Release();
};

struct NounMgr
{
    bool  SetupPlant(Plant* p, Species* s, Xform* elem, bool isEmpty, bool notFlag,
                     int* idx, VfxRef* vfx);          // 0x00b07560
    void* findSlot(uint32_t id, uint32_t kind);       // 0x00b04ba0
    void  SetEntry(uint8_t add, void* obj);           // 0x00b02350 (arg order from the call site)

    char   pad[0x1e5f0];
    RBTree mTree;                                     // +0x1e5f0
};

void*    __cdecl op_new(uint32_t size, const char* name, int a, int b, const char* c, int d);  // 0x00f473a0
PlanetModel* __cdecl GetPlanetModel();                 // 0x00b3d350
void*    __cdecl findEntry(void* bucket, uint32_t index);   // 0x00afff30
uint32_t __cdecl bucketSize(void* bucket);                  // 0x00affe10
uint32_t __cdecl slotMask(uint32_t bit);                    // 0x00aff160
void     __stdcall growInt(uint32_t* pos, const uint32_t* value);  // 0x004558a0 (ret 8)

// std::max spelled through references: the zero operand needs a stack slot.
template<class T> static inline const T& maxRef(const T& a, const T& b)
{
    return a < b ? b : a;
}

// @ 0x00b07560
bool NounMgr::SetupPlant(Plant* p, Species* s, Xform* elem, bool isEmpty, bool notFlag,
                         int* idx, VfxRef* vfx)
{
    Model*        model = s ? s->GetModelWorld() : 0;
    PlanetModel*  planet = GetPlanetModel();
    if (!model || !planet)
        return false;
    if (model->GetCount() <= 0)
        return false;

    Xform* pxf = &At<Xform>(p, 0x58);
    *pxf = *elem;

    uint32_t kind = s->GetKind();
    At<uint32_t>(p, 0x50) = kind;

    void* node = 0;
    void* found = mTree.find(&At<uint32_t>(p, 0x4c));
    if (found != At<void*>(this, 0x1e5f4))
        node = (char*)found + 8;

    Bucket* bucket = (Bucket*)findSlot(At<uint32_t>(p, 0x4c), kind);

    Vector3 dir = { elem->pos.x, elem->pos.y, elem->pos.z };
    Vector3 surface;
    Vector3* surf = planet->DirectionToSurfacePosition(&surface, &dir);
    dir = *surf;
    pxf->pos = *surf;
    pxf->flags |= 4;
    pxf->counter++;

    At<VfxRef>(p, 0x1c4).assign(vfx);
    ElemVec* ev = (ElemVec*)((char*)p + 0xa4);
    ev->reserve(model->GetCount());
    IntVec*  iv = (IntVec*)((char*)p + 0x19c);
    iv->reserve(model->GetCount());

    TreeData* item = 0;
    if (isEmpty) {
        if (bucket) {
            TreeData* td = (TreeData*)op_new(0x80, (const char*)0x145c804, 0, 0, 0, 0);
            AutoRef ref(td ? td->init() : 0);
            bucket->push(&ref);
            if (ref.p)
                (*(void(__thiscall**)(void*))(*(char**)ref.p + 4))(ref.p);
            item = ((TreeData**)bucket->end)[-1];

            item->f48 = 0;
            item->f4c = 0;
            item->f50 = At<uint32_t>(p, 0x4c);
            item->f54 = At<uint32_t>(p, 0x54);
            item->f58 = At<uint32_t>(p, 0x50);
            item->f5c = *s->GetPosition();
            int zero = 0;
            int last = (int)model->GetCount() - 1;
            const int& n = last < 0 ? zero : last;
            item->f4c |= slotMask((uint32_t)(1ull << n));
            At<Xform>(item, 0x0c) = *pxf;
            if (notFlag) {
                item->f0e++;
                item->f1c = 1.0f;
            }
            item->f6c = s->radius.x * At<float>(p, 0x68);
            item->f70 = s->radius.y * At<float>(p, 0x68);
            item->f74 = s->radius.z * At<float>(p, 0x68);
            SetEntry(1, item);
        }
    } else {
        item = (TreeData*)findEntry(bucket, At<uint32_t>(p, 0x54));
        if (item) {
            item->SetSurface(&surface);
            if (!notFlag)
                At<Xform>(item, 0x0c) = *pxf;
            SetEntry(1, item);
        }
    }

    int limit = 0;
    int mask  = 0;
    planet->Rebuild();
    uint32_t rad = planet->GetAverageRadius(&dir);
    if (item && (rad & 0x18000000)) {
        item->f48 = 0;
        item->f4c = 0;
    } else {
        mask  = node ? (int)((NodeMask*)node)->mask(At<uint32_t>(p, 0x50)) : 0;
        limit = (int)bucketSize(bucket);
    }

    uint32_t count = model->GetCount();
    for (uint32_t i = 0, off = 0; i < count; ++i) {
        void* src = model->elems + off;
        if (ev->end < ev->cap) {
            void* slot = ev->end;
            ev->end = (void**)((char*)ev->end + 0x38);
            if (slot)
                ((Elem*)slot)->construct(src);
        } else {
            ev->grow(ev->end, src);
        }

        ++*idx;
        uint32_t v = (uint32_t)*idx;
        if (iv->end < iv->cap) {
            uint32_t* slot = iv->end;
            iv->end = slot + 1;
            if (slot)
                *slot = v;
        } else {
            growInt(iv->end, &v);
        }

        if (item && isEmpty && limit < mask) {
            At<uint64_t>(item, 0x48) |= 1ull << i;
            ++limit;
        }
        off += 0x38;
    }
    return true;
}
