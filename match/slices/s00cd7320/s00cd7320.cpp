// SP::TRIBE_MATE_Tick -- place a newly born tribe mate at the birth position:
// face it towards the tribe, play its birth cry, record the birth event, show
// the tribe HUD hint and the birth timeline effect.
// Flags: /O2 /Ob1 /MD /Gy /TP /arch:SSE /GS- /fp:fast
#include "types.h"
#include <math.h>

// ------------------------------------------------------------------ math types
struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(float a, float b, float c) : x(a), y(b), z(c) {}
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
};

struct Quaternion {
    float x, y, z, w;
};

Quaternion* QuaternionFromFacingAndUp(Quaternion* out, const Vector3* facing,
                                      const Vector3* up);       // 0x0069b600

// ------------------------------------------------------------------ vtables
struct cCreature;

struct cTribeSub120 {
    virtual void s00(); virtual void s04(); virtual void s08(); virtual void s0c();
    virtual void s10(); virtual void s14(); virtual void s18(); virtual void s1c();
    virtual void s20(); virtual void s24(); virtual void s28();
    virtual int  GetTribeId(int a, int b, int c);                 // +0x2c
    uint8_t pad[0x110];                                           // size 0x114
};

struct cTribeVt0 {
    virtual void t00(); virtual void t04(); virtual void t08(); virtual void t0c();
    virtual void t10(); virtual void t14(); virtual void t18(); virtual void t1c();
    virtual void t20(); virtual void t24(); virtual void t28(); virtual void t2c();
    virtual void t30(); virtual void t34(); virtual void t38(); virtual void t3c();
    virtual void t40(); virtual void t44(); virtual void t48(); virtual void t4c();
    virtual void t50(); virtual void t54(); virtual void t58(); virtual void t5c();
    virtual void t60();
};
struct cTribeVtPos : cTribeVt0 {
    virtual Vector3* GetPosition();                               // +0x64
};
struct cTribeVtMate : cTribeVtPos {
    virtual void t68(); virtual void t6c(); virtual void t70(); virtual void t74();
    virtual void t78(); virtual void t7c(); virtual void t80(); virtual void t84();
    virtual void t88(); virtual void t8c(); virtual void t90(); virtual void t94();
    virtual cCreature* GetTribeMate(int index);                   // +0x98
};

struct cSpatialSub {
    virtual void p00(); virtual void p04(); virtual void p08(); virtual void p0c();
    virtual void p10(); virtual void p14(); virtual void p18(); virtual void p1c();
    virtual void p20(); virtual void p24(); virtual void p28(); virtual void p2c();
    virtual void p30(); virtual void p34();
    virtual void SetPosition(Vector3* position);                  // +0x38
    virtual void SetRotation(Quaternion* rotation);               // +0x3c
};

struct cCreature {
    uint32_t mVtbl;                                               // +0x00
    uint8_t  pad04[0xbc];                                         // +0x04..+0xbf
    cSpatialSub mSpatial;                                         // +0xc0

    uint32_t InterruptAnimation(uint32_t animID, int arg2, char arg3); // 0x00c12310
    uint32_t PlayVocal(uint32_t soundID, int mode, uint32_t id);       // 0x00c1d640
    void     UpdateTransform(Vector3* position, Quaternion* rotation); // 0x00c0fb80
    void*    GetSpeciesColor();                                   // 0x00c0bc00
};

struct cTribe : cTribeVtMate {
    uint8_t      pad04[0x11c];                                    // +0x04..+0x11f
    cTribeSub120 mSub120;                                         // +0x120
    uint32_t     mId234;                                          // +0x234
};

struct cGameNounManager {
    cTribe* GetPlayerTribe();                                     // 0x00bfc5f0
};

cGameNounManager* NounManager();                                  // 0x00b3d300

// ------------------------------------------------------------------ support
struct cTribeHud {
    uint8_t pad00[0x14];
    void*   mpLastTribe;                                          // +0x14
};
extern cTribeHud* gTribeHud;                                      // 0x0169b41c

struct cEventLog {
    void Post();                                                  // 0x00dd8640
};
cEventLog* __stdcall EventLog(uint32_t eventID, uint32_t category, int arg); // 0x00b3d3e0

struct cBirthRecord {
    void Reinit(uint32_t size);                                   // 0x007eb820
    uint8_t  pad00[0x0c];
    void*    mpInfo;                                              // +0x0c
};

void FUN_00e398e0(uint32_t id, void* p2, void* p3, void* p4,
                  uint32_t v1, uint32_t v2, uint32_t v3);         // 0x00e398e0
void SetGlobalProperty(int value, float scale);                   // 0x005ca880

class cTribeModeStrategy {
public:
    uint8_t pad000[0x3a3];
    bool    mb_3a3;                                               // +0x3a3

    void  TribeMateTick(Vector3 position);
    void* CreateEvent(int kind, Vector3 position);                // 0x00cd52e0
    void  SignalTribeEvent(uint32_t id);                          // 0x00cd6980
};

extern cTribeModeStrategy* gpTribeModeStrategy;                   // 0x0169b2c8

// @ 0x00cd7320
void cTribeModeStrategy::TribeMateTick(Vector3 position)
{
    uint32_t zero = 0;
    cTribe* tribe = NounManager()->GetPlayerTribe();

    cCreature* mate = tribe->GetTribeMate(zero);
    if (mate == 0)
        return;

    Vector3* tribePos = tribe->GetPosition();
    Vector3 direction;
    Vector3 delta;
    delta.x = tribePos->x - position.x;
    delta.y = tribePos->y - position.y;
    delta.z = tribePos->z - position.z;
    float s = 1.0f / sqrtf(delta.x * delta.x + delta.y * delta.y +
                           delta.z * delta.z + 1e-8f);
    direction.x = delta.x * s;
    direction.y = delta.y * s;
    direction.z = delta.z * s;

    Quaternion rotation;
    QuaternionFromFacingAndUp(&rotation, &direction, &position);

    mate->InterruptAnimation(0x640358f, -1, (char)zero);

    mate->mSpatial.SetPosition(&position);
    mate->mSpatial.SetRotation(&rotation);
    mate->UpdateTransform(&position, &rotation);

    mate->PlayVocal(0x1477574, 2, zero);

    cBirthRecord* record =
        (cBirthRecord*)gpTribeModeStrategy->CreateEvent(3, position);
    if (record != 0) {
        record->Reinit(8);
        *(uint32_t*)((uint8_t*)record->mpInfo + 0x1c) = 0;
    }

    if (gTribeHud != 0) {
        gTribeHud->mpLastTribe = mate;
        EventLog(0x6a602ea9, 0x182cd6ce, tribe->mSub120.GetTribeId(0, 1, 0))->Post();
    }

    gpTribeModeStrategy->SignalTribeEvent(0x11ade9ce);

    uint32_t tribeId = tribe->mId234;
    // Retail builds the "no key" colour at this point by storing the shared integer zero register
    // into the vector's 3 dwords (mov [esp+0x34], ebp x3); this is the spelling that makes cl 2008
    // keep 0 in ebp for the whole function (and therefore spill `this` to [esp+0x14]).
    *(int*)&direction.x = zero;
    *(int*)&direction.y = zero;
    *(int*)&direction.z = zero;
    FUN_00e398e0(0x782e0768, mate->GetSpeciesColor(), mate->GetSpeciesColor(),
                 &direction, tribeId, tribeId, zero);

    SetGlobalProperty(0x70d32097, 1.0f);

    mb_3a3 = true;
}
