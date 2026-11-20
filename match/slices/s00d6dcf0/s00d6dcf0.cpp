// Slice s00d6dcf0 -- SP::PLAY_FIGHT_Activate and the task-activation function the
// linker placed immediately after it (both inside the 219-byte fingerprint range).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

// ---------------------------------------------------------------------------
// callees: referenced by address and called through the convention the retail
// call sites set up (ecx for a first argument = thiscall). Call targets are
// masked by the checker, so only the argument setup has to match.
// ---------------------------------------------------------------------------
typedef void* (__thiscall *ThisFn1)(void*);
typedef char  (__thiscall *ThisFn1b)(void*);
typedef void  (__thiscall *ThisFn5)(void*, void*, void*, void*, void*);
typedef void* (__stdcall  *StdFn3)(uint32_t, void*, void*);
typedef void* (__stdcall  *StdFn4)(uint32_t, int, const char*, const char*);

void FUN_00bc9b10();                    // 0x00bc9b10  entry lookup
void FUN_00c04590();                    // 0x00c04590  getter
void FUN_00bc9b00();                    // 0x00bc9b00  task registry (3 and 4 args)
void FUN_00bca620();                    // 0x00bca620
void FUN_00bcb0b0();                    // 0x00bcb0b0
void FUN_00bc9b50();                    // 0x00bc9b50  AtBegin
void FUN_00bcb480();                    // 0x00bcb480

extern float g_1486110;                 // 0x01486110

// ---------------------------------------------------------------------------
// layout (offsets from the retail instruction diff)
// ---------------------------------------------------------------------------
struct Creature
{
    char  _p000[0xb20];
    void* mpSpecies;                    // +0xb20
    char  _p0b24[0x28];
    void* mpTree;                       // +0xb4c
};

struct FightNode
{
    char  _p000[0x1a4];
    int   mf1a4;                        // +0x1a4
    float mf1a8;                        // +0x1a8
    void* mf1ac;                        // +0x1ac
    int   mf1b0;                        // +0x1b0
    int   mf1b4;                        // +0x1b4
};

// ===========================================================================
// @ 0x00d6dcf0  SP::PLAY_FIGHT_Activate
// ===========================================================================
bool __cdecl PLAY_FIGHT_Activate(Creature* self, Creature* other)
{
    Creature* entry = (Creature*)((ThisFn1) FUN_00bc9b10)(self);
    void* target = ((ThisFn1) FUN_00c04590)(other);
    void* entryTarget = ((ThisFn1) FUN_00c04590)(entry);

    bool sameSpecies = entry->mpSpecies == other->mpSpecies;
    bool sameTarget = entryTarget == target;
    return (sameSpecies && sameTarget) ? true : false;
}

// ===========================================================================
// task activation emitted immediately after PLAY_FIGHT_Activate (0x00d6dd30,
// part of the same fingerprint row): register the predicate, create the fight
// node on first use, otherwise make sure it is not already running, then start
// the task on the creature's queue.
// ===========================================================================
bool __cdecl PlayFight_ActivateTask(Creature* self)
{
    void* it = ((StdFn3) FUN_00bc9b00)(0x53031f1u, (void*) PLAY_FIGHT_Activate, self);
    FightNode* node = (FightNode*)((ThisFn1) FUN_00bca620)(it);

    if (node == 0)
    {
        void* task = ((StdFn4) FUN_00bc9b00)(0x53031f1u, 2, (const char*) 0x18eb45e,
                                             (const char*) 0x147bca0);
        node = (FightNode*)((ThisFn1) FUN_00bcb0b0)(task);
        node->mf1a4 = 0;
        node->mf1a8 = g_1486110;
        node->mf1ac = 0;
        node->mf1b0 = 0;
        node->mf1b4 = 0;
    }
    else
    {
        if (((ThisFn1b) FUN_00bc9b50)(node))
            return false;
        node->mf1ac = self;
    }

    ((ThisFn5) FUN_00bcb480)(self->mpTree, node, self, 0, 0);
    return true;
}
