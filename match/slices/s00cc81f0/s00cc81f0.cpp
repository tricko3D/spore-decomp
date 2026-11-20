// Slice s00cc81f0 -- SP::cReservationGrid::GetRClusterByCoord.
//
// Collects every reservation-grid spot of the (2*face+1) x (2*face+1) square
// around the packed cube-map coordinate `rcoord` into `cluster`: each cell is
// wrapped onto the cube (SP_WrapCubeFace), packed back into a coordinate key
// and stored with the wrapped column/row/face.
//
// Flags: /O2 /Ob2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS-

#include "types.h"

typedef unsigned int u32;
typedef unsigned char u8;

// A cube-map cell: column, row and face.
struct cCubeMapIndex {
    int mCol;  // +0x00
    int mRow;  // +0x04
    int mFace; // +0x08
};

// A spot on the reservation grid: the cube-map cell, the id of the reserver
// (0 = none) and two flags.
struct cReservedSpot {
    cCubeMapIndex mCubeIndex; // +0x00
    u32 mID;                  // +0x0c  id of the reserver, 0 = none
    bool mbReserved;          // +0x10
    bool mbHasCitizen;        // +0x11
    u8 mPad12[2];             // +0x12  tail padding to 0x14

    cReservedSpot() {
        mID = 0;
        mbReserved = false;
        mbHasCitizen = false;
    }
};

// One (key, spot) entry of a fixed-capacity sorted map.
struct ReservedPair {
    u32 mKey;            // +0x00
    cReservedSpot mSpot; // +0x04
};

void CopyVertex3D(void* first, void* last, void* result);                                  // 0x00720250
int WrapCubeFace(int size, int* face, int* x, int* y, int rot, int flip);                  // 0x00684ca0

// Backing store of a `spstl::fixed_vector_map<unsigned int, cReservedSpot, 121, 1>`.
struct Map121 {
    ReservedPair* mpBegin; // +0x000
    ReservedPair* mpEnd;   // +0x004  one past the last live pair
    u32 mPad08[0x2da];     // +0x008  inline pair storage (121 * 0x14 bytes)
    bool mCompare;         // +0xb70
    u8 mPadTail[3];        // +0xb71

    cReservedSpot& Get(const u32& key); // 0x00cc80c0
    void clear();
};

// The reservation grid; only the cube-map resolution is needed here.
struct cReservationGrid {
    u32 mGridRes; // +0x000
    float mSquareSize; // +0x004 (unused here)

    void GetRClusterByCoord(u32 rcoord, Map121* cluster, u32 face); // 0x00cc81f0
};

// Drop every live entry.
void Map121::clear() {
    ReservedPair* first = mpBegin;
    ReservedPair* last = mpEnd;
    CopyVertex3D(last, mpEnd, first);
    mpEnd -= last - first;
}

// @ 0x00cc81f0
void cReservationGrid::GetRClusterByCoord(u32 rcoord, Map121* cluster, u32 face)
{
    if (face > 5)
        return;

    cluster->clear();

    int cx = (rcoord >> 16) & 0x1fff;
    int cy = rcoord & 0xffff;
    int cubeFace = rcoord >> 29;

    int xBegin = cx - (int)face;
    int xEnd = cx + (int)face + 1;
    int yBegin = cy - (int)face;
    int yEnd = cy + (int)face + 1;

    for (int i = xBegin; i < xEnd; ++i) {
        for (int j = yBegin; j < yEnd; ++j) {
            int wi = i;
            int wj = j;
            int wf = cubeFace;
            WrapCubeFace(mGridRes, &wf, &wi, &wj, 0, 0);

            u32 key = (((wf << 13) + (wi & 0x1fff)) << 16) + (wj & 0xffff);

            cReservedSpot spot;
            spot.mCubeIndex.mCol = wi;
            spot.mCubeIndex.mRow = wj;
            spot.mCubeIndex.mFace = wf;
            spot.mID = 0;
            cluster->Get(key) = spot;
        }
    }
}
