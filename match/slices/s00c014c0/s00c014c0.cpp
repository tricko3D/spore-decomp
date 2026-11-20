// s00c014c0: rebuild the lane object's obstacle cache (0x00c014c0).
// Module flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
#include "types.h"
#include <intrin.h>

struct Vec3 { float x, y, z; };

// 16-byte cache element: surface position + obstacle radius.
struct Elem16 { Vec3 pos; float w; };

struct PtrVec {
    void** mpBegin;
    void** mpEnd;
    void** mpCapacity;
};

void* __cdecl FUN_00705250(void* first, void* last, void* dst);   // 0x00705250

// The obstacle cache. Its two mutators are small enough that the game inlines
// them into their callers, so they are modelled here as inline members too.
struct Vec16 {
    Elem16* mpBegin;
    Elem16* mpEnd;
    Elem16* mpCapacity;

    void Resize(unsigned n);                 // 0x00c00e90
    void Grow(Elem16* pos, Elem16* val);     // 0x00c67440

    // erase(begin(), end()) of an empty range: copy the (empty) tail onto the
    // front, then rewind the end pointer onto the begin pointer.
    void EraseTo(Elem16* last) {
        Elem16* first = mpBegin;
        FUN_00705250(last, last, first);
        mpEnd -= last - first;
    }

    void PushBack() {
        if (mpEnd < mpCapacity) {
            ++mpEnd;
        } else {
            Elem16 tmp;
            Grow(mpEnd, &tmp);
        }
    }
};

struct cPlanetModel {
    Vec3 DirectionToSurfacePosition(const Vec3& dir) const;       // 0x00b815a0
};
cPlanetModel* PlanetModel();                                      // 0x00b3d350

// 8.0f, shared with the city-growth code.
extern const float kGrowStep;                                     // 0x013eeda0

// One entry of the lane object's obstacle list; only the two accessors this
// pass uses are modelled.
struct cLaneSource {
    void** mVt;                                                   // +0x00
    float* GetPosition() {
        typedef float* (__thiscall* Fn)(cLaneSource*);
        return ((Fn)mVt[0x2c / 4])(this);
    }
    float GetRadius() {
        typedef float (__thiscall* Fn)(cLaneSource*);
        return ((Fn)mVt[0x74 / 4])(this);
    }
};

// The lane owner: its obstacle cache at +0x3c and the "zone valid" byte at
// +0x6c (cleared here, recomputed by UpdateZone).
struct cLaneObject {
    void** mVt;                        // +0x00
    char pad04[0x3c - 0x04];
    Vec16 mObstacles;                  // +0x3c
    char pad48[0x6c - 0x48];
    unsigned char mZoneValid;          // +0x6c

    PtrVec* GetSources() {
        typedef PtrVec* (__thiscall* Fn)(cLaneObject*);
        return ((Fn)mVt[0x68 / 4])(this);
    }
    void FUN_00c014c0();
};

// @ 0x00c014c0
void cLaneObject::FUN_00c014c0()
{
    PtrVec* sources = GetSources();
    mZoneValid = 0;

    // The cache is reached through a reference for the erase and for the
    // element pointer, but through the object for the other cache calls: that
    // is the mix of vector- and this-relative addressing the game emits.
    Vec16& obs = mObstacles;
    Elem16* last = obs.mpEnd;
    obs.EraseTo(last);

    int count = (int)((char*)sources->mpEnd - (char*)sources->mpBegin) >> 2;
    mObstacles.Resize(count);
    PlanetModel();

    void** it = sources->mpBegin;
    void** end = sources->mpEnd;
    if (it != end) {
        float limit = kGrowStep;
        do {
            cLaneSource* source = (cLaneSource*)*it;
            mObstacles.PushBack();

            // The position store goes through a pointer computed before the
            // accessor calls (the game keeps it in a callee-saved register).
            // The radius store goes back through mpEnd: the barrier stops cl
            // from reusing that pointer across the virtual call, so the end
            // pointer is reloaded exactly where the game reloads it.
            Elem16* slot = mObstacles.mpEnd - 1;
            slot->pos =
                PlanetModel()->DirectionToSurfacePosition(*(const Vec3*)source->GetPosition());
            float radius = source->GetRadius();
            const float& span = radius > kGrowStep ? limit : radius;
            _ReadWriteBarrier();
            mObstacles.mpEnd[-1].w = span;
            ++it;
        } while (it != end);
    }
}
