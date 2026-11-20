// Slice s00eae610 -- SP::Audio::cPlayerCityMusic::RemoveNote.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"
#include <intrin.h>

// The audio system singleton (EA::Audio::GetSystemAT, 0x00a206f0) with the
// four vtable slots the property publish path uses.
struct AT {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34();
    virtual void Slot38(int id);              // +0x38  BeginMessage
    virtual void Slot3c(int id, float v);     // +0x3c  SetParamFloat
    virtual void Slot40(int id, int v);       // +0x40  SetParamInt
    virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54();
    virtual void Slot58();                    // +0x58  Send
};

AT* __cdecl GetSystemAT();                    // 0x00a206f0
extern "C" void* __cdecl FUN_11e0744_memcpy(void*, const void*, unsigned int); // 0x011e0744

// One published note property: id 0x3cdd1a9, int tag 0x34753a7, float 0x34753aa.
static __forceinline void NoteEventF(int which, float val)
{
    AT* at = GetSystemAT();
    if (at != 0) {
        at->Slot38(0x3cdd1a9);
        at->Slot40(0x34753a7, which);
        at->Slot3c(0x34753aa, val);
        at->Slot58();
    }
}

struct cPlayerCityMusic {
    char   pad0[0x128];
    float* mVec128B;          // +0x128
    float* mVec128E;          // +0x12c
    char   pad130[0x13c - 0x130];
    float* mVec13cB;          // +0x13c
    float* mVec13cE;          // +0x140
    char   pad144[0x150 - 0x144];
    float  mDownbeat;         // +0x150
    char   pad154[0x1cc - 0x154];
    unsigned* mNoteDegB;      // +0x1cc
    unsigned* mNoteDegE;      // +0x1d0
    char   pad1d4[0x1e0 - 0x1d4];
    float* mVec1e0B;          // +0x1e0
    float* mVec1e0E;          // +0x1e4
    void RemoveNote(unsigned idx);
};

// @ 0x00eae610
void cPlayerCityMusic::RemoveNote(unsigned idx)
{
    this->mDownbeat -= this->mVec1e0B[idx];

    unsigned* noteDeg = this->mNoteDegB + idx;
    _ReadWriteBarrier();
    unsigned* noteNext = noteDeg + 1;
    if (noteNext < this->mNoteDegE)
        FUN_11e0744_memcpy(noteDeg, noteNext,
                           (unsigned)((const char*)this->mNoteDegE - (const char*)noteNext));
    --this->mNoteDegE;
    _ReadWriteBarrier();

    float* vec13c = this->mVec13cB + idx;
    float* vec13cN = vec13c + 1;
    if (vec13cN < this->mVec13cE)
        FUN_11e0744_memcpy(vec13c, vec13cN,
                           (unsigned)((const char*)this->mVec13cE - (const char*)vec13cN));
    --this->mVec13cE;

    float* vec1e0 = this->mVec1e0B + idx;
    float* vec1e0N = vec1e0 + 1;
    if (vec1e0N < this->mVec1e0E)
        FUN_11e0744_memcpy(vec1e0, vec1e0N,
                           (unsigned)((const char*)this->mVec1e0E - (const char*)vec1e0N));
    --this->mVec1e0E;

    float* vec128 = this->mVec128B + idx;
    float* vec128N = vec128 + 1;
    if (vec128N < this->mVec128E)
        FUN_11e0744_memcpy(vec128, vec128N,
                           (unsigned)((const char*)this->mVec128E - (const char*)vec128N));
    --this->mVec128E;

    int count = (int)(this->mNoteDegE - this->mNoteDegB);
    for (; idx < (unsigned)count; ++idx) {
        NoteEventF(0xe7b72656, (float)idx);
        NoteEventF(0xfe31e5d9, (float)this->mNoteDegB[idx]);
        NoteEventF(0xc309bcc9, this->mVec1e0B[idx]);
    }
    unsigned total = (unsigned)(this->mNoteDegE - this->mNoteDegB);
    float totalF = (float)total;
    NoteEventF(0xead8248, totalF);
}
