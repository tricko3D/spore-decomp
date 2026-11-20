// @ 0x00e9e8f0
// Slice s00e9e8f0: eastl::sp_ref_vector-style insert(position, const T&) for a ref-counted
// element (three-pointer vector: begin/end/capacity, no allocator member).
//
// The in-place branch lets the value it is inserting move with the shifted tail (a reference into
// the vector itself is re-read one element further on), copy-constructs the last element into the
// spare slot, copy_backward-shifts [position, end - 1) into the end and assigns the value at
// position.  The full branch doubles the capacity (minimum 1), allocates with the eastl allocator,
// moves the head, copy-constructs the value in the new slot, moves the tail, frees the old buffer
// and rewrites begin/end/capacity.
#include "../../include/types.h"

// Ref-counted payload: vtable +0x8 is AddRef, +0xc is Release.
struct RefObject {
    virtual void v00();
    virtual void v01();
    virtual void AddRef();      // vtable +0x8
    virtual void Release();     // vtable +0xc
};

// The vector element: a ref-counted slot, copied/assigned by hand below.
struct AutoRef {
    RefObject* mpObject;        // +0x0
};

struct RefVector {
    AutoRef* mpBegin;           // +0x0
    AutoRef* mpEnd;             // +0x4
    AutoRef* mpCapacity;        // +0x8
    void insert(AutoRef* position, const AutoRef& value);
};

extern "C" void* EAAllocate(unsigned size, const char* name, int flags, int debugFlags,
                            const char* file, int line);                                  // 0x00f473a0
extern "C" void  EAFree(void* p);                                                         // 0x00f47380
extern "C" void* EASTL_memmove(void* dst, const void* src, unsigned n);                    // 0x011e0744
extern "C" AutoRef* copy_backward_ref(AutoRef* first, AutoRef* last, AutoRef* destEnd);    // 0x00662870

void RefVector::insert(AutoRef* position, const AutoRef& value)
{
    if (mpEnd != mpCapacity) {
        // The value may be an element of this vector: it moves right by one with the shift.
        const AutoRef* pValue = &value;
        if (pValue >= position && pValue < mpEnd)
            pValue = pValue + 1;

        // Copy-construct the last element into the spare slot at the end.
        if (mpEnd) {
            RefObject* const pLast = mpEnd[-1].mpObject;
            mpEnd->mpObject = pLast;
            if (pLast)
                pLast->AddRef();
        }

        // Shift [position, end - 1) one slot to the right.
        copy_backward_ref(position, mpEnd - 1, mpEnd);

        // Assign the value (AddRef new, Release old).
        RefObject* const pNewValue = pValue->mpObject;
        RefObject* const pOldValue = position->mpObject;
        if (pNewValue != pOldValue) {
            if (pNewValue)
                pNewValue->AddRef();
            position->mpObject = pNewValue;
            if (pOldValue)
                pOldValue->Release();
        }

        mpEnd += 1;
        return;
    }

    // No spare slot: reallocate with twice the capacity (at least one element).
    const unsigned n = (unsigned)(mpEnd - mpBegin);
    unsigned newCapacity;
    if (n > 0)
        newCapacity = n * 2;
    else
        newCapacity = 1;

    AutoRef* pNew;
    if (newCapacity)
        pNew = (AutoRef*)EAAllocate(newCapacity * 4, "EASTL", 0, 0, "EASTL/allocator.h", 0xd1);
    else
        pNew = 0;

    // Move the head: [begin, position).
    const int nHeadBytes = (int)((char*)position - (char*)mpBegin);
    AutoRef* pSlot = (AutoRef*)EASTL_memmove(pNew, mpBegin, (unsigned)nHeadBytes)
                   + (nHeadBytes >> 2);   // four bytes per slot

    // Copy-construct the value in its new slot.
    if (pSlot) {
        RefObject* const pNewValue = value.mpObject;
        pSlot->mpObject = pNewValue;
        if (pNewValue)
            pNewValue->AddRef();
    }

    // Move the tail: [position, end).
    AutoRef* const pTailDest = pSlot + 1;      // slot just past the new element
    const int nTailBytes = (int)((char*)mpEnd - (char*)position);
    AutoRef* pNewEnd = (AutoRef*)EASTL_memmove(pTailDest, position, (unsigned)nTailBytes)
                     + (nTailBytes >> 2);   // four bytes per slot
    if (mpBegin)
        EAFree(mpBegin);

    mpEnd = pNewEnd;
    mpBegin = pNew;
    mpCapacity = pNew + newCapacity;
}
