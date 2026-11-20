// slice s00ca44f0: heap sift-down (adjust_heap) over an array of ref-counted
// element handles.  Children are compared through the by-value predicate
// object (0x00c9f490), the smaller child is moved into the hole with the usual
// retain/store/release handle assignment, and the saved element is then handed
// to the promote/push helper (0x00ca0f60) together with the predicate's four
// reference floats, which travel as a by-value four-float block.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

// Ref-counted element: vtable slot 24 (+0x60) retains a reference and slot 25
// (+0x64) releases one.
struct cHeapElem
{
    virtual void vpad00(); virtual void vpad01(); virtual void vpad02(); virtual void vpad03();
    virtual void vpad04(); virtual void vpad05(); virtual void vpad06(); virtual void vpad07();
    virtual void vpad08(); virtual void vpad09(); virtual void vpad10(); virtual void vpad11();
    virtual void vpad12(); virtual void vpad13(); virtual void vpad14(); virtual void vpad15();
    virtual void vpad16(); virtual void vpad17(); virtual void vpad18(); virtual void vpad19();
    virtual void vpad20(); virtual void vpad21(); virtual void vpad22(); virtual void vpad23();
    virtual void AddRef();                             // +0x60
    virtual void Release();                            // +0x64
};

// Four-byte owning handle: copying retains, destroying releases.
struct ElemRef
{
    cHeapElem* mpElem;                                 // +0x00

    ElemRef(const ElemRef& other) : mpElem(other.mpElem)
    {
        if (mpElem)
            mpElem->AddRef();
    }
    ~ElemRef()
    {
        if (mpElem)
            mpElem->Release();
    }
};

// Comparator object, passed by value.  It holds the reference point the
// comparison is made against; all four of its floats are what the helper
// receives.
struct HeapPred;

// Reference block handed down to the helper.  Retail builds it in place in the
// outgoing argument area, one member at a time, so the class carries a
// user-declared constructor and copy constructor (an implicit copy constructor
// makes cl build a stack temporary and copy it afterwards).
struct Vec4f
{
    float mx, my, mz, mw;

    Vec4f(const HeapPred& pred);
    Vec4f(const Vec4f& other) : mx(other.mx), my(other.my), mz(other.mz), mw(other.mw) {}
};

struct HeapPred
{
    float mRefA;                                       // +0x00
    float mRefX;                                       // +0x04
    float mRefY;                                       // +0x08
    float mRefZ;                                       // +0x0c

    bool Less(cHeapElem* a, cHeapElem* b);             // 0x00c9f490
};

Vec4f::Vec4f(const HeapPred& pred)
    : mx(pred.mRefA), my(pred.mRefX), mz(pred.mRefY), mw(pred.mRefZ)
{
}

// 0x00ca0f60: push the saved element up from the hole towards the top.  Retail
// hands it the sort key as a plain four-byte value (no x87 round trip), so the
// slot is declared as the key's bit pattern.
void Helper(ElemRef* first, int keyBits, int position, ElemRef value, Vec4f ref);

// @ 0x00ca44f0
__declspec(noinline)
void AdjustHeap(ElemRef* first, float key, int count, int position, ElemRef value, HeapPred pred)
{
    int childPosition = (2 * position) + 2;
    if (childPosition < count) {
        do {
            if (pred.Less(first[childPosition].mpElem, first[childPosition - 1].mpElem))
                --childPosition;

            cHeapElem* old = first[position].mpElem;
            cHeapElem* child = first[childPosition].mpElem;
            if (child != old) {
                if (child)
                    child->AddRef();
                first[position].mpElem = child;
                if (old)
                    old->Release();
            }

            position = childPosition;
            childPosition = (2 * childPosition) + 2;
        } while (childPosition < count);
    }

    if (childPosition == count) {
        cHeapElem* old = first[position].mpElem;
        cHeapElem* child = first[childPosition - 1].mpElem;
        if (child != old) {
            if (child)
                child->AddRef();
            first[position].mpElem = child;
            if (old)
                old->Release();
        }
        position = childPosition - 1;
    }

    Helper(first, *(int*)&key, position, value, Vec4f(pred));
}
