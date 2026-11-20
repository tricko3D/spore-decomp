// @ 0x00e4e230
// sCellNeighborList -- the occupied cells of the 20 x 15 fluid cell grid that
// surround `cell`.  The cell index maps back to column (index % 20) and row
// (index / 20); the two half extents are the current view rectangle scaled by
// the cell-size scale (one ceil'd cell count per axis), so a zoomed-out view
// widens the kernel.  Writes the grid index (column + 20 * row) of every cell
// that falls inside the grid into `cells` and returns how many were written
// (at most 9 for a 3 x 3 kernel).  Retail passes the cell in ecx, the scale in
// xmm3 and pushes the cells pointer, so the function is built /GL and linked
// with /LTCG (see ltcg/00e4e230.cpp).
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
#include "game/types.h"
#include "sse_helpers.h"

// Current view rectangle in world units.
extern float g_16b3c88;   // 0x016b3c88  left
extern float g_16b3c8c;   // 0x016b3c8c  top
extern float g_16b3c94;   // 0x016b3c94  right
extern float g_16b3c98;   // 0x016b3c98  bottom

// World-units-to-cells factors per axis and the half-cell bias that keeps a
// cell whose centre sits on the border from widening the kernel.
extern float g_1483ba8;   // 0x01483ba8
extern float g_1483bac;   // 0x01483bac
extern float g_1483bb4;   // 0x01483bb4

#define GRID_WIDTH  20
#define GRID_HEIGHT 15

__declspec(noinline) int sCellNeighborList(int cell, float scale, int* cells)
{
    float spanX = g_16b3c94 - g_16b3c88;
    float spanY = g_16b3c98 - g_16b3c8c;

    int row = cell / GRID_WIDTH;
    int col = cell % GRID_WIDTH;

    int n = 0;
    int radiusCol = SseCeilToInt((spanX * scale) * g_1483ba8 - g_1483bb4);
    int radiusRow = SseCeilToInt((spanY * scale) * g_1483bac - g_1483bb4);

    for (int c = col - radiusCol; c <= col + radiusCol; ++c) {
        int r = row - radiusRow;
        int rEnd = row + radiusRow;
        if (r <= rEnd) {
            int count = rEnd - r + 1;
            do {
                if ((unsigned)c <= (unsigned)(GRID_WIDTH - 1) &&
                    (unsigned)r <= (unsigned)(GRID_HEIGHT - 1))
                    cells[n++] = c + GRID_WIDTH * r;
                ++r;
            } while (--count != 0);
        }
    }
    return n;
}
