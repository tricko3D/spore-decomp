// 0x00ff1330: range copy over 0x34-byte elements (eight floats at +0x00,
// five 32-bit words at +0x20). Retail copies member by member (x87 fld/fstp
// for the floats, integer moves for the words) and skips the store, but not
// the walk, when the destination pointer is null.
#include "types.h"

struct Element34 {
    float    f0, f1, f2, f3, f4, f5, f6, f7;   // +0x00
    uint32_t w0, w1, w2, w3, w4;               // +0x20
};

// @ 0x00ff1330
void CopyElements34(const Element34* first, const Element34* last, Element34* dst) {
    while (first != last) {
        if (dst) {
            dst->f0 = first->f0;
            dst->f1 = first->f1;
            dst->f2 = first->f2;
            dst->f3 = first->f3;
            dst->f4 = first->f4;
            dst->f5 = first->f5;
            dst->f6 = first->f6;
            dst->f7 = first->f7;
            dst->w0 = first->w0;
            dst->w1 = first->w1;
            dst->w2 = first->w2;
            dst->w3 = first->w3;
            dst->w4 = first->w4;
        }
        ++first;
        ++dst;
    }
}
