// SP::Audio::cAmbienceLayerManager (SPGonzago/SPAudioAmbienceLayerManager.obj)
//
// Rebuilds the manager's "audible" cell list for the current listener sphere:
// the cell view is cleared, then the 6 cube faces x mNumCells x mNumCells cell
// grid is walked, each cell centre is mapped onto the planet surface, and the
// cells - and the tags stored in those cells - whose surface point lies closer
// than mRadius to mOrigin are appended to the two views.
//
// Callees are declaration-only externals so cl emits the direct calls instead
// of inlining a stub: vector erase 0x009e0480, the two growth-insert helpers
// 0x00e9e4f0 / 0x00ea9440, PlanetModel 0x00b3d350 and the surface projection
// 0x00b815a0.
//
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast

#include "types.h"
#include <math.h>

struct Vector3 {
    float x;   // +0x0
    float y;   // +0x4
    float z;   // +0x8
};

struct cPlanetModel {
    // Keeps the surface point of the cube direction `dir` in `out`.
    Vector3* DirectionToSurfacePosition(Vector3* out, const float* dir);  // 0x00b815a0
};

cPlanetModel* PlanetModel();  // 0x00b3d350

// A tag placed in the world; only its position is needed here.
struct cAmbienceTag {
    void*   mpVtbl;   // +0x00
    void*   mpUnk4;   // +0x04
    Vector3 mPos;     // +0x08
};

// (cellA, cellB, face) of one audible cell.
struct CellIndex {
    int32_t a;     // +0x0
    int32_t b;     // +0x4
    int32_t face;  // +0x8
};

// eastl::vector<CellIndex>.
struct CellIndexVec {
    CellIndex* mpBegin;  // +0x00
    CellIndex* mpEnd;    // +0x04
    CellIndex* mpCap;    // +0x08

    void erase(CellIndex* first, CellIndex* last);                    // 0x009e0480
    void DoInsertValue(CellIndex* position, const CellIndex& value);  // 0x00e9e4f0

    void clear() { erase(mpBegin, mpEnd); }

    // The container stores the new element into a local, advances the end
    // pointer and then assigns through it (the assignment is guarded).
    void push_back(const CellIndex& value) {
        if (mpEnd < mpCap) {
            CellIndex* p = mpEnd;
            mpEnd = p + 1;
            if (p)
                *p = value;
        } else {
            DoInsertValue(mpEnd, value);
        }
    }
};

// eastl::vector<cAmbienceTag*>; the per-cell tag list at +0x2c is a grid of
// these (index (face * mNumCells + b) * mNumCells + a).
struct TagVec {
    cAmbienceTag** mpBegin;  // +0x00
    cAmbienceTag** mpEnd;    // +0x04
    cAmbienceTag** mpCap;    // +0x08

    void DoInsertValue(cAmbienceTag** position, cAmbienceTag* const& value);  // 0x00ea9440

    int32_t Count() const { return (int32_t)(mpEnd - mpBegin); }

    cAmbienceTag* operator[](int32_t i) const { return mpBegin[i]; }

    void push_back(cAmbienceTag* const& value) {
        if (mpEnd < mpCap) {
            cAmbienceTag** p = mpEnd;
            mpEnd = p + 1;
            if (p)
                *p = value;
        } else {
            DoInsertValue(mpEnd, value);
        }
    }
};

// Cube face -> permutation of the direction's components: the third entry of a
// row is the face's axis, the other two are the in-plane coordinates. The two
// faces of one axis share a row (face >> 1).
static const uint8_t sFaceAxes[3][4] = {
    {0, 1, 2, 0},
    {1, 2, 0, 0},
    {2, 0, 1, 0},
};

// Kept in double so the length stays on the x87 stack (as in the original).
static __forceinline double Distance(const Vector3& p, const Vector3& q) {
    double dx = (double)p.x - q.x;
    double dy = (double)p.y - q.y;
    double dz = (double)p.z - q.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

// ---------------------------------------------------------------------------
// SP::Audio::cAmbienceLayerManager.  Only the members this function touches
// are named; the layout matches the class reconstructed in the s00e9e290
// slice.
// ---------------------------------------------------------------------------
class cAmbienceLayerManager {
public:
    uint8_t pad00[0x28];        // +0x00..+0x27
    int32_t mNumCells;          // +0x28  cells per cube-face edge
    TagVec** mpCellGrid;        // +0x2c  [face][a][b] -> tag list
    uint8_t pad30[0x10];        // +0x30..+0x3f
    CellIndexVec mViewCells;    // +0x40
    uint8_t pad4c[8];           // +0x4c..+0x53
    TagVec mViewTags;           // +0x54
    uint8_t pad60[0x3c];        // +0x60..+0x9b
    Vector3 mOrigin;            // +0x9c  listener position
    float mRadius;              // +0xa8  listener radius

    void RebuildAudibleSet();
};

// @ 0x00e9ea40
void cAmbienceLayerManager::RebuildAudibleSet() {
    mViewCells.clear();

    if (!PlanetModel())
        return;

    for (int face = 0; face < 6; ++face) {
        for (int b = 0; b < mNumCells; ++b) {
            float ub = (float)b + 0.5f;
            int flip = face & 1;
            const uint8_t* pAxis = sFaceAxes[face >> 1];

            for (int a = 0; a < mNumCells; ++a) {
                float invCells = 1.0f / (float)mNumCells;
                float u = ub * invCells;
                float v = ((float)a + 0.5f) * invCells;
                float x = u * 2.0f - 1.0f;
                float y = v * 2.0f - 1.0f;

                // Length of (+/-y, x, +/-1): the sign alternates per face so
                // the two faces of an axis cover opposite hemispheres.
                float inv = (float)(1.0 / sqrt((double)x * x + (double)y * y + 1.0));
                float s = flip ? -inv : inv;

                CellIndex cell = { a, b, face };

                float dir[3];
                dir[pAxis[0]] = s * y;
                dir[pAxis[1]] = inv * x;
                dir[pAxis[2]] = s;

                Vector3 surface;
                PlanetModel()->DirectionToSurfacePosition(&surface, dir);

                if (Distance(surface, mOrigin) < mRadius) {
                    mViewCells.push_back(cell);

                    TagVec* pTags = mpCellGrid[(face * mNumCells + b) * mNumCells + a];
                    int32_t count = pTags->Count();
                    for (int32_t i = 0; i < count; ++i) {
                        cAmbienceTag* pTag = (*pTags)[i];
                        if (Distance(pTag->mPos, mOrigin) < mRadius)
                            mViewTags.push_back(pTag);
                    }
                }
            }
        }
    }
}
