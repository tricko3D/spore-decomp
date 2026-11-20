// Slice s00e75350 -- cell stage: scatter a small cluster of cells around a source cell.
// SP::sGenCluster (PDB name; dev 0x00a84160).  The routine walks a coarse time interval and
// for every step tries to place `count` cells at a random distance in [lo,hi] from an already
// placed cell (or from a random point of the supplied bounds for the first one), rejecting
// positions that collide, fall outside the bounds, or swallow too many probe samples for the
// player cell.  Newly created cells are tagged with a group id and appended to that group's
// id list.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (cell TU).
#include "types.h"
#include <math.h>

struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    Vector3(const Vector3& o) : x(o.x), y(o.y), z(o.z) {}
    Vector3& operator=(const Vector3& o) { x = o.x; y = o.y; z = o.z; return *this; }
};

// +0x00 min, +0x0c max (the retail body compares pos against both triples).
struct Bounds3 {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
};

// Cell object: only the placement fields and the group tag are touched here.
struct Cell {
    unsigned char pad00[0x4c];
    Vector3       mPosition;   // +0x4c
    float         mRadius;     // +0x58
    unsigned char pad5c[0x38c - 0x5c];
    int           mGroup;      // +0x38c
};

// Object pool holding cells (cSPObjectPoolT<cCell,...> at cGame + 0x1c) and
// the cluster groups (pool at cGame + 0xa8).
struct ObjectPool {
    void* Get(int id);         // 0x00b72210
    int   New();               // 0x00b72160
};

// Header of the object returned by the group pool at cGame + 0xa8.
struct Group {
    unsigned char pad00[4];
    int           mCount;      // +0x04
    int           mIds[1];     // +0x08
};

struct Game {
    unsigned char pad0000[0x1c];
    ObjectPool    mCells;      // +0x001c
    unsigned char pad001d[0xa8 - 0x1d];
    ObjectPool    mGroups;     // +0x00a8
    unsigned char pad00a9[0x40fc - 0xa9];
    int           mPlayerCell; // +0x40fc
    unsigned char pad4100[0x514c - 0x4100];
    float         mCellSize;   // +0x514c
};

extern Game* gspGame;          // 0x016b3c04

// EA::Random::RandomLinearCongruential at 0x01601760.
struct RandomLCG {
    double       RandomDoubleUniform();                 // 0x009360d0
    unsigned int RandomUint32Uniform(unsigned int range); // 0x00a68fb0
};
extern RandomLCG sMathRandom;  // 0x01601760

extern "C" Vector3* __cdecl RandomDirection3(Vector3* out);              // 0x00b7e560
extern "C" Vector3* __cdecl RandomPointInBounds(Vector3* scratch, const Bounds3* box); // 0x00df5f50
extern "C" bool __cdecl IsPositionBlocked(int cellId, Vector3* pos, float radius);      // 0x00e5d410
extern "C" int  __cdecl ProbeCellSpace(int cellId, Vector3* pos, float radius,
                                       float* samples, int maxSamples, int flags);     // 0x00e87260
extern "C" int  __cdecl CreateCell(int group, Vector3* pos, float elevation, int defId,
                                   int level, float sizeScale, float size, int radiusFlag,
                                   const void* orient, int filter);                    // 0x00e74a20
extern "C" void __cdecl OnCellCreated(int cellId);                                     // 0x00e86f70
extern "C" double __cdecl GetClusterSpread(int defId, int level);                      // 0x00e5d2b0

