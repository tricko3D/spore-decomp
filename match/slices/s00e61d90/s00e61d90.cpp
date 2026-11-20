// Slice s00e61d90 -- SPGonzago/SPCellGame.obj (cell stage).
//
// 0x00e61d90 is a TU-internal effect-notification helper.  Retail calls it with
// the state object in EAX and its two remaining arguments as the two stack words
// (both call sites, 0x00e7908e and 0x00e79172 in the AI caterpillar update, do
// `push <index>; push <cell>; mov eax, <cell>; call`), i.e. cl's internal
// register convention for a static helper.  It walks two nested maps through
// 0x00e4cc40 with zero-initialised 4-byte lookup handles (0x00743b50, released by
// 0x00e82130) and, when entry `index` of the inner array is live, spawns a
// directed cell effect (0x00e61b90) with
//     EAX = the cell's transform block (self+0x4c)
//     EDX = the entry slot
//     stack: the flag `self->mCell35c != CellGame->mCurCell40fc`, then source+0x4c.
//
// source and self are the same object at both retail call sites: the helper is
// told where to put the effect (the source object's +0x4c block) and where the
// cell's own block is, and they coincide there.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// Why the two helpers below are `static`: cl only passes an argument of an
// internal-linkage function in a register.  With Notify static (and a same-TU
// caller that keeps it emitted) the state arrives in EAX exactly as in retail:
// `sub esp,8; push esi; push edi; lea ecx,[esp+0xc]; mov esi,eax`.
#include "s00e61d90.h"

// 0x00e61b90 -- spawn one directed cell effect.  Its real body is slice
// s00e61b90; this stand-in only has to give the call site retail's argument
// shape, so it is written the way retail's body uses its arguments: the entry
// slot is read first (EDX), the position block next (EAX), and the effect is
// placed towards the target unless the two blocks already coincide.  Keeping pos
// and entry in EAX/EDX also keeps the flag and the target on the stack.
static __declspec(noinline)
void SpawnDirectedCellEffect(void* pos, void** entry, int spaceEffects, void* target)
{
    void* id = entry[0];
    float* p = (float*)pos;
    float* t = (float*)target;
    KeyHandle h;
    FUN_00743b50(&h);
    void** known = (void**)FUN_00e4cc40(id, &h);
    if (p != 0) {
        float dx = t[0] - p[0], dy = t[1] - p[1], dz = t[2] - p[2];
        float d2 = dx * dx + dy * dy + dz * dz;
        if (d2 > 1.0f) {
            p[0] = t[0];
            p[1] = t[1];
            p[2] = t[2];
            FUN_00e4cc40(known, t);
        }
    }
    if (spaceEffects)
        entry[0] = (char*)id + 1;
    FUN_00e82130(&h);
}

// keeps the static stand-in emitted (its real body is in another slice)
void KeepSpawnDirectedCellEffect(void* pos, void** entry, int spaceEffects, void* target)
{
    SpawnDirectedCellEffect(pos, entry, spaceEffects, target);
}

// @ 0x00e61d90
static void Notify(CellNotifyState* self, char* source, int index)
{
    KeyHandle outerKey;
    FUN_00743b50(&outerKey);
    void** found = (void**)FUN_00e4cc40(self->mOuterMap108, &outerKey);

    KeyHandle innerKey;
    FUN_00743b50(&innerKey);
    void** entries = (void**)FUN_00e4cc40(*found, &innerKey);

    void** entry = entries + index;
    if (*entry != 0) {
        SpawnDirectedCellEffect(self->mBlock4c, entry,
                                self->mCell35c != gspCellGame->mCurCell40fc, source + 0x4c);
    }

    FUN_00e82130(&innerKey);
    FUN_00e82130(&outerKey);
}

// same-TU caller: it is what makes cl keep the internal register convention
// (state in EAX, `source`/`index` on the stack) and the plain `ret`.
void CellNotifyState_Notify(CellNotifyState* self, char* source, int index)
{
    Notify(self, source, index);
}
