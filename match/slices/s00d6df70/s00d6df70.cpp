// Slice s00d6df70 -- creature "go to a random surface point" behaviour tick.
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast.  Callees are masked relocations.
#pragma warning(disable : 4100)
typedef unsigned int uint;

struct Vector3 {
    float x;                                                 // +0x00
    float y;                                                 // +0x04
    float z;                                                 // +0x08
};

// ---------------------------------------------------------------------------------------------
// externs
// ---------------------------------------------------------------------------------------------
struct RandomLinearCongruential {
    unsigned int RandomUint32Uniform(unsigned int n);        // 0x00a68fb0
};

// max() returning a reference (the shape MSVC compiles to a pointer select)
template <class T>
inline const T& MaxRef(const T& a, const T& b) { return a > b ? a : b; }

extern RandomLinearCongruential gRandom;                     // 0x01601760
extern const float gkMinRandomPointRadius;                   // 0x014853bc
extern const float gkRandomPointSpread;                      // 0x01486110
extern const float gkHalf;                                   // 0x01471064

// ---------------------------------------------------------------------------------------------
// spatial/locomotion subobject embedded at +0xc0 of the creature
// ---------------------------------------------------------------------------------------------
struct Spatial {
    virtual void u00(); virtual void u01(); virtual void u02(); virtual void u03();
    virtual void u04(); virtual void u05(); virtual void u06(); virtual void u07();
    virtual void u08(); virtual void u09(); virtual void u10();
    virtual Vector3* GetPosition();                          // vtable +0x2c
    virtual void u12(); virtual void u13(); virtual void u14(); virtual void u15();
    virtual void u16(); virtual void u17(); virtual void u18(); virtual void u19();
    virtual void u20(); virtual void u21(); virtual void u22(); virtual void u23();
    virtual void u24(); virtual void u25(); virtual void u26(); virtual void u27();
    virtual void u28();
    virtual float GetRadius();                               // vtable +0x74

    bool IsNearGoal();                                       // 0x00c42e20
};

// tuning block reached through Creature::Get()
struct CreatureTuning {
    char mPad000[0x108];
    float mField108;                                         // +0x108

    Vector3* GetPosition();                                  // 0x00c6acc0
};

struct Creature {
    char mPad000[0xc0];
    Spatial mSpatial;                                        // +0x0c0
    char mPad0c4[0x110 - 0xc4];
    uint mFlags110;                                          // +0x110
    char mPad114[0x137 - 0x114];
    char mState137;                                          // +0x137
    char mPad138[0xf90 - 0x138];
    char mFieldF90;                                          // +0x0f90

    bool IsAnimationFinished(int anim);                      // 0x00c123f0
    int  PlayAnimation(int guid, int loop, int blend);       // 0x00c12190
    void Hover(int on);                                      // 0x00c13a30
    CreatureTuning* Get();                                   // 0x00c04590
    void MoveToPointAtSpeed(int speedClass, const Vector3* pPoint, float radius,
                            float speed);                    // 0x00c1c1d0
};

// ---------------------------------------------------------------------------------------------
// planet helpers
// ---------------------------------------------------------------------------------------------
struct cPlanetModel {
    Vector3* MakeRandomWorldPosition(Vector3* pOut, const Vector3* pCenter,
                                     float minDist, float maxDist); // 0x00b81780
};

cPlanetModel* PlanetModel();                                 // 0x00b3d350
int __cdecl PickAggressiveEpicAnimation(Creature* self);      // 0x00d7a060
void __cdecl ProjectToSurface(const Vector3* pFrom, Vector3* pTo); // 0x00d99e20

// ---------------------------------------------------------------------------------------------
// @ 0x00d6df70
bool FUN_00d6df70(Creature* self, int arg2, int arg3, uint flags, int arg5, int* pState)
{
    switch (*pState) {
    case 0:
        if (!self->mSpatial.IsNearGoal())
            return true;
        self->Hover(0);
        if (self->mState137 == 0 && (self->mFlags110 & 0x1000) == 0)
            return true;
        if (self->mFieldF90 == 0 && (flags & 0x180000) == 0) {
            *pState = 1;
            return true;
        }
        {
            int roll = gRandom.RandomUint32Uniform(1000);
            if (roll < 450) {
                self->PlayAnimation(0x27844ad, 1, -1);
                *pState = 2;
                return true;
            }
            if (roll < 700) {
                self->PlayAnimation(0x3139a53, 1, -1);
                *pState = 2;
                return true;
            }
            int epic = PickAggressiveEpicAnimation(self);
            self->PlayAnimation(epic, 1, -1);
            *pState = 3;
            return true;
        }
    case 1:
        if (self->mFieldF90 == 0 && (flags & 0x180000) == 0)
            return true;
        *pState = 0;
        return true;
    case 2:
    case 3:
        if (!self->IsAnimationFinished(0))
            return true;
        *pState = 0;
        {
            CreatureTuning* pTuning = self->Get();
            // retail clamps the follow distance to at least the locomotion soft factor
            float radius = gkMinRandomPointRadius;
            radius = MaxRef(radius, pTuning->mField108);

            const Vector3* pOrigin;
            if ((flags & 0x8400) != 0)
                pOrigin = self->mSpatial.GetPosition();
            else
                pOrigin = pTuning->GetPosition();

            Vector3 dest;
            PlanetModel()->MakeRandomWorldPosition(&dest, pOrigin,
                                                   gkRandomPointSpread, radius);
            ProjectToSurface(self->mSpatial.GetPosition(), &dest);
            self->MoveToPointAtSpeed(2, &dest, self->mSpatial.GetRadius() * gkHalf,
                                     self->mSpatial.GetRadius());
            self->Hover(1);
        }
        return true;
    }
    return true;
}
