// @ 0x00b84270
// cPlanetModel: rebuilds the terrain volume, resets the map layers and re-runs
// the two placement passes over the sphere's world objects.
#include "types.h"
#include <string.h>
#include <intrin.h>

typedef unsigned int uint32;

struct Vec3 {
    float x, y, z;
    Vec3() {}
    explicit Vec3(float v) : x(v), y(v), z(v) {}
};

extern const float kFloatMax;                   // 0x01465438
extern const float kNegFloatMax;                // 0x013f51ac

// Bounding volume: two corners, initially an empty box (+FLT_MAX / -FLT_MAX).
struct cBoundingBox {
    Vec3 vMin;
    Vec3 vMax;
    cBoundingBox()
    {
        Vec3 v(kFloatMax);
        vMin = v;
        v = Vec3(kNegFloatMax);
        vMax = v;
    }
};

// ---- terrain map set (see s00b832d0) ----
struct cTerrainMapSet {
    char  pad00[0x34];
    float f34;                                  // base height
    float f38;                                  // height scale
    char  pad3c[0x54 - 0x3c];
    float f54;                                  // height factor
};

// ---- interfaces ----
// Slots are named only where recovered; placeholders keep vtable offsets exact.

// World sphere owned by the planet model (+0x20).
struct cWorldSphere {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2C();
    virtual void p30(); virtual void p34(); virtual void p38(); virtual void p3C();
    virtual void p40(); virtual void p44(); virtual void p48(); virtual void p4C();
    virtual void p50(); virtual void p54(); virtual void p58(); virtual void p5C();
    virtual void p60(); virtual void p64(); virtual void p68(); virtual void p6C();
    virtual void p70(); virtual void p74(); virtual void p78(); virtual void p7C();
    virtual void p80(); virtual void p84(); virtual void p88(); virtual void p8C();
    virtual void p90(); virtual void p94();
    /* 98h */ virtual void SetActive(int active);

    char pad04[0x18 - 0x04];
    Vec3 v18;                                   // 0x18
    char pad24[0x924 - 0x24];
    void* p924;                                 // 0x924
};

// Terrain sphere interface behind cPlanetModel::mpISphere (+0x24).
struct cISphere {
    virtual void p00(); virtual void p04(); virtual void p08();
    /* 0Ch */ virtual cTerrainMapSet* GetMapSet();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2C();
    virtual void p30(); virtual void p34(); virtual void p38(); virtual void p3C();
    virtual void p40(); virtual void p44(); virtual void p48(); virtual void p4C();
    virtual void p50(); virtual void p54(); virtual void p58(); virtual void p5C();
    virtual void p60(); virtual void p64(); virtual void p68(); virtual void p6C();
    virtual void p70(); virtual void p74(); virtual void p78(); virtual void p7C();
    virtual void p80(); virtual void p84(); virtual void p88(); virtual void p8C();
    virtual void p90(); virtual void p94(); virtual void p98(); virtual void p9C();
    virtual void pA0(); virtual void pA4(); virtual void pA8(); virtual void pAC();
    /* B0h */ virtual int CollectObjects(void*** out);
    virtual void pB4(); virtual void pB8(); virtual void pBC(); virtual void pC0();
    virtual void pC4(); virtual void pC8(); virtual void pCC(); virtual void pD0();
    virtual void pD4(); virtual void pD8(); virtual void pDC(); virtual void pE0();
    /* E4h */ virtual void Attach(void* object, int mode);
    virtual void pE8(); virtual void pEC();
    /* F0h */ virtual void Attach2(void* object, int mode);
};

