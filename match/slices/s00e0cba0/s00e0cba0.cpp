// Minimap cube-face band builder.
//
// For every output byte this walks one band of the 6-face (cube) image of the
// active terrain grid: it builds the direction (sin v, cos v * sin u,
// cos v * cos u) from two sine/cosine tables (tables at 0x15a4cc0/0x15a4cc4,
// the quarter-shifted halves at +0x100 bytes), uses the dominant axis as the
// cube face, projects the other two onto that face, and copies the high byte
// of the 16-bit grid sample.
//
// The module flags are the ones the rest of SPGonzago/_at_00e00000 uses;
// /fp:fast is what makes the lookup-table fill an inline x87 fsin.
// Flags: /O2 /Ob2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast /GS- /Isrc/game/include

#include "game/types.h"
#include "sse_helpers.h"
#include <math.h>

#pragma intrinsic(sin)

void* SP_GetActiveTerrainSphere(); // 0x00f48aa0

namespace {

// Grid reached through the active terrain sphere.
struct TerrainGrid {
    void*          mpVt;    // +0x00
    uint8_t        mPad04[4];
    unsigned       mDim;    // +0x08  face size, also the row stride
    uint8_t        mPad0c[4];
    const uint8_t* mpData;  // +0x10  16-bit samples, face major
};

uint32_t sFlags;       // 0x16a38b4  one-shot table-pointer initialisation bits
float*   spRowTable;   // 0x16a38b0  (0x15a4cc4 + 0x100 bytes)
float*   spColTable;   // 0x16a38ac  (0x15a4cc0 + 0x100 bytes)
int      sSinLutReady; // 0x16a38a8
float    sSinLut[769]; // 0x16a2ca4

} // namespace

// @ 0x00e0cba0
void __cdecl MinimapCubeBand(void* unused, uint8_t* out, int yStart, int yCount, const float* angles)
{
    uint32_t flags = sFlags;
    if ((flags & 1) == 0) {
        float* rows = *(float**)0x15a4cc4;
        flags |= 1;
        sFlags = flags;
        spRowTable = rows + 0x40;
    }
    if ((flags & 2) == 0) {
        float* cols = *(float**)0x15a4cc0;
        flags |= 2;
        sFlags = flags;
        spColTable = cols + 0x40;
    }
    if (sSinLutReady == 0) {
        float a = 0.0f;
        const float step = *(float*)0x147fab8;
        sSinLutReady = 1;
        unsigned i = 0;
        do {
            sSinLut[i + 1] = (float)sin((double)a);
            a += step;
            i++;
        } while (i < 0x300);
    }

    // Angle of the band, folded into [0, pi] and scaled to a table index.
    float v = (float)acos((double)angles[1]);
    const float pi = *(float*)0x16a2024;
    if (angles[0] > *(float*)0x1485378)
        v = pi - v;

    float angle = v / pi;
    int first = (int)(angle * *(float*)0x13f1160);
    int last = first + 0x100;

    void* sphere = SP_GetActiveTerrainSphere();
    void* desc = ((void* (__thiscall*)(void*))((void**)*(void**)sphere)[3])(sphere);
    TerrainGrid* grid = *(TerrainGrid**)((char*)desc + 8);
    const uint8_t* data = grid->mpData;
    unsigned dim = grid->mDim;
    uint8_t* dst = out;

    int top = 0x7f - yStart;
    int bottom = top - yCount;
    if (top <= bottom)
        return;

    for (int row = top; row > bottom; row--) {
        float av = (*(float**)0x15a4cc4)[row];
        float cv = spRowTable[row];
        for (int i = first; i < last; i++) {
            float bv = spColTable[i];
            float dv = (*(float**)0x15a4cc0)[i];
            float fc = (float)fabs((double)cv);
            float radius = (float)dim;
            double dab = (double)av * (double)bv;
            float ab = (float)dab;
            double dad = (double)av * (double)dv;
            float nad = (float)-dad;
            float fad = (float)fabs(-dad);
            float fab = (float)fabs(dab);

            float fu;
            float fw;
            int face;
            if (fc >= fad && fc >= fab) {
                fu = (nad / cv + 1.0f) * 0.5f;
                fw = (ab / fc + 1.0f) * 0.5f;
                face = (cv >= 0.0f) ? 0 : 1;
            } else if (fab >= fad) {
                fu = (cv / ab + 1.0f) * 0.5f;
                fw = (nad / fab + 1.0f) * 0.5f;
                face = (ab >= 0.0f) ? 4 : 5;
            } else {
                fu = (ab / nad + 1.0f) * 0.5f;
                fw = (cv / fad + 1.0f) * 0.5f;
                face = (nad >= 0.0f) ? 2 : 3;
            }

            float fx = radius * fu;
            float fy = radius * fw;
            int ix = SseTruncToInt(fx);
            int iy = SseTruncToInt(fy);
            *dst++ = data[((face * (int)dim + iy) * (int)dim + ix) * 2 + 1];
        }
    }
}
