// @ 0x0100a160
//
// Space-game queued-event update. Ages every entry of the simulator's event
// queue by the elapsed milliseconds, retires the entries whose referenced
// entity is gone / finished, re-queues entries while the entity is idle and
// dispatches the live ones through the entity state handlers.
//
//  0100a160  cSimulatorSpaceGame::UpdateEvents   (retail size 1243 bytes)
//
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (no /EHsc).
#include "types.h"
#include <new>

#define PVCAT2(a, b) a##b
#define PVCAT(a, b) PVCAT2(a, b)
#define PV virtual void PVCAT(pv_, __COUNTER__)();
#define PV4 PV PV PV PV
#define PV8 PV4 PV4
#define PV10 PV8 PV2
#define PV2 PV PV

struct cEntity;
struct cEntityId;
struct cSimulatorSpaceGame;

// A queued space-game event: 0x14 bytes, polymorphic payload at +0.
struct cEvent {
    PV2
    virtual void Release(int32_t param);    // vtable +0x08
    uint32_t mu04;
    uint32_t mu08;
    cEntityId* mp0C;    // the avatar the event refers to
    int32_t  mi10;      // remaining milliseconds
    // Only the avatar and the timer take part in the assignment.
    cEvent& operator=(const cEvent& rhs)
    {
        mp0C = rhs.mp0C;
        mi10 = rhs.mi10;
        return *this;
    }
};

// A queue of events (begin/end pair: the idle list at +0xd0, the event list
// at +0xdc).
struct cEventQueue {
    cEvent* mpBegin;
    cEvent* mpEnd;
    cEvent* Push(cEvent* elem);             // 0x01009ab0
};

// The avatar record an entity carries; its state is the dispatch state.
struct cEntityId {
    int32_t GetState();                     // 0x00bb9ae0
    void    SetState(int32_t state, int32_t unused);         // 0x00bb9b00
};

// An entry of an entity's list: the payload is a 4-byte-element array.
struct cEntityListEntry {
    void Cancel(int32_t flag, cEntityListEntry* keep);      // 0x00ff2d50
    uint8_t pad00[0x3c];
    uint8_t* mpBegin;                       // +0x3c
    uint8_t* mpEnd;                         // +0x40
};

struct cEntity {
    cEntityId* GetId();                     // 0x00b8de30
    int32_t  GetState();                    // 0x00b8dab0
    bool     IsFinished();                  // 0x00b8d970
    void*    GetOwner();                    // 0x00b8e090
    cEntityListEntry* GetListEntry(uint32_t index);          // 0x00b8dec0
    void     FlushLists();                  // 0x00b8e760
    void     SetFlag(uint32_t value);       // 0x00b8d920
    uint8_t pad000[0xd0];
    cEventQueue mIdleQueue;                 // +0xd0
    uint8_t pad0D8[4];
    cEventQueue mEvents;                    // +0xdc
    uint8_t pad0E4[0x78];
    uint8_t* mpListBegin;                   // +0x15c
    uint8_t* mpListEnd;                     // +0x160
};

// The star manager that owns the running simulators.
struct cStarManager {
    cEntity* FindEntity();                  // 0x00ba6dc0
    cEntity* GetActive();                   // 0x00ba6630
    void*    GetShip();                     // 0x00bb1340
};
cStarManager* __stdcall StarManager(void* avatar);          // 0x00b3d2a0
cStarManager* __stdcall StarManagerByIndex(int32_t index);  // 0x00b3d2a0

// The editor-side ship object the state-4 path drives.
struct cShip {
    uint8_t pad000[0x84];
    void* mp84;                             // +0x84
    void Activate(int32_t on);              // 0x00c309b0
    void SetTarget(void* target);           // 0x00c33690
    void Attach(cEntity* ent);              // 0x00c34e70
    void Track(cEntity* ent);               // 0x00c316c0
};

// The dispatch record written before an event is broadcast.
struct cEventDispatch {
    uint8_t pad00[0x0c];
    cEntity* mpEntity;                      // +0x0c
    cEntityId* mpAvatar;                    // +0x10
};
struct cEventProcessor {
    void Run();                             // 0x00dd8640
};
extern cEventDispatch* gpEventDispatch;     // 0x016e0d08

