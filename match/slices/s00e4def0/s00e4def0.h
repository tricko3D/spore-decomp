// Shared shapes for 0x00e4def0 (SPGonzago): the function and its /GL caller stub.
//
// Parameter order. Under the whole-program /GL image a callee's first three parameters
// arrive in edx, ecx and eax (in that order) and the fourth on the stack; retail's
// entry moves show ebx = the vector sums (from ecx), esi = the lengths (from eax) and
// edi = the weight sums (from edx), so the source lists the weight sums first, the
// vector sums second and the lengths third. The decorated name is unchanged: the
// parameter types are still (float*, SmoothVec*, float*, const SmoothSet*).
#ifndef S00E4DEF0_H
#define S00E4DEF0_H

struct SmoothVec {
    float x;
    float y;
    float z;
};

// One influence slot: a vector plus the weight it is divided by (32 bytes).
struct SmoothInfluence {
    float x;        // +0
    float y;        // +4
    float z;        // +8
    float weight;   // +12
    float pad[4];   // +16..+31
};

// Per-slot row of influence slots; the loop walks row i and takes its first `count` entries.
struct SmoothRow {
    SmoothInfluence influence[8];
};

struct SmoothSet {
    int count;                  // +0x000
    char pad0[0x4bc - 4];       // +0x004
    SmoothRow rows[300];        // +0x4bc
};

__declspec(noinline)
void SmoothSlotAverages(float* pWeight, SmoothVec* pSum, float* pLen, const SmoothSet* set);

#endif
