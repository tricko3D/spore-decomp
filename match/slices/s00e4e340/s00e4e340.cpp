// @ 0x00e4e340
// Cell partition neighbourhood query used by the cell-game fluid solver.
// Retail takes its two pointer arguments in registers (position in eax, the
// partition in ebx), so the exported signature below is the honest C++ shape
// of the function; the register convention is recorded in the note.
#include "game/types.h"
#include "SP/cFluidPartition.h"

extern float g_15a7b90;  // 0x015a7b90

int SP_sPositionToCellIndex(float* p);      // 0x00e4dc90
int sCellNeighborList(int cell, float scale, int* cells);  // 0x00e4e230

// Fills out[] with the address of each occupied cell block around the cell that
// contains pos, and counts[] with that cell's entry count.  Returns the number
// of neighbours written (0 when pos is outside the partition).
int SP_sPartitionGetNeighbors(SP::cFluidPartition* part, float* pos,
                              int* out, int* counts)
{
    int cell = SP_sPositionToCellIndex(pos);
    if (cell == -1)
        return 0;

    int cells[9];
    int n = sCellNeighborList(cell, g_15a7b90, cells);
    if (n <= 0)
        return 0;

    for (int i = 0; i < n; ++i) {
        int id = cells[i];
        // numEntries[300] sits at +0x000, the per-cell blocks at +0x4b0 with a
        // 0x100-byte stride (8 entries of 0x20 bytes each).
        out[i] = (int)((char*)part + 0x4b0 + (id << 8));
        counts[i] = part->numEntries[id];
    }
    return n;
}
