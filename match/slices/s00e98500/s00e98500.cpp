// @ 0x00e98500
// Factory for cPlanetGfxView: the object is allocated through the named allocator, its
// constructor (base ctor at 0x00e92af0 plus the member initialisers) is inlined here, and the
// view is then attached to the spatial object and populated with the planet's effects.
// Class layout and base constructor come from the matched destructor at 0x00e98440.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-

#include "game/types.h"

struct Vector3 { float x, y, z; };
struct Matrix33 { float m[9]; };

struct IEffect
{
    virtual int  AddRef();                                   // +0x00
    virtual int  Release();                                  // +0x04
    virtual void Start(int);                                 // +0x08
    virtual void Stop(int);                                  // +0x0c
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void Configure(int, const Vector3*, int);        // +0x40
    virtual void v44(); virtual void v48(); virtual void v4c(); virtual void v50();
    virtual void v54(); virtual void v58(); virtual void v5c();
    virtual __int64 GetId();                                 // +0x60
};

struct RefPtr
{
    IEffect* mp;
    RefPtr() : mp(0) {}
    ~RefPtr() { if (mp) mp->Release(); }
    IEffect** AsPPTypeParam()
    {
        if (mp)
        {
            IEffect* old = mp;
            mp = 0;
            old->Release();
        }
        return &mp;
    }
};

struct EffectList
{
    IEffect** mpBegin;                                       // +0x00
    IEffect** mpEnd;                                         // +0x04
    IEffect** mpCapacity;                                    // +0x08
    unsigned  mAllocator;                                    // +0x0c
    unsigned  mPad10;                                        // +0x10
    EffectList() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    void push_back(RefPtr* ref);                             // 0x00a7f430
};

struct IEffectFactory
{
    virtual void v00();                                      // +0x00
    virtual void v04();                                      // +0x04
    virtual bool Create(__int64 id, IEffect** out);          // +0x08
};

struct cEffectsHolder
{
    char            mPad00[0x94];
    IEffectFactory* mpFactory94;                             // +0x94
    IEffectFactory* mpFactory98;                             // +0x98
    IEffectFactory* mpFactory9c;                             // +0x9c
};
extern "C" cEffectsHolder* __cdecl GetEffectsHolder();       // 0x00b3d470

extern Vector3  gAxis15a8ab8;                                // 0x015a8ab8
extern Matrix33 gMatrix16c62fc;                              // 0x016c62fc
extern unsigned gModelKey16c62c8;                            // 0x016c62c8

// free helpers
extern "C" void* __cdecl SPNewObject(unsigned size, const char* name, int, int, int, int);  // 0x00f473a0
extern "C" Matrix33* __cdecl RotationMatrix(Matrix33* out, const Vector3& axis, float angle);   // 0x00576b00
extern "C" int  __cdecl GetUniverseContext();                                               // 0x01021080

struct Vec3List
{
    Vector3* mpBegin;                                        // +0x00
    Vector3* mpEnd;                                          // +0x04
    Vector3* mpCapacity;                                     // +0x08
    Vec3List() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    ~Vec3List();                                             // 0x00540520
};
extern "C" void __cdecl SampleOrbit(void* orbit, Vec3List* out, int count);                 // 0x00fe88d0

// planet record (owner of the world object)
struct cRecord
{
    char     mPad00[0x2c];
    unsigned mFlags;                                         // +0x2c
    bool IsOwned();                                          // 0x00bb9b70
    int  GetValue();                                         // 0x00b8d9b0
    bool IsAvatarA();                                        // 0x00bba100
    bool IsAvatarB();                                        // 0x00bb9d90
};

// planet game data
struct cPlanet
{
    char       mPad000[0x10c];
    Matrix33   mMatrix;                                      // +0x10c
    char       mPad130[0x13c - 0x130];
    cRecord*   mpRecord;                                     // +0x13c
    char       mPad140[0x15c - 0x140];
    unsigned   mField15c;                                    // +0x15c
    unsigned   mField160;                                    // +0x160
    char       mPad164[0x16c - 0x164];
    unsigned   mEffectId;                                    // +0x16c
    EffectList mEffects;                                     // +0x170
    char       mPad184[0x1bc - 0x184];
    unsigned   mFlags1bc;                                    // +0x1bc

