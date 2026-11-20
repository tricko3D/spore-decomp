// Slice s00c84620 -- SP::cSolarSystem::ActivateEffects(bool, float).
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast

// The layout and helper classes follow slice s00c86aa0 (same cSolarSystem,
// same +0x94/+0x9c model manager members).  Offsets below are retail.

// ============================================================ imports
struct CManager;
struct CStarEffect;
struct CStar;
struct CBody;

template <typename T>
struct AutoRefCount {
    T* mpObject;
    AutoRefCount(T* p) : mpObject(p) { if (mpObject) mpObject->AddRef(); }
    AutoRefCount(const AutoRefCount& o) : mpObject(o.mpObject) { if (mpObject) mpObject->AddRef(); }
    ~AutoRefCount() { T* p = mpObject; if (p) p->Release(); }
    AutoRefCount& operator=(T* p)
    {
        T* pOld = mpObject;
        if (p != pOld) {
            if (p) p->AddRef();
            mpObject = p;
            if (pOld) pOld->Release();
        }
        return *this;
    }
    AutoRefCount& operator=(const AutoRefCount& o)
    {
        T* p = o.mpObject;
        if (p != mpObject) {
            T* pOld = mpObject;
            if (p) p->AddRef();
            mpObject = p;
            if (pOld) pOld->Release();
        }
        return *this;
    }
};

struct Vec3 { float x, y, z; };

// ============================================================ effect parameter block
// Stack block handed to the per-star effect calls.  The +0x14 sub-object is a
// copy of the three colour/axis triples at 0x1694c70.
struct EffectParamSub {
    Vec3 mV0;                            // 0x00
    Vec3 mV1;                            // 0x0c
    Vec3 mV2;                            // 0x18

    void Init(const void* pDesc);        // 0x0041cb40
};

extern float gEffectColorR;              // 0x01694c94
extern float gEffectColorG;              // 0x01694c98
extern float gEffectColorB;              // 0x01694c9c
extern float gEffectAlpha;               // 0x01485720
extern int   gEffectParamDesc;           // 0x01694c70

struct EffectParams {
    unsigned short mStar;                // 0x00
    unsigned short mIndex;               // 0x02
    float          mColorR;              // 0x04
    float          mColorG;              // 0x08
    float          mColorB;              // 0x0c
    float          mAlpha;               // 0x10
    EffectParamSub mSub;                 // 0x14

    EffectParams()
        : mStar(0), mIndex(0),
          mColorR(gEffectColorR), mColorG(gEffectColorG), mColorB(gEffectColorB),
          mAlpha(gEffectAlpha)
    {
        mSub.Init(&gEffectParamDesc);
    }
};

// ============================================================ effect objects
// Ref-counted slot at cCelestialBody::+0xc0.
struct EffectRef {
    CStarEffect* mpObject;
    void Reset();
    CStarEffect* operator->() const { return mpObject; }
    operator CStarEffect*() const { return mpObject; }
};

// Object living in the effect slot; only the slots used here are named, the
// gaps keep the vtable byte offsets.
struct CStarEffect {
    virtual void AddRef();                              // 0x00
    virtual void Release();                             // 0x04
    virtual void v08(bool on);                          // 0x08
    virtual void v0c(int mode);                         // 0x0c
    virtual void v10();                                 // 0x10
    virtual void v14();                                 // 0x14
    virtual void v18(const EffectParams* pParams);      // 0x18
    virtual void v1c();                                 // 0x1c
    virtual void v20(const EffectParams* pParams);      // 0x20
};

inline void EffectRef::Reset()
{
    CStarEffect* p = mpObject;
    if (p) {
        mpObject = 0;
        p->Release();
    }
}

// Scene model manager (scene + 0x94 / + 0x9c are ref-counted handles to it).
struct CManager {
    virtual void AddRef();                              // 0x00
    virtual void Release();                             // 0x04
    virtual bool v08(unsigned int key, unsigned int flags,
                     EffectRef* pOut);                  // 0x08
};

struct CScene {
    char                    pad00[0x94];
    AutoRefCount<CManager>  mp94;                       // 0x94
    char                    pad98[4];
    AutoRefCount<CManager>  mp9c;                       // 0x9c
};

// Creates/attaches the star effect object for one celestial body.
void CreateStarEffect(EffectRef* pEffect, AutoRefCount<CManager> manager,
                      int type);                        // 0x00c83cf0

CScene*  GetSceneA();                                   // 0x00b3d470
int      GetUniverseContext();                          // 0x01021080

// ============================================================ classes
struct CStar {
    char  pad00[0x34];
    int   mBand1;                                       // 0x34
    int   mBand2;                                       // 0x38

    bool  HasBinaryStar();                              // 0x00c8b7e0
    int   GetType();                                    // 0x00c8b550
};

struct CBody {
    char       pad00[0x34];
    int        mType;                                   // 0x34
    char       pad38[0xc0 - 0x38];
    EffectRef  mpEffect;                                // 0xc0
};

struct RawPtrVec {
    void**      mpBegin;
    void**      mpEnd;
    void**      mpCapacity;
    struct Alloc { char* mpName; unsigned int mFlags; } mAllocator;
};

class cSolarSystem {
public:
    char       pad00[0x0c];
    CStar*     mStar;                                   // 0x0c
    RawPtrVec  mPlanets;                                // 0x10
    RawPtrVec  mCelestialBodies;                        // 0x24
    CBody*     mStar1;                                  // 0x38
    CBody*     mStar2;                                  // 0x3c
    RawPtrVec  mPtolemaicHitSpheres;                    // 0x40

    void ActivateEffects(bool activate, float value);
};

// @ 0x00c84620
void cSolarSystem::ActivateEffects(bool activate, float value)
{
    if (activate) {
        AutoRefCount<CManager> pManager = GetSceneA()->mp9c;

        if (GetUniverseContext() == 1) {
            pManager = GetSceneA()->mp94;
            CManager* pEffects = GetSceneA()->mp94.mpObject;
            for (void** pp = mCelestialBodies.mpBegin; pp != mCelestialBodies.mpEnd; ++pp) {
                CBody* pBody = (CBody*)*pp;
                if (pBody->mpEffect == 0) {
                    switch (pBody->mType) {
                    case 2:
                        pBody->mpEffect.Reset();
                        if (pEffects->v08(0x3d6f8d8, 0, &pBody->mpEffect))
                            pBody->mpEffect->v08(false);
                        break;
                    }
                }
            }
        }

        bool binary = mStar->HasBinaryStar();
        if (binary) {
            CreateStarEffect(&mStar1->mpEffect, pManager, mStar->mBand1);
            EffectParams params;
            mStar1->mpEffect->v20(&params);
            params.mAlpha = value;
            params.mIndex++;
            mStar1->mpEffect->v18(&params);

            CreateStarEffect(&mStar2->mpEffect, pManager, mStar->mBand2);
            mStar2->mpEffect->v20(&params);
            params.mAlpha = value;
            params.mIndex++;
            mStar2->mpEffect->v18(&params);
        } else {
            CreateStarEffect(&mStar1->mpEffect, pManager, mStar->GetType());
            EffectParams params;
            mStar1->mpEffect->v20(&params);
            params.mAlpha = value;
            params.mIndex++;
            mStar1->mpEffect->v18(&params);
        }
    } else {
        for (void** pp = mCelestialBodies.mpBegin; pp != mCelestialBodies.mpEnd; ++pp) {
            CBody* pBody = (CBody*)*pp;
            EffectRef& effect = pBody->mpEffect;
            if (effect) {
                effect->v0c(1);
                effect.Reset();
            }
        }
    }
}
