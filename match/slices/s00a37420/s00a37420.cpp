// @ 0x00a37420
#include "game/types.h"

typedef unsigned int u32;
struct AudioSystem;
AudioSystem* GetSystemAT(); // 0x00a206f0
void FUN_00572020(void*, int); // 0x00572020

struct SoundEffect {
    void* vtbl0;      // +0x00
    void* vtbl1;      // +0x04
    int   m8;         // +0x08
    void* m0c;        // +0x0c
    float m10;        // +0x10
    float m14;        // +0x14
    float m18;        // +0x18
    bool  b1c;        // +0x1c
    bool  b1d;        // +0x1d
    bool  b1e;        // +0x1e
    bool  b1f;        // +0x1f
    void* m20;        // +0x20
    float m24;        // +0x24
    float m28;        // +0x28
    float m2c;        // +0x2c
    float m30;        // +0x30

    void Activate(int a);                       // 0xa372e0
    bool SetFloatParams(int which, float* val, int x); // 0xa37320
    void Pause(bool p);                         // 0xa37400
    void Stop(int a);                           // 0xa37420
    void ApplyEffect(float dt, int x, void* ctx); // 0xa37480
    void SetLocation(void* t, void* loc);       // 0xa376f0
};

// @ 0x00a37420
void SoundEffect::Stop(int a)
{
    if (b1f) {
        b1f = false;
        if (((*(u32*)((char*)m0c + 8) >> 3) & 1) && m20)
            FUN_00572020(m20, a == 1);
        void* h = m20;
        AudioSystem* s = GetSystemAT();
        if (s)
            ((void (__thiscall*)(void*, void*))((*(void***)s)[0x2c / 4]))(s, h);
        m20 = 0;
    }
}