// Terrain volume built by 0x00b3d520.
struct cTerrainVolume {
    /* 00h */ virtual void AddRef();
    /* 04h */ virtual void Release();
    virtual void p08(); virtual void p0C(); virtual void p10(); virtual void p14();
    virtual void p18(); virtual void p1C(); virtual void p20(); virtual void p24();
    virtual void p28(); virtual void p2C(); virtual void p30(); virtual void p34();
    virtual void p38(); virtual void p3C(); virtual void p40(); virtual void p44();
    virtual void p48(); virtual void p4C(); virtual void p50(); virtual void p54();
    virtual void p58(); virtual void p5C(); virtual void p60(); virtual void p64();
    virtual void p68(); virtual void p6C(); virtual void p70(); virtual void p74();
    virtual void p78(); virtual void p7C(); virtual void p80(); virtual void p84();
    virtual void p88(); virtual void p8C(); virtual void p90(); virtual void p94();
    virtual void p98(); virtual void p9C(); virtual void pA0(); virtual void pA4();
    virtual void pA8(); virtual void pAC(); virtual void pB0(); virtual void pB4();
    virtual void pB8(); virtual void pBC(); virtual void pC0(); virtual void pC4();
    virtual void pC8(); virtual void pCC(); virtual void pD0(); virtual void pD4();
    virtual void pD8(); virtual void pDC(); virtual void pE0(); virtual void pE4();
    virtual void pE8(); virtual void pEC(); virtual void pF0(); virtual void pF4();
    virtual void pF8(); virtual void pFC(); virtual void p100(); virtual void p104();
    virtual void p108(); virtual void p10C(); virtual void p110(); virtual void p114();
    virtual void p118(); virtual void p11C(); virtual void p120(); virtual void p124();
    virtual void p128(); virtual void p12C(); virtual void p130(); virtual void p134();
    virtual void p138(); virtual void p13C(); virtual void p140(); virtual void p144();
    virtual void p148(); virtual void p14C();
    /* 150h */ virtual void Setup(const cBoundingBox* box, float a, float b);
};

// Message server (0x0067dcc0).
struct cMessageServer {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    /* 20h */ virtual void Subscribe(void* listener, const char* message);
};

// Change notifier (0x0067dd50).
struct cNotifier {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2C();
    virtual void p30(); virtual void p34(); virtual void p38(); virtual void p3C();
    virtual void p40(); virtual void p44(); virtual void p48();
    /* 4Ch */ virtual void Notify(void* object, int event, int value);
};

// Plugin host (0x0067dd80).
struct cPluginHost {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18();
    /* 1Ch */ virtual void* Create(const char* name);
};

// Overlay interface (0x0067de00).
struct cOverlay {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2C();
    virtual void p30(); virtual void p34(); virtual void p38(); virtual void p3C();
    virtual void p40(); virtual void p44(); virtual void p48(); virtual void p4C();
    virtual void p50(); virtual void p54(); virtual void p58(); virtual void p5C();
    virtual void p60(); virtual void p64(); virtual void p68(); virtual void p6C();
    virtual void p70(); virtual void p74(); virtual void p78(); virtual void p7C();
    virtual void p80(); virtual void p84(); virtual void p88(); virtual void p8C();
    virtual void p90(); virtual void p94(); virtual void p98(); virtual void p9C();
    virtual void pA0();
    /* A4h */ virtual void Draw(const cBoundingBox* box, float a, float b);
};

// World/service object reached through the global at 0x0167ea54.
struct cWorldHost {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0C();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1C();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2C();
    virtual void p30(); virtual void p34(); virtual void p38();
    /* 3Ch */ virtual void Handle(void* object);
};

// ---- simulator-side singletons ----
struct cSimObject { char pad00[0xf0]; void* pF0; };   // +0xf0 tested for null
struct cSimulator {
    char pad00[0x3c];
    cSimObject* p3c;
    bool IsFeatureEnabled(const char* name);    // 0x006a25a0
};

// Object with a null test at 0x00f678a0.
struct cQueried {
    bool IsSet();                               // 0x00f678a0
};

// ---- property bag attached to world objects (+0x90) ----
struct cPropertyBag {
    virtual void slot00(); virtual void slot04(); virtual void slot08();
    virtual void slot0C(); virtual void slot10(); virtual void slot14();
    virtual void slot18(); virtual void slot1C(); virtual void slot20();
    /* 24h */ virtual bool GetProperty(uint32 id, void** out);
};

