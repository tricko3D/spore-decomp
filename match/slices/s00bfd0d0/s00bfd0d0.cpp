// Combatant::FullRepair - restore health to the maximum.  When the combatant is
// hurt (and the call is not forced) it first notifies the owner through the
// hostile-target path, then clears the damage state and broadcasts the
// damage-state message if the state actually changed.
#include "game/types.h"

extern "C" long __cdecl _InterlockedExchange(long volatile*, long);
#pragma intrinsic(_InterlockedExchange)

struct Combatant {
    void**   vftable;             // +0x00
    uint32_t pad04;               // +0x04
    uint32_t pad08[0x0b];         // +0x08..+0x33
    uint32_t mField34;            // +0x34 damage state
    float    mHealth38;           // +0x38
};

struct Owner {                    // the object returned by vtable +0x0c
    void**   vftable;             // +0x00
};

// MessageBasicRC<5>: derived vtable 0x013eb844, base (MessageRC) vtable 0x013eb90c
struct Msg {
    void*    vftable;             // +0x00
    long     mRefCount;           // +0x04
    void*    mSubject;            // +0x08 payload (the combatant)
    uint32_t mField0c;            // +0x0c
    uint32_t mField10;            // +0x10
    uint32_t mPad14[7];           // +0x14..+0x2f
    uint32_t mId;                 // +0x30
    uint32_t mPad34;              // +0x34
    uint32_t mRCFlags;            // +0x38
    uint32_t mPad3c;              // +0x3c

    Msg() : mId(0)
    {
        vftable = (void*)0x13eb90c;            // MessageRC sub-object
        _InterlockedExchange(&mRefCount, 0);
        vftable = (void*)0x13eb844;            // MessageBasicRC<5>
        mRCFlags = 0;
    }
};

struct RcMsg {                                  // MessageServer
    virtual void _v0();
    virtual void _v1();
    virtual void _v2();
    virtual void _v3();
    virtual void _v4();
    virtual void Send(unsigned id, void* msg, unsigned flags);   // vtable +0x14
};

void*  MessageServer();                                          // 0x0067DCC0
char   FUN_00b18740(void* o);                                    // 0x00B18740
void   FUN_00bfca80(void* o, void* owner, int extra);            // 0x00BFCA80
void   SlotMessage_Destruct(void* m);                            // 0x00421CF0

static __forceinline void* GetOwner(Combatant* self)
{
    return ((void* (__thiscall*)(Combatant*))self->vftable[3])(self);          // vtable +0x0c
}

// The owner's current vehicle/trike (vtable +0x4c); kept inside the caller's
// argument list so the zero argument is pushed before the lookup runs.
static __forceinline void* GetTrike(Combatant* self)
{
    Owner* o = (Owner*)GetOwner(self);
    return ((void* (__thiscall*)(Owner*))o->vftable[0x13])(o);                 // vtable +0x4c
}

// @ 0x00bfd0d0
void __fastcall Combatant_FullRepair(Combatant* self, int, char force)
{
    if (((float (__thiscall*)(Combatant*))self->vftable[0x16])(self) > self->mHealth38) {   // vtable +0x58
        if (!force) {
            void* owner = GetOwner(self);
            if (!FUN_00b18740(owner)) {
                FUN_00bfca80(GetOwner(self), GetTrike(self), 0);
            }
        }
    }

    uint32_t old = self->mField34;
    float (__thiscall* maxHealthFn)(Combatant*) = (float (__thiscall*)(Combatant*))self->vftable[0x16];   // vtable +0x58
    self->mField34 = 0;
    self->mHealth38 = maxHealthFn(self);

    if (old != self->mField34) {
        Msg msg;
        msg.mSubject = self;
        msg.mField10 = 0;
        ((RcMsg*)MessageServer())->Send(0x1622184, &msg, 0);
        ((void (__thiscall*)(void*))SlotMessage_Destruct)(&msg);
    }
}
