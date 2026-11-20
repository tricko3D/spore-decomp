// Destructor of a two-base class: primary base at +0x00, second base (own vtable)
// at +0x0d4.  The destructor prologue of the second base is passed around in edi.
//
// Destruction order (reverse declaration order):
//   +0x4cc  RefPtr        explicit detach in the body, then its own destructor
//   +0x4b8  CookieBlock
//   +0x490  CookieBlock
//   +0x278  SpatialVector (out-of-line destructor 0x00AD92D0)
//   +0x1f4  CookieBlock
//   +0x0f0  InlineBuffer
//   +0x0dc  CookieBlock
//   +0x0d4  second base destructor (0x00AC5430), then the primary base (0x00C8A420)
//
// The stand-in types below exist only to reproduce the member layout and the
// destructor spellings; the real headers are not available for this slice.
#include "types.h"

void FreeData(void* p);  // 0x00F47380

struct RefCounted {
    virtual void v00();
    virtual void Release();  // vtable +0x04
};

// Intrusive pointer.  Detach() clears the slot before releasing, so a reentrant
// Release cannot reach the object twice; the destructor is the plain release.
struct RefPtr {
    RefCounted* mpObject;
    void Detach() {
        RefCounted* const pObject = mpObject;
        if (pObject) {
            mpObject = 0;
            pObject->Release();
        }
    }
    ~RefPtr() {
        if (mpObject)
            mpObject->Release();
    }
};

// Block freed only when the allocation cookie in front of it is non-zero.
struct CookieBlock {
    void* mpBlock;
    ~CookieBlock() {
        if (mpBlock && ((int*)mpBlock)[-1] != 0)
            FreeData(mpBlock);
    }
};

// Buffer that frees its data unless it still points into its inline storage at +0x10.
struct InlineBuffer {  // 0x14 bytes
    void* mpData;      // +0x00
    uint32_t mSize;    // +0x04
    uint32_t mCapacity;// +0x08
    uint32_t mFlags;   // +0x0c
    void* mpInline;    // +0x10
    ~InlineBuffer() {
        if (mpData && mpData != mpInline)
            FreeData(mpData);
    }
};

struct SpatialVector {
    void* mpBegin;
    void* mpEnd;
    void* mpCapacity;
    ~SpatialVector();  // 0x00AD92D0
};

struct Base0 {  // +0x00
    virtual ~Base0();  // 0x00C8A420
    uint32_t pad[(0xd4 - 4) / 4];
};

struct Base1 {  // +0x0d4
    virtual ~Base1();  // 0x00AC5430
    uint32_t pad0;
};

struct CookiePtr : Base0, Base1 {
    CookieBlock mDC;                            // +0x0dc
    uint32_t padE0[(0xf0 - 0xe0) / 4];
    InlineBuffer mF0;                           // +0x0f0
    uint32_t pad104[(0x1f4 - 0x104) / 4];
    CookieBlock m1F4;                           // +0x1f4
    uint32_t pad1F8[(0x278 - 0x1f8) / 4];
    SpatialVector m278;                         // +0x278
    uint32_t pad284[(0x490 - 0x284) / 4];
    CookieBlock m490;                           // +0x490
    uint32_t pad494[(0x4b8 - 0x494) / 4];
    CookieBlock m4B8;                           // +0x4b8
    uint32_t pad4BC[(0x4cc - 0x4bc) / 4];
    RefPtr m4CC;                                // +0x4cc
    virtual ~CookiePtr();
};

// @ 0x00c432e0
CookiePtr::~CookiePtr() {
    m4CC.Detach();
}