// Property value: type tag at +0x12, string/element tag at +0x10 (0x30 = heap).
struct cPropertyVariant {
    char pad00[0x10];
    unsigned char mFlags;                       // 0x10
    char pad11;
    unsigned short mType;                       // 0x12

    const char* AsString() const
    {
        return (mFlags & 0x30) ? *(const char* const*)this : (const char*)this;
    }
};

// ---- world object visited by the placement loop ----
struct cLayerObject { char pad00[0x28]; char mFlag28; };

struct cWorldObject {
    char pad00[0x0c];
    Vec3 v0c;                                   // 0x0c position
    char pad18[0x90 - 0x18];
    cPropertyBag* mpProperties;                 // 0x90
};

struct cPlanetModel {
    char pad00[0x20];
    cWorldSphere* mpSphere;                     // 0x20
    cISphere*     mpISphere;                    // 0x24
    char pad28[0x2c - 0x28];
    Vec3          v2c;                          // 0x2c
    unsigned short* mpTextBegin;                // 0x38
    unsigned short* mpTextEnd;                  // 0x3c
    char pad40[0x48 - 0x40];
    char mFlag48;                               // 0x48
    char pad49[0x4c - 0x49];
    unsigned* mpContinentMap;                   // 0x4c
    char pad50[0x78 - 0x50];
    char mFlag78;                               // 0x78
    char pad79[0x7c - 0x79];
    void* mpBitmap;                             // 0x7c
    char pad80[0xa8 - 0x80];
    cLayerObject* mpLayer;                      // 0xa8

    bool FUN_00b84270();
    void FUN_00b83c40(const float* dir, float radius, uint32 flags, void* cb);
};

// ---- helpers (addresses from the retail image) ----
extern void* FUN_00f48aa0();                    // 0x00f48aa0
extern void  FUN_00f48af0(void*);               // 0x00f48af0
extern void  FUN_00b7e0b0(void*);               // 0x00b7e0b0
extern cQueried* FUN_00f48a70();                // 0x00f48a70
extern cMessageServer* FUN_0067dcc0();          // 0x67dcc0
extern cNotifier* FUN_0067dd50();               // 0x67dd50
extern cPluginHost* FUN_0067dd80();             // 0x67dd80
extern cOverlay* FUN_0067de00();                // 0x67de00
extern cTerrainVolume* FUN_00b3d520();          // 0x00b3d520
extern void* FUN_00b33f40();                    // 0x00b33f40
extern float* FUN_0041ea70();                   // 0x0041ea70
extern float FUN_00c9eb60();                    // 0x00c9eb60

extern cSimulator* g_pSimulator;                // 0x015fd918
extern cWorldHost* g_pWorld;                    // 0x0167ea54

extern Vec3        gTerrainCenter;              // 0x016881f0
extern const float kHeightBias;                 // 0x013f1cac
extern const float kVolumeParamA;               // 0x01488874
extern const float kVolumeParamB;               // 0x013ec4d0
extern const float kDefaultRadius;              // 0x013ebc68
extern const float kFloatMax;                   // 0x01465438
extern const float kNegFloatMax;                // 0x013f51ac

extern const char kMsgSetupB[];                 // 0x0182c582

