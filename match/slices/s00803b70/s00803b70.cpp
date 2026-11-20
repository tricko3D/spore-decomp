#include "game/types.h"
#include <intrin.h>

struct Event;
typedef void (*CbFn)(void*, Event*);

// Drag/drop callback descriptor. The second field owns a reference.
struct Cb {
    void* field0;
    void* ptr;
    CbFn fn;
    void* ctx;
    bool b10;
    bool b11;

    Cb& operator=(const Cb& other);
};

// @ 0x00803b70
Cb& Cb::operator=(const Cb& other) {
    field0 = other.field0;
    _ReadWriteBarrier();
    void* old = ptr;
    void* replacement = other.ptr;
    if (replacement != old) {
        if (replacement) {
            ((void (__thiscall*)(void*))(*(void***)replacement)[0])(replacement);
        }
        ptr = replacement;
        if (old) {
            ((void (__thiscall*)(void*))(*(void***)old)[1])(old);
        }
    }
    fn = other.fn;
    ctx = other.ctx;
    b10 = other.b10;
    b11 = other.b11;
    return *this;
}