cEventProcessor* __stdcall BeginEvent(uint32_t a, uint32_t b, int32_t c,
                                      int32_t d, int32_t e, int32_t f);  // 0x00b3d3e0

// A reference-counted handle returned by the current context.
struct cRefCounted {
    virtual void Destroy();                 // vtable +0x00
    virtual void Release();                 // vtable +0x04
};
struct cContext {
    PV8 PV2 PV
    virtual void* Query(uint32_t keyA, uint32_t keyB, void** out);   // vtable +0x2c
};
struct cRefPtr {
    void* mp;
};
struct cRefObject {
    void* RefToObj();                       // 0x004df550
};

void*     GetCurrentContext();              // 0x0067de30
uint32_t* __stdcall ResolveId(uint32_t id, float scale);             // 0x01021090
void*     __stdcall GetValueHolder(void* owner, uint32_t* id);       // 0x00b3d2c0
struct cValueHolder {
    float ReadValue();                      // 0x00d06240
};

void  ResetQueued(cEntity* ent);            // 0x00c6feb0
void  ClearQueued(cEntity* ent);            // 0x00c6fea0
void  SetState(cEntity* ent, int32_t state);   // 0x00c706d0
void  NotifyScene(cEntity* ent, cEntity* active);   // 0x00c737a0
void  BindScene(cEntityId* avatar);         // 0x00c8b5e0
void  SetSceneFlag(cEntity* ent, int32_t flag);   // 0x00b96f40
void* __stdcall MakeRef(void* owner);       // 0x00401090
void  PutIdValue(cSimulatorSpaceGame* self, uint32_t id, int32_t* value);  // 0x00410370

struct cTimer {
    void Start(int32_t a, int32_t b);       // 0x00fe5430
};
struct cTimerHost {
    uint8_t pad00[0x70];
    cTimer* mp70;                           // +0x70
};
extern cTimerHost* gpTimerHost;             // 0x016dc0fc

// Move [source, last) down onto dest.
void MoveEvents(cEvent* dest, cEvent* last, cEvent* source);  // 0x01008820

class cSimulatorSpaceGame {
public:
    void UpdateEvents(int32_t elapsed, int32_t unused);
private:
    uint8_t pad000[0xd0];
    cEventQueue mIdleQueue;                 // +0xd0
    uint8_t pad0D8[4];
    cEventQueue mEvents;                    // +0xdc
};

