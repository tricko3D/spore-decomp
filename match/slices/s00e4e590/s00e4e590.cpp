// Slice s00e4e590 -- SP cell/fluid Simulator helpers.
// Module flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast (same object as SPCellFluid)
#include "game/types.h"

// ---------------------------------------------------------------- globals
extern float g_cellX;          // 0x016b3c88  cell origin x
extern float g_cellY;          // 0x016b3c8c  cell origin y
extern float g_15a7b9c;        // cells per unit, x
extern float g_15a7ba0;        // cells per unit, y
extern float g_1483bb8;        // highest valid cell coordinate, y
extern float g_1483bbc;        // highest valid cell coordinate, x

// ---------------------------------------------------------------- callees
// 0x00e4dd20, clamp a world position to cell coordinates. It is TU-local in
// retail: the output buffer is the first register argument (EAX) and the input
// position the third (ECX); the middle parameter is never read.
__declspec(noinline) static void CellCoords(float* out, int, const float* in)
{
    out[0] = (in[0] - g_cellX) * g_15a7b9c;
    out[1] = (in[1] - g_cellY) * g_15a7ba0;
    if (out[0] <= 0.0f) out[0] = 0.0f;
    if (g_1483bbc <= out[0]) out[0] = g_1483bbc;
    if (out[1] <= 0.0f) out[1] = 0.0f;
    if (g_1483bb8 <= out[1]) out[1] = g_1483bb8;
}

// 0x00e4e170, split a cell coordinate into its floor index, the two corner
// weights and the direction of the neighbouring cell.
void __cdecl FUN_00e4e170(float v, int lo, int hi, int* idx, float* w, float* iw, int* dir);

// @ 0x00e4e590
float FUN_00e4e590(void* selfv, const float* pos)
{
    char* self = (char*)selfv;
    float c[2];
    CellCoords(c, 0, pos);

    float wx = 0.0f;
    int dx = 0;
    float wy = 0.0f;
    int dy = 0;
    float dummy;
    int x;

    FUN_00e4e170(c[0], 0, 0x14, &x, &wx, &dummy, &dx);
    int y;
    FUN_00e4e170(c[1], 0, 0x0f, &y, &wy, &dummy, &dy);

    const float* mag = (const float*)(self + 0x1d020);
    return mag[y * 20 + x + dx] + mag[y * 20 + x]
         + mag[(y + dy) * 20 + x] + mag[(y + dy) * 20 + x + dx];
}
