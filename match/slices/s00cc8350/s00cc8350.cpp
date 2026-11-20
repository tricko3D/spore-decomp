// @ 0x00cc8350  SP::cReservationGrid::ReserveSpotsInside
//
// Flags: /O2 /Ob2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-
//
// Reserves every cell of the reservation grid whose surface position lies
// inside the sphere (pos, radius):
//   * the cell containing `pos` is reserved first (RCoordFromPos),
//   * then a square ring of cells around it is walked; only the rim of the
//     ring is sampled (the inside was handled by the previous, smaller ring),
//     each rim cell is wrapped back onto its cube face (WrapCubeFace), turned
//     into a cube direction (the kFaceAxis face-rotation table), projected
//     onto the planet surface and distance-tested against radius^2,
//   * the ring grows one cell in every direction while the previous ring
//     reserved at least one cell.
//
// A packed reservation coordinate holds x in bits 16..28, y in bits 0..15 and
// the cube face in bits 29..31; a reserved spot stores the wrapped cube-map
// cell {col, row, face}, the reserving id and two flags.
//
// Cross-checked against the sibling functions of the same TU:
//   0x00684ca0 WrapCubeFace(size, &face, &x, &y, rot, flip)
//   0x00b3d350 PlanetModel(&surface, dir)   (stdcall, two pointers)
//   0x00b815a0 cPlanetModel::DirectionToSurfacePosition()
//   0x00cc7080 cReservationGrid::RCoordFromPos
//   0x00cc8040 Map100::Get
//   0x01476a40 kFaceAxis, three axis indices per face (four bytes apart)

#include "types.h"
#include <math.h>

typedef unsigned int  u32;
typedef unsigned char u8;

struct cSPVector3 {
    float x, y, z;
};

namespace SP {

struct cCubeMapIndex {
    int mCol;   // +0x00
    int mRow;   // +0x04
    int mFace;  // +0x08
};

struct cReservedSpot {
    cCubeMapIndex mCubeIndex; // +0x00
    u32           mID;        // +0x0c  id of the reserver, 0 = none
    bool          mbReserved; // +0x10
    bool          mbHasCitizen; // +0x11
    u8            mPad12[2];  // +0x12

    cReservedSpot() {
        mID = 0;
        mbReserved = false;
        mbHasCitizen = false;
    }
};

struct ReservedPair {
    u32           mKey;   // +0x00
    cReservedSpot mSpot;  // +0x04
};

// fixed_vector_map<unsigned, cReservedSpot, 100, 1, less<unsigned> >
struct Map100 {
    ReservedPair* mpBegin;  // +0x000
    ReservedPair* mpEnd;    // +0x004
    u32           mPad08[0x25c];
    bool          mCompare; // +0x978

    cReservedSpot& Get(const u32& key);  // 0x00cc8040
};

class cPlanetModel {
 public:
    void DirectionToSurfacePosition() const;  // 0x00b815a0
};

}  // namespace SP

int WrapCubeFace(int size, int* face, int* x, int* y, int rot, int flip);  // 0x00684ca0
SP::cPlanetModel* SP_MakePlanetModel(cSPVector3* surface, const float* dir); // 0x00b3d350
extern const u8 kFaceAxis[];                                              // 0x01476a40

namespace SP {

class cReservationGrid {
 public:
    int   mGridRes;         // +0x000
    float mSquareSize;      // +0x004
    Map100 mReservedSpots;  // +0x008
    Map100 mKeepClearSpots; // +0x984
    float mGridMax;         // +0x1300
    float mGridMin;         // +0x1304
    float mGridAvg;         // +0x1308

    u32  RCoordFromPos(const cSPVector3* pos);                             // 0x00cc7080
    void ReserveSpotsInside(Map100* spots, const cSPVector3* pos,
                            float radius, u32 id);                         // 0x00cc8350
};

// @ 0x00cc8350
void cReservationGrid::ReserveSpotsInside(Map100* spots, const cSPVector3* pos,
                                          float radius, u32 id) {
    const u32 rcoord = RCoordFromPos(pos);
    const u32 face   = rcoord >> 29;
    const int cy     = (int)(rcoord & 0xffff);
    const int cx     = (int)((rcoord >> 16) & 0x1fff);

    // The cell holding the position itself is always reserved.
    {
        cReservedSpot spot;
        spot.mCubeIndex.mCol  = cx;
        spot.mCubeIndex.mRow  = cy;
        spot.mCubeIndex.mFace = (int)face;
        spot.mID = id;
        spots->Get(rcoord) = spot;
    }

    const float radius2 = radius * radius;

    const int delta = cy - cx;
    int  lo   = cx - 1;
    int  hi   = cx + 2;
    bool grew = true;
    while (grew) {
        grew = false;
        const int yLo = delta + lo;
        const int yHi = delta + hi;
        for (int i = lo; i < hi; ++i) {
            for (int j = yLo; j < yHi; ++j) {
                // Only the rim of the ring is sampled.
                if (i != lo && i != hi - 1 && j != yLo && j != yHi - 1)
                    continue;

                // Wrap the rim cell back onto its cube face: the helper fills
                // the packed face word plus the two face coordinates.
                int wi = i;
                int wj = j;
                int wf = (int)face;
                WrapCubeFace(mGridRes, &wf, &wi, &wj, 0, 0);

                // Face coordinates -> (-1, 1) on the cube face.
                const float inv = 1.0f / (float)mGridRes;
                const float f1  = ((float)wj + 0.5f) * inv * 2.0f - 1.0f;
                const float f2  = ((float)wi + 0.5f) * inv * 2.0f - 1.0f;
                // 1 / sqrt(1 + u^2 + v^2); the low bit of the face word picks
                // the mirrored half of the face.
                const float s    = 1.0f / (float)sqrt((double)(f2 * f2 + f1 * f1 + 1.0f));
                const float negs = (wf & 1) ? -s : s;

                // Rotate the two face coordinates onto the cube axes.
                const u8* axis = kFaceAxis + ((wf >> 1) * 4);
                float     dir[3];
                dir[axis[0]] = negs * f2;
                dir[axis[1]] = s * f1;
                dir[axis[2]] = negs;

                cSPVector3 surface;
                cPlanetModel* model = SP_MakePlanetModel(&surface, dir);
                model->DirectionToSurfacePosition();

                const float dx = pos->x - surface.x;
                const float dy = pos->y - surface.y;
                const float dz = pos->z - surface.z;
                if (radius2 > dx * dx + dy * dy + dz * dz) {
                    // Pack the wrapped cell the way RCoordFromPos does.
                    const u32 key = (((u32)wf << 13) + ((u32)wi & 0x1fff)) << 16;
                    const u32 rkey = key + ((u32)wj & 0xffff);
                    cReservedSpot spot;
                    spot.mCubeIndex.mCol  = wi;
                    spot.mCubeIndex.mRow  = wj;
                    spot.mCubeIndex.mFace = wf;
                    spot.mID = id;
                    spots->Get(rkey) = spot;
                    grew = true;
                }
            }
        }
        ++hi;
        --lo;
    }
}

}  // namespace SP