void cSimulatorSpaceGame::UpdateEvents(int32_t elapsed, int32_t unused)
{
    cRefPtr held;
    cEvent* it = mEvents.mpBegin;
    if (it == mEvents.mpEnd)
        return;

    do {
        it->mi10 -= elapsed;
        if (it->mi10 <= 0) {

        cEntity* ent = StarManager(it->mp0C)->FindEntity();
        if (!ent) {
            cEvent* last = mEvents.mpEnd;
            if (it + 1 < last) {
                cEvent* dst = it;
                cEvent* src = it + 1;
                do {
                    *dst = *src;
                    ++dst;
                    ++src;
                } while (src != last);
            }
            mEvents.mpEnd -= 1;
            mEvents.mpEnd->Release(0);
            continue;
        }

        if (ent->GetId()->GetState() != ent->GetState()) {
            if (it + 1 < mEvents.mpEnd)
                MoveEvents(it + 1, mEvents.mpEnd, it);
            mEvents.mpEnd -= 1;
            mEvents.mpEnd->Release(0);
            continue;
        }
        // The live path falls through; keeping the erase body on the else
        // branch makes cl emit its own copy of the removal block.
        if (!ent->IsFinished()) {
        } else {
            if (it + 1 < mEvents.mpEnd)
                MoveEvents(it + 1, mEvents.mpEnd, it);
            mEvents.mpEnd -= 1;
            mEvents.mpEnd->Release(0);
            continue;
        }

        if (ent->mIdleQueue.mpBegin == ent->mIdleQueue.mpEnd) {
            it = mEvents.Push(it);
            continue;
        }

        // The entity is busy: look the event up in the current context.
        held.mp = 0;
        cContext* ctx = (cContext*)GetCurrentContext();
        if (held.mp) {
            cRefCounted* old = (cRefCounted*)held.mp;
            held.mp = 0;
            old->Release();
        }
        ctx->Query(0x249e8357, 0x2ae0c7e, &held.mp);

        switch (ent->GetState()) {
        case 1:
            ResetQueued(ent);
            ClearQueued(ent);
            SetState(ent, 2);
            ent->SetFlag(1);
            SetSceneFlag(ent, 2);
            PutIdValue(this, 0x897907de, &it->mi10);
            it->mi10 *= 1000;
            gpEventDispatch->mpEntity = ent;
            gpEventDispatch->mpAvatar = ent->GetId();
            BeginEvent(0xfe7023e5, 0x131a9f54, 0, 0, 1, 0)->Run();
            gpTimerHost->mp70->Start(0x14, 1);
            break;

        case 2:
            ResetQueued(ent);
            ClearQueued(ent);
            SetState(ent, 4);
            ent->SetFlag(1);
            SetSceneFlag(ent, 4);
            PutIdValue(this, 0xcf99dc04, &it->mi10);
            it->mi10 *= 1000;
            gpEventDispatch->mpEntity = ent;
            gpEventDispatch->mpAvatar = ent->GetId();
            BeginEvent(0x381a2313, 0x131a9f54, 0, 0, 1, 0)->Run();
            break;

        case 4: {
            NotifyScene(ent, StarManagerByIndex(1)->GetActive());
            SetState(ent, 5);
            BindScene(ent->GetId());

            cShip* ship = (cShip*)StarManager(ent->GetId())->GetShip();
            if (ship) {
                ship->Activate(1);
                ship->SetTarget(((cRefObject*)MakeRef(ent->GetOwner()))->RefToObj());
                ship->Attach(ent);
                ship->Track(ent);
                void* owner = ship->mp84;
                uint32_t* id = ResolveId(0x55165f5, 1.0f);
                ((cValueHolder*)GetValueHolder(owner, id))->ReadValue();
            }

            // The first populated list stays; every other populated list is
            // drained against it.
            cEntityListEntry* keep = 0;
            int32_t count = (int32_t)(ent->mpListEnd - ent->mpListBegin) >> 2;
            for (int32_t i = 0; i < count; ++i) {
                if (keep)
                    break;
                cEntityListEntry* entry = ent->GetListEntry(i);
                if ((uint32_t)(entry->mpEnd - entry->mpBegin) & 0xfffffffc)
                    keep = entry;
            }
            if (keep) {
                int32_t total = (int32_t)(ent->mpListEnd - ent->mpListBegin) >> 2;
                for (int32_t i = 0; i < total; ++i) {
                    cEntityListEntry* entry = ent->GetListEntry(i);
                    if (entry == keep)
                        continue;
                    if (((uint32_t)(entry->mpEnd - entry->mpBegin) & 0xfffffffc) == 0)
                        continue;
                    do {
                        entry->Cancel(0, keep);
                    } while ((uint32_t)(entry->mpEnd - entry->mpBegin) & 0xfffffffc);
                }
                ent->FlushLists();
            }

            gpEventDispatch->mpEntity = ent;
            gpEventDispatch->mpAvatar = ent->GetId();
            BeginEvent(0x3e2c6d13, 0x131a9f54, 0, 0, 1, 0)->Run();
            break;
        }

        default:
            break;
        }

        if (ent->GetState() == 5) {
            ent->GetId()->SetState(8, 0);
            if (it + 1 < mEvents.mpEnd)
                MoveEvents(it + 1, mEvents.mpEnd, it);
            mEvents.mpEnd -= 1;
            mEvents.mpEnd->Release(0);
        } else {
            ++it;
        }
        if (held.mp)
            ((cRefCounted*)held.mp)->Release();
        } else {
            ++it;
        }
    } while (it != mEvents.mpEnd);
}
