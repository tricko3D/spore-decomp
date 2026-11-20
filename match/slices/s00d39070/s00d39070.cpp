// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// Sweep predicate registered on the strategy callback slot: when the query's
// owner is a creature animal whose game data noun is not a hit sphere, fire a
// sweep (ray) test from the query's start point along its end-minus-start
// direction against the creature's attached model.
#include "types.h"
#include <intrin.h>

struct Vector3 {
    float x;
    float y;
    float z;

    Vector3() {}
    Vector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    Vector3(const Vector3& o) : x(o.x), y(o.y), z(o.z) {}
    Vector3& operator=(const Vector3& o)
    {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }
};

// The query handed to the callback: the owning noun plus the segment.
struct SweepQuery {
    void*    mpObject;   // +0x00
    uint32_t mField04;   // +0x04
    Vector3  mStart;     // +0x08
    Vector3  mEnd;       // +0x14
};

// A creature body record: the attached model sits at +0x17c.
struct CreatureBody {
    uint8_t mUnused00[0x17c];
    void*   mpModel;     // +0x17c
};

typedef void*    (__thiscall* CastFn)(void*, unsigned int);
typedef unsigned (__thiscall* TypeFn)(void*);

Vector3 normalized_safe(const Vector3& v);                      // 0x00449c20 (sret)
bool SweepTestModel(const Vector3& start, const Vector3& dir, void* pModel, int* pBone,
                    int a, Vector3* pHitPos, int b, int c);      // 0x009b6450

// @ 0x00d39070
bool FUN_00d39070(SweepQuery* query)
{
    void* p = *(void**)((char*)query->mpObject + 0x64);
    if (p != 0) {
        void* noun = ((CastFn)(*(void***)p)[0x0c / 4])(p, 0x17f243b);
        if (noun != 0) {
            if (((TypeFn)(*(void***)noun)[0x20 / 4])(noun) == 0x18eb45e) {
                if (*(int*)((char*)query->mpObject + 0x60) != 0x2e72cae) {
                    CreatureBody* body = (CreatureBody*)*(void**)((char*)noun + 0xb54);
                    if (body != 0) {
                        if (body->mpModel != 0) {
                            Vector3 start = query->mStart;
                            int bone = 0;
                            Vector3 dir;
                            dir = Vector3(query->mEnd.x - query->mStart.x,
                                          query->mEnd.y - query->mStart.y,
                                          query->mEnd.z - query->mStart.z);
                            dir = normalized_safe(dir);
                            _ReadWriteBarrier();
                            void* model = body->mpModel;
                            return SweepTestModel(start, dir, model, &bone, 0, 0, 0, 0);
                        }
                    }
                }
            }
            if (((TypeFn)(*(void***)noun)[0x20 / 4])(noun) == 0x2e72cae) {
                void* hit = *(void**)((char*)noun + 0x24c);
                if (hit != 0 && ((TypeFn)(*(void***)hit)[0x20 / 4])(hit) == 0x18ebadc)
                    return false;
            }
        }
    }
    return true;
}