    int   GetPlanetType() const;                             // 0x00c70880
    void* GetOrbit() const;                                  // 0x00c708a0
    void* GetNameRow(bool wantCol0) const;                   // 0x00c707f0
    bool  IsFlagged() const;                                 // 0x00c71540
};

// spatial object handed to the factory (only Cast matters)
struct cSpatialObject
{
    virtual void v00();                                      // +0x00
    virtual void v04();                                      // +0x04
    virtual void v08();                                      // +0x08
    virtual void* Cast(unsigned type);                       // +0x0c
};

struct cSpatialObjectView
{
    cSpatialObjectView();                                    // 0x00e92af0
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20();
    virtual void Init(void* object);                         // +0x24
    virtual void v28();                                      // +0x28
    char mPad04[0x30];
};

class cPlanetGfxView : public cSpatialObjectView
{
public:
    cPlanetGfxView() : mpPlanet(0) {}

    static void* operator new(unsigned size)
    {
        return SPNewObject(size, "PlanetGfxView", 0, 0, 0, 0);
    }

    virtual void* GetModel();                                // +0x2c
    virtual void  OnImpostorAdded(void* impostor, void* avatar);   // +0x30

    cPlanet*   mpPlanet;                                     // +0x34
    EffectList mEffectsA;                                    // +0x38
    EffectList mEffectsB;                                    // +0x4c
    RefPtr     mSelected;                                    // +0x60
    RefPtr     mHidden;                                      // +0x64

    void AddModelGroupImpostors();                           // 0x00e97350
    void UpdateSelectionEffect();                            // 0x00e978e0
};

// avatar holder / avatar / model reference returned by the avatar lookup
struct cAvatarHolder
{
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58(); virtual void v5c();
    virtual void v60(); virtual void v64(); virtual void v68(); virtual void v6c();
    virtual void* GetAvatar();                               // +0x70
};
extern "C" cAvatarHolder* __cdecl GetAvatarHolder();         // 0x00b3d240

struct cAvatar
{
    virtual void v00();                                      // +0x00
    virtual void v04();                                      // +0x04
    virtual void v08();                                      // +0x08
    virtual void* GetModel(unsigned key, unsigned flags, unsigned a);   // +0x0c
};

struct RefSlot
{
    void operator=(void* p);                                 // 0x00b5f950
};

struct cModelRef
{
    virtual int v00();                                       // +0x00
    virtual int Release();                                   // +0x04
    char         mPad04[0x40 - 0x04];
    unsigned     mRefs40;                                    // +0x40
    char         mPad44[0x64 - 0x44];
    RefSlot      mSlot64;                                    // +0x64
};

struct ModelRefHolder
{
    cModelRef* mp;
    ~ModelRefHolder();                                       // 0x005765e0
};

