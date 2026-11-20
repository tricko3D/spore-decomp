// cTribeModeStrategy::cd71b0 -- switch the tribe tool mode: optionally stop the previous tool's
// Seti cue, refresh the tool UI, then make every selected idle creature play the tool's hold animation.
// Flags: /O2 /Ob1 /MD /Gy /TP /arch:SSE /GS-
#include "types.h"

// ------------------------------------------------------------------ vtables
struct cAudioSystem {
    virtual void a00(); virtual void a04(); virtual void a08(); virtual void a0c();
    virtual void a10(); virtual void a14(); virtual void a18(); virtual void a1c();
    virtual int GetState();                           // +0x20
};
struct cToggleable {
    virtual void t00(); virtual void t04(); virtual void t08(); virtual void t0c();
    virtual void t10(); virtual void t14(); virtual void t18(); virtual void t1c();
    virtual void t20(); virtual void t24();
    virtual void SetActive(int active);               // +0x28
};

struct cGameObject {
    virtual void o00(); virtual void o04(); virtual void o08(); virtual void o0c();
    virtual void o10(); virtual void o14(); virtual void o18(); virtual void o1c();
    virtual void o20(); virtual void o24(); virtual void o28(); virtual void o2c();
    virtual void o30(); virtual void o34(); virtual void o38(); virtual void o3c();
    virtual void o40(); virtual void o44(); virtual void o48(); virtual void o4c();
    virtual void o50(); virtual void o54(); virtual void o58(); virtual void o5c();
    virtual void o60(); virtual void o64(); virtual void o68(); virtual void o6c();
    virtual void o70(); virtual void o74(); virtual void o78(); virtual void o7c();
    virtual void o80(); virtual void o84(); virtual void o88(); virtual void o8c();
    virtual void o90(); virtual void o94(); virtual void o98(); virtual void o9c();
    virtual void oa0(); virtual void oa4(); virtual void oa8(); virtual void oac();
    virtual void ob0(); virtual void ob4();
    virtual void* Cast(uint32_t typeID);              // +0xb8
    virtual int AddRef();                             // +0xbc
    virtual int Release();                            // +0xc0
};

void FreeArray(void* p);                              // 0x00f47380 (EA allocator)

// selection element: holds one reference, released when the vector dies
struct cGameObjectPtr {
    cGameObject* mpObject;
    ~cGameObjectPtr() { if (mpObject) mpObject->Release(); }
};

template <class T> __forceinline T* DoDestroyValues(T* first, T* last)
{
    for (; first < last; ++first)
        first->~T();
    return last;
}

struct sp_vector_allocator {
    uint32_t mData[2];
    __forceinline void deallocate(void* p) { if (((uint32_t*)p)[-1]) FreeArray(p); }
};

template <class T> struct SPVector {
    T* mpBegin;
    T* mpEnd;
    T* mpCapacity;
    sp_vector_allocator mAllocator;
    SPVector() : mpBegin(0), mpEnd(0), mpCapacity(0) {}
    __forceinline ~SPVector()
    {
        DoDestroyValues(mpBegin, mpEnd);
        if (mpBegin)
            mAllocator.deallocate(mpBegin);
    }
    T* begin() { return mpBegin; }
    T* end() { return mpEnd; }
};

struct cTerrainCursor {
    virtual void c00(); virtual void c04(); virtual void c08(); virtual void c0c();
    virtual void c10(); virtual void c14(); virtual void c18(); virtual void c1c();
    virtual void c20(); virtual void c24(); virtual void c28(); virtual void c2c();
    virtual void c30(); virtual void c34(); virtual void c38(); virtual void c3c();
    virtual void c40(); virtual void c44(); virtual void c48(); virtual void c4c();
    virtual void c50(); virtual void c54(); virtual void c58();
    virtual void GetSelection(SPVector<cGameObjectPtr>& out);   // +0x5c
};

