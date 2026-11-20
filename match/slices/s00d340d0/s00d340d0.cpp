// Slice s00d340d0 -- 0x00d340d0, the introsort driver used by the creature-mode input
// strategy for its ref-counted pick-object list.
//
// The entry point takes a half-open range of 4-byte SP::SortRef elements and a 0x1c-byte
// by-value comparator (a position + an orientation: 7 floats).  It runs SP::IntrosortLoop
// with the usual 2*floor(log2(n)) depth budget and then finishes the range with insertion
// sorts: the first 28 elements separately, the remainder (already introsorted) in one pass.
//
// 0x00d34fc0 is the little thunk that forwards here (see slice s00d34f90).
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

namespace SP {

struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(const Vector3& v) { x = v.x; y = v.y; z = v.z; }
};

struct Quaternion {
    float x, y, z, w;
    Quaternion() {}
    Quaternion(const Quaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; }
};

// 0x1c bytes (a position plus an orientation), passed by value all the way
// down the sort helpers.
struct SortCompare {
    Vector3    mPosition;
    Quaternion mOrientation;
};

struct SortRef {
    void* mpObject;
};

void __cdecl IntrosortLoop(SortRef* first, SortRef* last, int depth, SortCompare compare);  // 0x00d33ce0
void __cdecl InsertionSort(SortRef* first, SortRef* last, SortCompare compare);             // 0x00d2f630
void __cdecl InsertionSortRest(SortRef* first, SortRef* last, SortCompare compare);         // 0x00d2f710

static int Log2(int n)
{
    int bits = 0;
    while (n != 0) {
        n >>= 1;
        ++bits;
    }
    return bits;
}

// @ 0x00d340d0
void __cdecl Sort(SortRef* first, SortRef* last, SortCompare compare)
{
    if (first == last)
        return;

    IntrosortLoop(first, last, 2 * Log2((int)(last - first)) - 2, compare);

    if ((int)(last - first) > 28) {
        InsertionSort(first, first + 28, compare);
        InsertionSortRest(first + 28, last, compare);
    } else {
        InsertionSort(first, last, compare);
    }
}

}  // namespace SP