// @ 0x00e98500
extern "C" cPlanetGfxView* __stdcall MakePlanetGfxView(cSpatialObject* pObject)
{
    cPlanetGfxView* view = new cPlanetGfxView;

    cSpatialObject* obj = pObject;
    cSpatialObject* cast = obj ? (cSpatialObject*)obj->Cast(0x44462a6) : 0;

    view->mpPlanet = (cPlanet*)*(void**)((char*)cast + 0x108);
    view->Init(obj);

    cPlanet* planet = view->mpPlanet;
    if (!planet)
        return view;

    if (planet->mField15c != planet->mField160)
    {
        int type = planet->GetPlanetType();
        if (type >= 2 && type <= 5)
        {
            Matrix33 m;
            view->mpPlanet->mMatrix = *RotationMatrix(&m, gAxis15a8ab8, 0.0f);
            view->AddModelGroupImpostors();
            view->UpdateSelectionEffect();
        }
        else
        {
            view->mpPlanet->mMatrix = gMatrix16c62fc;
        }
    }
    else if (planet->mEffectId != 0)
    {
        RefPtr ref;
        IEffectFactory** ppFactory = &GetEffectsHolder()->mpFactory9c;
        IEffectFactory* factory = *ppFactory;
        if (factory->Create(planet->mEffectId, ref.AsPPTypeParam()))
        {
            view->mpPlanet->mEffects.push_back(&ref);
            ref.mp->Start(0);
            if (ref.mp->GetId() == 0x431c8c8)
            {
                Vec3List points;
                SampleOrbit(view->mpPlanet->GetOrbit(), &points, 0x32);
                ref.mp->Configure(0xd, points.mpBegin, 0x32);
            }
        }

        if (planet->GetPlanetType() == 1)
        {
            cAvatar* avatar = (cAvatar*)GetAvatarHolder()->GetAvatar();
            ModelRefHolder model;
            model.mp = (cModelRef*)avatar->GetModel(0x2e81370, gModelKey16c62c8, 0x10);
            if (model.mp)
            {
                ++model.mp->mRefs40;
                view->OnImpostorAdded(model.mp, avatar);
                model.mp->mSlot64 = (void*)((char*)planet + 0xd4);
            }
        }
    }

    void* nameRow = planet->GetNameRow(false);
    if (nameRow)
    {
        RefPtr ref;
        IEffectFactory** ppFactory = &GetEffectsHolder()->mpFactory9c;
        IEffectFactory* factory = *ppFactory;
        if (factory->Create((__int64)(unsigned)(size_t)nameRow, ref.AsPPTypeParam()))
        {
            view->mpPlanet->mEffects.push_back(&ref);
            ref.mp->Start(0);
        }
    }

    if (planet->GetPlanetType() != 0)
    {
        bool flagged = view->mpPlanet->IsFlagged();
        cRecord* rec = (cRecord*)*(void**)((char*)view->mpPlanet + 0x13c);
        unsigned id = (rec->mFlags & 0x1000) ? 0x5b02bb3 + (flagged ? 3 : 0)
                    : (rec->mFlags & 0x2000) ? 0x5b02bb5 + (flagged ? 3 : 0)
                    :                          0x5b02bb4 + (flagged ? 3 : 0);

        RefPtr ref;
        cEffectsHolder* holder = GetEffectsHolder();
        if (holder && holder->mpFactory98)
        {
            if (holder->mpFactory98->Create(id, ref.AsPPTypeParam()))
                view->mpPlanet->mEffects.push_back(&ref);
        }
    }

    if (GetUniverseContext() == 1)
    {
        cRecord* rec = (cRecord*)*(void**)((char*)planet + 0x13c);
        if (rec->IsOwned())
        {
            RefPtr ref;
            IEffectFactory** ppFactory = &GetEffectsHolder()->mpFactory94;
            IEffectFactory* factory = *ppFactory;
            if (factory->Create((__int64)0x43093342, ref.AsPPTypeParam()))
            {
                view->mEffectsA.push_back(&ref);
                ref.mp->Start(0);
            }
        }
        if (((cRecord*)*(void**)((char*)planet + 0x13c))->GetValue() != 0)
        {
            RefPtr ref;
            IEffectFactory** ppFactory = &GetEffectsHolder()->mpFactory94;
            IEffectFactory* factory = *ppFactory;
            if (factory->Create((__int64)0xd33456e7, ref.AsPPTypeParam()))
            {
                view->mEffectsA.push_back(&ref);
                ref.mp->Start(0);
            }
        }
        if (rec->IsAvatarA() || ((cRecord*)*(void**)((char*)planet + 0x13c))->IsAvatarB())
        {
            RefPtr ref;
            IEffectFactory** ppFactory = &GetEffectsHolder()->mpFactory94;
            IEffectFactory* factory = *ppFactory;
            if (factory->Create((__int64)0x5dbcb07b, ref.AsPPTypeParam()))
            {
                view->mEffectsA.push_back(&ref);
                ref.mp->Start(0);
            }
        }
    }

    view->mpPlanet->mFlags1bc |= 4;
    return view;
}