struct cAnimRequest {
    uint32_t mPad00;
    uint32_t mPad04;
    uint32_t mPad08;
    uint32_t mAnimA;                                  // +0x0c
    uint32_t mAnimB;                                  // +0x10
};
struct cAnimQueue {
    cAnimRequest* Play(int a, int b, float blend, int c);   // 0x00bc97f0
};
struct cAnimatedCreature {
    uint32_t mPad00;
    uint32_t mPad04;
    cAnimQueue mQueue;                                // +0x08
    uint32_t mPad0c[(0x1d8 - 0x0c) / 4];
    uint32_t mModelType;
};

// virtual part only: the vptr lives at +0x00, so members start right after it
struct cCreatureVtbl {
    virtual void r00(); virtual void r04(); virtual void r08(); virtual void r0c();
    virtual void r10(); virtual void r14(); virtual void r18(); virtual void r1c();
    virtual void r20(); virtual void r24(); virtual void r28(); virtual void r2c();
    virtual void r30(); virtual void r34(); virtual void r38(); virtual void r3c();
    virtual void r40(); virtual void r44(); virtual void r48(); virtual void r4c();
    virtual void r50(); virtual void r54(); virtual void r58(); virtual void r5c();
    virtual void r60(); virtual void r64(); virtual void r68(); virtual void r6c();
    virtual void r70(); virtual void r74(); virtual void r78(); virtual void r7c();
    virtual void r80(); virtual void r84(); virtual void r88(); virtual void r8c();
    virtual void r90(); virtual void r94(); virtual void r98(); virtual void r9c();
    virtual void ra0(); virtual void ra4(); virtual void ra8(); virtual void rac();
    virtual void rb0(); virtual void rb4();
    virtual void* Cast(uint32_t typeID);              // +0xb8
    virtual int AddRef();                             // +0xbc
    virtual int Release();                            // +0xc0
};
struct cCreature : cCreatureVtbl {
    enum { TYPE = 0x4f176642 };
    uint32_t mPad04[(0xb4c - 4) / 4];
    cAnimatedCreature* mpAnimatedCreature;            // +0xb4c
    bool IsIdle();                                    // 0x00c24560
};

template <class T> __forceinline T* object_cast(cGameObject* p)
{
    return p ? (T*)p->Cast(T::TYPE) : 0;
}

// ------------------------------------------------------------------ helpers
cAudioSystem* GetAudioSystem();                       // 0x00a206f0
void KillSetiEffects(uint32_t id, int state);         // 0x00435ed0
cTerrainCursor* GetGameTerrainCursor();               // 0x00b30d70
extern const float kAnimBlend_01486110;               // 0x01486110

static __forceinline int GetAudioState()
{
    cAudioSystem* audio = GetAudioSystem();
    return audio ? audio->GetState() : 0;
}

// ------------------------------------------------------------------ the function
struct cTribeModeStrategy {
    uint8_t pad00[0x390];
    int mToolMode;                                    // +0x390
    cToggleable* mpToolUI;                            // +0x394

    void cd71b0(int mode, bool playCue);
};

// @ 0x00cd71b0
void cTribeModeStrategy::cd71b0(int mode, bool playCue)
{
    if (playCue) {
        switch (mode) {
        case 0:
            KillSetiEffects(0x381d5506, GetAudioState());
            break;
        case 1:
            KillSetiEffects(0x99a5b10f, GetAudioState());
            break;
        }
    }

    mToolMode = mode;
    if (mpToolUI)
        mpToolUI->SetActive(0);

    cTerrainCursor* cursor = GetGameTerrainCursor();
    if (cursor) {
        SPVector<cGameObjectPtr> selection;
        cursor->GetSelection(selection);
        for (cGameObjectPtr* it = selection.begin(); it != selection.end(); ++it) {
            cGameObject* object = it->mpObject;
            cCreature* creature = object_cast<cCreature>(object);
            if (creature && creature->IsIdle()) {
                cAnimRequest* req = creature->mpAnimatedCreature->mQueue.Play(0, 8, kAnimBlend_01486110, 0);
                if (mode == 0) {
                    req->mAnimA = 0x55bb346;
                    req->mAnimB = 0x55bb34b;
                } else if (mode == 1) {
                    req->mAnimA = 0x55bb337;
                    req->mAnimB = 0x55bb341;
                }
            }
        }
    }
}
