// @ 0x00cc86a0
// SP::cReservationGrid::RemoveReservedByID
//   void RemoveReservedByID(spstl::fixed_vector_map<unsigned int,
//                              SP::cReservationGrid::cReservedSpot, 100, 1,
//                              eastl::less<unsigned int> >& entries,
//                            unsigned int id);
// Rebuilds the reserved-spot map without the spot whose id matches.
//
// Flags: /O2 /Ob2 /MD /Gy /TP /arch:SSE /fp:fast /GS-

#include "types.h"

typedef unsigned int u32;
typedef unsigned char u8;

// One reserved spot, stored as the mapped value of the reserved-spot map.
struct cReservedSpot {
    u32 mX;        // +0x00
    u32 mY;        // +0x04
    u32 mZ;        // +0x08
    u32 mID;       // +0x0c
    u32 mFlags;    // +0x10  mbReserved / mbHasCitizen plus padding
};

// Sorted map entry: key plus the reserved spot.
struct ReservedPair {
    u32 mKey;             // +0x00
    cReservedSpot mSpot;  // +0x04
};

// DoAssign's false_type argument (an empty class, passed as a dummy stack
// dword) and the random-access iterator tag handed to DoAssignFromIterator.
struct FalseType { FalseType() {} };
struct RandomAccessTag { RandomAccessTag() {} };

// 0x00720250: eastl::do_copy(first, last, result) -- the element-wise copy helper.
void __cdecl do_copy(ReservedPair* first, ReservedPair* last, ReservedPair* result);

// spstl::fixed_vector_map<unsigned int, cReservedSpot, 100, 1, less<unsigned int> >
//   eastl::vector_map<unsigned int, cReservedSpot, eastl::less<unsigned int>,
//                     eastl::empty<void>,
//                     eastl::fixed_vector<eastl::pair<unsigned int, cReservedSpot>, 100, 1> >
struct ReservedMap {
    ReservedPair* mpBegin;         // +0x000
    ReservedPair* mpEnd;           // +0x004
    ReservedPair* mpCapacity;      // +0x008
    u32           mPad0C;          // +0x00c
    ReservedPair* mpInlineBuffer;  // +0x010
    u32           mPad14;          // +0x014
    ReservedPair  mInline[100];    // +0x018
    bool          mCompare;        // +0x978
    u8            mPad979[3];      // +0x979

    ReservedMap() {
        mpInlineBuffer = mInline;
        mpCapacity = mInline + 100;
        mpEnd = mInline;
        mpBegin = mInline;
    }

    ~ReservedMap() {
        if (mpBegin && mpBegin != mpInlineBuffer)
            ::operator delete(mpBegin);
    }

    // 0x00cc8040: lower_bound/insert and return the mapped value.
    cReservedSpot& operator[](const u32& key);

    // 0x00cc7d20: vector::DoAssignFromIterator(first, last, random_access_iterator_tag()).
    void DoAssignFromIterator(ReservedPair* first, ReservedPair* last, RandomAccessTag tag);

    // eastl::vector::erase(first, last): copy the tail down, then shrink.
    void erase(ReservedPair* first, ReservedPair* last) {
        do_copy(last, mpEnd, first);
        mpEnd -= last - first;
    }

    // eastl::vector::DoAssign(first, last, false_type) for pointer iterators.
    void DoAssign(ReservedPair* first, ReservedPair* last, const FalseType&) {
        DoAssignFromIterator(first, last, RandomAccessTag());
    }

    // eastl::vector::operator=: clear, then assign the source range.
    __forceinline ReservedMap& operator=(const ReservedMap& x) {
        if (this != &x) {
            erase(mpBegin, mpEnd);
            DoAssign(x.mpBegin, x.mpEnd, FalseType());
        }
        mCompare = x.mCompare;
        return *this;
    }
};

// SP::cReservationGrid (only the part this slice needs).
struct cReservationGrid {
    void RemoveReservedByID(ReservedMap& entries, u32 id);
};

// @ 0x00cc86a0
void cReservationGrid::RemoveReservedByID(ReservedMap& entries, u32 id)
{
    ReservedMap kept;
    bool removed = false;

    ReservedPair* last = entries.mpEnd;
    for (ReservedPair* it = entries.mpBegin; it != last; ++it) {
        u32 key = it->mKey;
        cReservedSpot spot = it->mSpot;
        if (spot.mID == id)
            removed = true;
        else
            kept[key] = spot;
    }

    if (removed)
        entries = kept;
}