// @ 0x00e75350
void __cdecl sGenCluster(int sourceCell, const Bounds3* bounds, float minDistance,
                         float maxDistance, int count, float time, int defId,
                         unsigned char useLatestParent, int level,
                         unsigned char allowBlocked)
{
    if (!(time > 0.0f))
        return;

    int   ids[512];       // ids[0..created-1] are the cells placed so far
    float samples[512];   // probe buffer used by the 0x00e87260 pass
    int   created = 0;

    for (float t = time; t > 0.0f; t -= 1.0f) {
        // A random event only happens while the remaining slice is long enough.
        if (sMathRandom.RandomDoubleUniform() <= t) {
            int groupId = gspGame->mGroups.New();

            // Radial distance for the placed cells; maxDistance == 0 selects minDistance.
            // The original evaluates this in double and wraps out-of-range samples back
            // to minDistance (the two x87 compares in the retail body).
            float  hi = (maxDistance == 0.0f) ? minDistance : maxDistance;
            double lo = (double)minDistance;
            double d  = lo + (hi - lo) * sMathRandom.RandomDoubleUniform();
            double rad = (d < hi) ? ((lo < d) ? d : lo) : lo;
            float  radius = (float)rad;

            for (int remaining = count; remaining > 0; --remaining) {
                // Spread of one cluster, expressed in cell-size units.
                float spread = (float)(GetClusterSpread(defId, level) / gspGame->mCellSize);

                Vector3 dir;
                RandomDirection3(&dir);
                float inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + 1e-8f);
                Vector3 const unit(dir.x * inv, dir.y * inv, 0.0f * inv);
                dir = unit;
                Vector3 const dirGroup = unit;

                sMathRandom.RandomUint32Uniform(2);

                int attempt = 0;
                while (attempt < 10) {
                    ++attempt;
                    // A position is rejected unless one of the checks below clears the flag.
                    int reject = 1;
                    Vector3 pos;

                    if (created > 0) {
                        if (useLatestParent) {
                            Cell* parent = (Cell*)gspGame->mCells.Get(ids[created - 1]);
                            float r = parent->mRadius;
                            sMathRandom.RandomDoubleUniform();     // original draws and drops a sample here
                            float const ax = dirGroup.x * r + parent->mPosition.x;
                            float const ay = dirGroup.y * r + parent->mPosition.y;
                            float const az = dirGroup.z * r + parent->mPosition.z;
                            pos.x = ax + dirGroup.x * spread * 1.0001f;
                            pos.y = ay + dirGroup.y * spread * 1.0001f;
                            pos.z = az + dirGroup.z * spread * 1.0001f;
                        } else {
                            int parentId = ids[sMathRandom.RandomUint32Uniform(created)];
                            Cell* parent = (Cell*)gspGame->mCells.Get(parentId);
                            float r = parent->mRadius;
                            Vector3 parentPos = parent->mPosition;

                            Vector3 dir2;
                            RandomDirection3(&dir2);
                            float inv2 = 1.0f / sqrtf(dir2.x * dir2.x + dir2.y * dir2.y + 1e-8f);
                            Vector3 const unit2(dir2.x * inv2, dir2.y * inv2, 0.0f * inv2);
                            dir2 = unit2;

                            float const bx = dir2.x * r + parentPos.x;
                            float const by = dir2.y * r + parentPos.y;
                            float const bz = dir2.z * r + parentPos.z;
                            pos.x = bx + dir2.x * spread * 1.0001f;
                            pos.y = by + dir2.y * spread * 1.0001f;
                            pos.z = bz + dir2.z * spread * 1.0001f;
                        }
                    } else {
                        Vector3 scratch;
                        pos = *RandomPointInBounds(&scratch, bounds);
                        pos.z = 0.0f;
                    }

                    if (!IsPositionBlocked(sourceCell, &pos, spread) || allowBlocked) {
                        if (pos.x >= bounds->minX && pos.x <= bounds->maxX &&
                            pos.y >= bounds->minY && pos.y <= bounds->maxY &&
                            pos.z >= bounds->minZ && pos.z <= bounds->maxZ) {
                            if (sourceCell == gspGame->mPlayerCell)
                                reject = ProbeCellSpace(sourceCell, &pos, spread, samples, 512, 0);
                            else
                                reject = 0;
                        }
                    }

                    if (reject > 0)
                        continue;
                    if (attempt >= 10)
                        break;

                    int newId = CreateCell(sourceCell, &pos, radius, defId, level,
                                           1.0f, spread, 1, 0, 0);
                    ids[created++] = newId;

                    Group* group = (Group*)gspGame->mGroups.Get(groupId);
                    Cell*  cell  = (Cell*)gspGame->mCells.Get(newId);
                    cell->mGroup = groupId;
                    group->mIds[group->mCount++] = newId;

                    OnCellCreated(sourceCell);
                    break;
                }
            }
        }
    }
}
