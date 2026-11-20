// @ 0x00e89060
// City view (cBuildingView-family) simulation update: refreshes the view's visual
// effect from the placeable object's model/transform, and maintains its minimap icon.
// Build with the neighbouring SPCityViews flags:
//   /O2 /MD /Gy /TP /arch:SSE /fp:fast /Isrc/game/include   (the SSE arch is required:
//   retail uses movss/comiss where the default /arch yields x87 fld/fcomp).
#include "game/types.h"

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;

#define VS(o, off)              (*(void**)(*(char**)(o) + (off)))
#define VC0(R, o, off)          ((R (__thiscall*)(void*))VS(o, off))((void*)(o))
#define VC1(R, o, off, T1, a1)  ((R (__thiscall*)(void*, T1))VS(o, off))((void*)(o), (a1))
#define VC3(R, o, off, T1, a1, T2, a2, T3, a3) \
    ((R (__thiscall*)(void*, T1, T2, T3))VS(o, off))((void*)(o), (a1), (a2), (a3))
#define VCF(o, off)             ((float (__thiscall*)(void*))VS(o, off))((void*)(o))

struct Vec3f { float x, y, z; };
struct Mat3 { float m[9]; void Assign(const Mat3* src); };   // 0x0041cb40 Matrix3::Assign
// 0x0059c190 Matrix3 copy constructor: writes to `out` and returns it.
extern "C" Mat3* FUN_0059c190(Mat3* out, const Mat3* src);

// The effect-instance descriptor handed to cVisualEffect::SetParams (vtable +0x18).
struct EffectDesc {
    u16   flags;      // +0x00
    u16   count;      // +0x02
    Vec3f pos;        // +0x04
    float scale;      // +0x10
    Mat3  rot;        // +0x14
};

// Minimap icon record (same 0x24-byte shape used by cSPMission::AddIcon callers).
struct CityIcon {
    int   color;      // +0x00
    float x;          // +0x04
    float y;          // +0x08
    float z;          // +0x0c
    int   flags;      // +0x10
    void* mpText;     // +0x14
    float f18;        // +0x18 (written as a float zero by the original)
    int   pad[2];     // +0x1c
};

struct Ref { void* Assign(void* p); };                       // 0x00b5f950
struct Win {
    Win* GetWin();                                           // 0x01067e60
    void RemoveIcon(void* city, int a, int b);               // 0x00e0f9c0
    void AddIcon(void* city, void* icon, int a, int b);      // 0x00e10580
};
struct Svc { int GetId(); };                                 // 0x00b1f9d0
struct Host { bool Test(); };                                // 0x00bd83a0 (thiscall)

extern "C" void* FUN_010666a0();                    // 0x010666a0 minimap provider accessor
extern "C" void* FUN_0067ddd0();                    // 0x0067ddd0 effects world
extern "C" void* FUN_00a206f0();                    // 0x00a206f0
extern "C" void* FUN_00b3d300();                    // 0x00b3d300 service accessor
extern "C" void  FUN_00571f80(u32 id, void* obj, float x, float y, float z);  // 0x00571f80

extern const float kOne;              // 0x01485720 (1.0f)
extern const float kPosX;             // 0x016c4bbc
extern const float kPosY;             // 0x016c4bc0
extern const float kPosZ;             // 0x016c4bc4
extern const Mat3  kMat_16c4b98;      // 0x016c4b98 identity matrix

struct View {
    char  pad_00[0x0c];
    void* mpObject;       // +0x0c
    char  pad_10[0x80 - 0x10];
    void* mpEffect_80;    // +0x80 refcounted effect
};

void __fastcall FUN_00e89060(void* ecx_this, void* edx)
{
    (void)edx;
    View* self = (View*)ecx_this;

    void* obj = (self->mpObject != 0)
                    ? (void*)VC1(void*, self->mpObject, 0xb8, u32, 0xe9cb8ba)
                    : 0;

    // One POD frame: retail keeps these at esp+0x10 (p), esp+0x1c (icon) and
    // esp+0x40 (descriptor); the Matrix3 temp reuses the icon slot at esp+0x1c.
    Vec3f p;
    CityIcon icon;
    EffectDesc d;

    d.flags = 0;
    d.count = 0;
    d.pos.x = kPosX;
    d.pos.y = kPosY;
    d.pos.z = kPosZ;
    d.scale = kOne;
    d.rot.Assign(&kMat_16c4b98);

    d.pos = *(Vec3f*)VC0(void*, self->mpObject, 0x2c);
    d.flags |= 4;
    d.count += 1;

    d.rot = *FUN_0059c190((Mat3*)&icon,
                              (const Mat3*)VC0(void*, self->mpObject, 0x30));
    void* pObject = self->mpObject;
    d.flags |= 2;
    d.count += 1;
    d.scale = VCF(pObject, 0x34);
    d.count += 1;

    if (obj != 0) {
        void* sub = (void*)VC1(void*, obj, 0xc, u32, 0x1007ae63);
        if (sub != 0 && (*(float*)((char*)VC0(void*, sub, 0x84) + 0x748) >= 100.0f
                         || ((Host*)VC0(void*, sub, 0x84))->Test())) {
            if (self->mpEffect_80 == 0) {
                void* world = FUN_0067ddd0();
                void* old = self->mpEffect_80;
                if (old != 0) {
                    self->mpEffect_80 = 0;
                    VC0(void, old, 4);
                }
                if (VC3(bool, world, 0x2c, u32, 0x7dbe7f0, int, 0,
                        void**, &self->mpEffect_80)) {
                    VC1(void, self->mpEffect_80, 8, int, 0);

                    Vec3f* mp = (Vec3f*)VC0(void*, (char*)sub + 0x34, 0x2c);
                    p.x = mp->x;
                    p.y = mp->y;
                    p.z = mp->z;
                    void* o = FUN_00a206f0();
                    o = (o != 0) ? VC0(void*, o, 0x20) : 0;
                    FUN_00571f80(0xf7e5f6e3, o, p.x, p.y, p.z);

                    int cur = *(int*)((char*)VC0(void*, sub, 0x84) + 0x74c);
                    if (cur == ((Svc*)FUN_00b3d300())->GetId()) {
                        icon.f18 = 0.0f;
                        icon.x = d.pos.x;
                        icon.y = d.pos.y;
                        icon.mpText = 0;
                        icon.pad[0] = 0;
                        icon.pad[1] = 0;
                        icon.flags = 0x20000;
                        icon.z = d.pos.z;
                        icon.color = (int)0xff000000;
                        ((Ref*)&icon.mpText)->Assign(VC0(void*, sub, 0x84));

                        ((Win*)FUN_010666a0())->GetWin()->RemoveIcon(self, 1, 0);
                        ((Win*)FUN_010666a0())->GetWin()->AddIcon(self, &icon, 0, 0);

                        if (icon.mpText != 0)
                            VC0(void, icon.mpText, 4);
                    }
                }
            }
            if (self->mpEffect_80 != 0)
                VC1(void, self->mpEffect_80, 0x18, void*, &d);
            return;
        }
    }

    if (self->mpEffect_80 != 0) {
        VC1(void, self->mpEffect_80, 0xc, int, 0);
        void* eff = self->mpEffect_80;
        if (eff != 0) {
            self->mpEffect_80 = 0;
            VC0(void, eff, 4);
        }
        ((Win*)FUN_010666a0())->GetWin()->RemoveIcon(self, 1, 0);
    }
}
