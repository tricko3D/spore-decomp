// @ 0x00d39000
#include "game/types.h"

namespace SP {
// Stand-ins for the real SPGonzago classes (src/game/SPGonzago/SPCreatureModeStrategy.cpp
// only names cCreatureModeStrategy::mpSink at +0x68; the sink's +0x58 and the layout class
// itself have no canonical header yet).  Only the members this accessor reads are modelled.
struct cSPUILayout {
    // The layout getter at 0x0093b6c0.  SPCreatureDisplayStrategy.cpp also records a
    // two-argument GetBuffer(unsigned id, int flag) at that address, but the retail tail call
    // here pushes nothing before the jmp, so this is the no-argument form (`this` in ecx).
    void* GetBuffer(); // 0x0093b6c0
};

struct cCreatureModeEventSink {
    unsigned char mUnknown00[0x58];
    cSPUILayout* mpLayout; // +0x58
};

struct cCreatureModeStrategy {
    unsigned char mUnknown00[0x68];
    cCreatureModeEventSink* mpSink;            // +0x68
    static cCreatureModeStrategy* spInstance;  // 0x0169e294
};
}

// The retail fingerprint at 0x00d39000 has size 110, but this accessor ends in an
// unconditional tail `jmp 0x93b6c0` at +0x0b.  Bytes +0x10..+0x6d are a complete separate
// function (push esi / push ebx frame, two `ret`s) whose entry the start list does not know
// (`fingerprint(0x00d39010)` is None; the next known start is 0x00d39070, so the 94 bytes end
// exactly there); it is registered as a callback (`RegisterThing((void*)0x00d39010)` at
// 0x00d39800, `B33CE0(0x00d39010)` at 0x00ef07d0), so it is not reachable from here.  No
// single compiled function can emit code after an unconditional jmp, so the 110-byte row
// cannot be matched until the fingerprint is regenerated with size 16 (a `data:` issue for a
// maintainer).  The 16 real bytes are byte-exact: a1 <g> 8b4068 8b4858 e9 <rel32>,
// relocations only at +1 and +12 (the masked offsets), and the masked 16-byte SHA-256 equals
// retail's.  The trailing 94 bytes cannot be pulled into this symbol either: /Gy puts every
// function in its own COMDAT section (appending a sibling function to this file leaves the
// extracted size at 16), and MSVC never emits code after an unconditional jmp.  The same holds
// with /Gy- (verified this pass): the two functions then share one .text section, 16-byte
// aligned, but fingerprint.extract() cuts a symbol at the next symbol in its section, so the
// accessor is still 16 bytes.  Only compiler-generated funclets (which start with '$') are
// skipped as cut points, and they cannot hold the retail 94-byte body.
void* FUN_00d39000()
{
    SP::cCreatureModeStrategy* strategy = SP::cCreatureModeStrategy::spInstance;
    return strategy->mpSink->mpLayout->GetBuffer();
}