// @ 0x00b84270
bool cPlanetModel::FUN_00b84270()
{
    Vec3* pCenter = &mpSphere->v18;
    float radius;
    float threshold;
    cTerrainVolume* volume;
    void** objects;
    cPropertyVariant* name;
    cPropertyVariant* value1;
    cPropertyVariant* value2;
    cPropertyVariant* value3;
    int count;

    if (mpISphere != FUN_00f48aa0())
        FUN_00f48af0(mpISphere);

    FUN_00b7e0b0(mpISphere);
    mpSphere->SetActive(1);

    if (!FUN_00f48a70()->IsSet())
        g_pWorld->Handle(mpSphere->p924);

    if (!mFlag48) {
        mFlag48 = 1;
        cMessageServer* messages = FUN_0067dcc0();
        messages->Subscribe(this, (const char*)0x01b537e5);
        messages = FUN_0067dcc0();
        messages->Subscribe(this, (const char*)0x0182c582);
        messages = FUN_0067dcc0();
        messages->Subscribe(this, (const char*)0x044448ed);
        messages = FUN_0067dcc0();
        messages->Subscribe(this, (const char*)0x045fe992);
    }

    cTerrainMapSet* pMapSet = mpISphere->GetMapSet();
    float heightFactor = pMapSet->f54;
    _ReadWriteBarrier();
    float height = pMapSet->f34 + pMapSet->f38 * heightFactor + kHeightBias;

    cBoundingBox box;
    box.vMin.x = gTerrainCenter.x - height;
    box.vMin.y = gTerrainCenter.y - height;
    box.vMin.z = gTerrainCenter.z - height;
    box.vMax.x = gTerrainCenter.x + height;
    box.vMax.y = gTerrainCenter.y + height;
    box.vMax.z = gTerrainCenter.z + height;

    volume = FUN_00b3d520();
    if (volume != 0)
        volume->AddRef();

    volume->Setup(&box, kVolumeParamA, kVolumeParamB);

    if (g_pSimulator->p3c->pF0 != 0) {
        cOverlay* overlay = FUN_0067de00();
        overlay->Draw(&box, kVolumeParamA, kVolumeParamB);
    }

    v2c = *pCenter;
    _ReadWriteBarrier();

    if (mpTextBegin != mpTextEnd) {
        *mpTextBegin = 0;
        mpTextEnd = mpTextBegin;
    }

    mFlag78 = 1;
    memset(mpBitmap, 0, 0x3000);
    unsigned* pCells = mpContinentMap;
    for (int i = 0; i < 0x18000; ++i)
        pCells[i] &= 0xC3FFFFFF;

    if (mpISphere != 0) {
        cLayerObject* layer = mpLayer;
        if (!layer->mFlag28) {
            cNotifier* notify = FUN_0067dd50();
            notify->Notify(layer, 9, 0);
            notify = FUN_0067dd50();
            notify->Notify(layer, 0x19, 0);
        }
        layer->mFlag28 = 1;

        mpISphere->Attach(FUN_00b3d520(), 5);

        mpISphere->Attach2(FUN_00b33f40(), 5);
        if (g_pSimulator->IsFeatureEnabled((const char*)0x05404855)) {
            cPluginHost* pluginHost = FUN_0067dd80();
            void* plugin = pluginHost->Create((const char*)0x03fbae24);
            mpISphere->Attach(plugin, 1);
        }

        count = mpISphere->CollectObjects(&objects);

        for (int i = 0; i < count; ++i) {
            cWorldObject* object = (cWorldObject*)objects[i];
            if (object == 0)
                continue;

            cPropertyBag* props = object->mpProperties;
            if (props != 0 &&
                props->GetProperty(0x03f6d22a, (void**)&name) &&
                name->mType == 1 && name->AsString()[0] != 0)
                continue;

            radius = kDefaultRadius;
            props = object->mpProperties;
            if (props != 0 &&
                props->GetProperty(0x0254cf89, (void**)&value1) &&
                value1->mType == 0xd)
                radius = *FUN_0041ea70();

            FUN_00b83c40((const float*)&object->v0c, radius, 0x10000000, 0);

            threshold = 0.0f;
            props = object->mpProperties;
            if (props != 0 &&
                props->GetProperty(0x0254cf97, (void**)&value2) &&
                value2->mType == 0xd)
                threshold = *FUN_0041ea70();

            if (threshold >= FUN_00c9eb60()) {
                props = object->mpProperties;
                if (props != 0 &&
                    props->GetProperty(0x0254cf8f, (void**)&value3) &&
                    value3->mType == 0xd)
                    radius = *FUN_0041ea70();

                FUN_00b83c40((const float*)&object->v0c, radius, 0x20000000, 0);
            }
        }
    }

    bool active = mpISphere != 0;
    volume->Release();
    return active;
}
