// match/slices/s01013160/s01013160.cpp
// Object file: Simulator/SPGrobShock.obj (the "Simulator" EASTL allocator area,
// same object as s010137b0 / s01023fc0).
//
// Target: 01013160
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE2
//
// Recovered shape: range copy into caller storage of a 0x18-byte element.  The
// element starts with a 32-bit key and, at +4, an EASTL three-pointer vector of
// 32-bit values that is cloned with the "Simulator" allocator (allocator.h:209)
// and memcpy.  The last eight bytes of the element are not part of the copied
// value (they are left untouched by the original loop), so they are named only
// for their offsets.
//
// The original tolerates a null destination: each element is skipped when the
// out pointer is null, and the advanced out pointer is returned.

#include "game/types.h"

// ---------------------------------------------------------------------------
// Allocator / CRT entry points (declared only, out of line in the image).
// ---------------------------------------------------------------------------
void* __cdecl operator new(size_t size, const char* pName, int flags, int debugFlags,
                           const char* pFile, int line); // 0x00F473A0
extern "C" void* __cdecl memcpy(void* dst, const void* src, uint32_t n); // 0x011E0744

#define EASTL_SIM_FILE \
    "c:\\BuildAgent\\max-spore001-spore\\CMBuild\\SporeEP1_RL\\Core\\UTFKernel\\EASTL\\include\\EASTL/allocator.h"

// ---------------------------------------------------------------------------
// Element: key plus an eastl::vector<uint32_t> clone.
// ---------------------------------------------------------------------------
struct SimVec {
    uint32_t* mpBegin;  // +0x00
    uint32_t* mpEnd;    // +0x04
    uint32_t* mpCap;    // +0x08
};

struct SimEntry {
    uint32_t  mKey;       // +0x00  copied verbatim
    SimVec    mVec;       // +0x04  cloned payload
    uint32_t  mExtra0;    // +0x10  not part of the copied value
    uint32_t  mExtra1;    // +0x14
};

// @ 0x01013160
SimEntry* CopySimEntries(SimEntry* first, SimEntry* last, SimEntry* dest)
{
    for (; first != last; ++first, ++dest) {
        if (dest != 0) {
            dest->mKey = first->mKey;

            // eastl::vector<uint32_t> copy construction at +4.
            const int n = (int)(first->mVec.mpEnd - first->mVec.mpBegin);
            uint32_t* data = n ? (uint32_t*)operator new(n * (int)sizeof(uint32_t),
                                                         "Simulator", 0, 0,
                                                         EASTL_SIM_FILE, 0xd1)
                               : 0;

            dest->mVec.mpBegin = data;
            dest->mVec.mpEnd = data;
            dest->mVec.mpCap = data + n;

            // The pointer difference is a whole number of elements: the
            // original divides the byte count by 4 with an arithmetic shift.
            const int bytes =
                (int)((char*)first->mVec.mpEnd - (char*)first->mVec.mpBegin);
            dest->mVec.mpEnd = (uint32_t*)memcpy(data, first->mVec.mpBegin,
                                                 (uint32_t)bytes) +
                               (bytes >> 2);
        }
    }

    return dest;
}
