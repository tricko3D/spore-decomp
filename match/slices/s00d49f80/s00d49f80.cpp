// @ 0x00d49f80
// SPGonzago: draws a window's two glow quads (fill + border image) over its rect.
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
#include "game/types.h"

typedef unsigned int uint;

struct Image { char pad[0x1c]; int width; int height; };                  // +0x1c / +0x20
struct RenderContext {
    uint Begin2D(int flags);                                              // 0x0095bc10 (ret 4)
    void End2D();                                                         // 0x0095bb20
};
struct DrawParams { uint a; uint b; uint c; uint d; };
struct ImageSource {
virtual void s0();
virtual void s1();
virtual void s2();
virtual void s3();
    virtual Image* GetImage(int which);                                   // slot 4 (+0x10)
};
struct Painter {
virtual void s0();
virtual void s1();
virtual void s2();
virtual void s3();
    virtual void Prepare(RenderContext* ctx, const float* rect, const DrawParams* p); // slot 4 (+0x10)
    int pad4[2];
    ImageSource src;                                                      // +0xc
    float* A830c90(float* tmp, int n);                                    // 0x00830c90 ret 8
    float* A830cc0(float* tmp, int n);                                    // 0x00830cc0 ret 8
    uint   A830c80(int n);                                                // 0x00830c80 ret 4
    Image* A830cf0(int n);                                                // 0x00830cf0 ret 4
    float* A830d30(float* tmp, int n);                                    // 0x00830d30 ret 8
    float* A830d60(float* tmp, int n);                                    // 0x00830d60 ret 8
    uint   A830d10(int n);                                                // 0x00830d10 ret 4
};
struct IObj {                                                             // generic UI object
virtual void s0();
virtual void s1();
virtual void s2();
    virtual IObj* Query(uint id);                                         // slot 3 (+0xc)
virtual void s4();
virtual void s5();
virtual void s6();
virtual void s7();
    virtual uint GetFlags();                                              // slot 8 (+0x20)
};
struct IWin {                                                             // window (arg3)
virtual void s0();
virtual void s1();
virtual void s2();
    virtual IObj* Query(uint id);                                         // slot 3 (+0xc)
virtual void s4();
virtual void s5();
virtual void s6();
virtual void s7();
virtual void s8();
virtual void s9();
virtual void s10();
virtual void s11();
    virtual uint Get30();                                                 // slot 12 (+0x30)
    virtual void s13();
    virtual const float* GetRect();                                       // slot 14 (+0x38)
virtual void s15();
virtual void s16();
virtual void s17();
virtual void s18();
virtual void s19();
virtual void s20();
virtual void s21();
virtual void s22();
virtual void s23();
virtual void s24();
virtual void s25();
virtual void s26();
virtual void s27();
virtual void s28();
virtual void s29();
virtual void s30();
virtual void s31();
virtual void s32();
virtual void s33();
virtual void s34();
virtual void s35();
virtual void s36();
virtual void s37();
virtual void s38();
virtual void s39();
virtual void s40();
    virtual uint GetA4();                                                 // slot 41 (+0xa4)
    virtual IObj* GetA8();                                                // slot 42 (+0xa8)
};

// unnamed retail strands (address in the comment)
void F00805bb0(uint ctxToken, float a, float b, float c, float d, uint e, uint f,
               float g, float h, Image* img, float i);                    // 0x00805bb0 (cdecl, 11 args)
extern float g_UIGlowScale;                                               // 0x0169e434

template<class T> const T& maxref(const T& a, const T& b)
{
    return a > b ? a : b;
}

// @ 0x00d49f80
void DrawUIGlow(RenderContext* ctx, float f, IWin* pWin)
{
    IObj* a = pWin ? pWin->Query(0x8ed27e7a) : 0;
    uint flags = a->GetFlags();
    uint8_t flip = (uint8_t)(flags >> 2);
    flip &= 1;
    IObj* o2 = pWin->GetA8();
    Painter* p = o2 ? (Painter*)o2->Query(0x53eb526) : 0;
    const float* r = pWin->GetRect();
    float rect[4] = { r[0], r[1], r[2], r[3] };
    rect[2] = rect[2] - rect[0];
    rect[3] = rect[3] - rect[1];
    rect[0] = 0.0f;
    rect[1] = 0.0f;
    uint v30 = pWin->Get30();
    DrawParams dp;
    dp.c = pWin->GetA4();
    dp.d = v30;
    dp.a = 1;
    dp.b = 0;
    p->Prepare(ctx, rect, &dp);
    float u0, u1;
    if (flip) {
        u0 = 1.5707964f;
        u1 = 1.5707964f - g_UIGlowScale * f;
    } else {
        u1 = 1.5707964f - g_UIGlowScale;
        u0 = 1.5707964f - g_UIGlowScale * f;
    }
    ImageSource* is = &p->src;
    Image* img = is->GetImage(7);
    if (!img)
        img = is->GetImage(0);
    if (img) {
        float w = (float)img->width;
        float h = (float)img->height;
        float t0[2];
        float* q0 = p->A830c90(t0, 7);
        float x0 = q0[0], y0 = q0[1];
        float* q1 = p->A830cc0(t0, 7);
        float x1 = q1[0], y1 = q1[1];
        uint c = p->A830c80(7);
        x0 *= (rect[2] - rect[0]) / maxref(w, 1.0f);
        y0 *= (rect[3] - rect[1]) / maxref(h, 1.0f);
        uint tok = ctx->Begin2D(0);
        F00805bb0(tok, x1 + (rect[2] - rect[0]) * 0.5f, y1 + (rect[3] - rect[1]) * 0.5f,
                  x0 * w, y0 * h, c, c, u1, u0, img, 0.05f);
        ctx->End2D();
    }
    img = p->A830cf0(0);
    if (img) {
        float w = (float)img->width;
        float h = (float)img->height;
        float t0[2];
        float* q0 = p->A830d30(t0, 7);
        float x0 = q0[0], y0 = q0[1];
        float* q1 = p->A830d60(t0, 7);
        float x1 = q1[0], y1 = q1[1];
        uint c = p->A830d10(7);
        uint tok = ctx->Begin2D(0);
        F00805bb0(tok, x1 + (rect[2] - rect[0]) * 0.5f, y1 + (rect[3] - rect[1]) * 0.5f,
                  x0 * w, y0 * h, c, c, u1, u0, img, 0.05f);
        ctx->End2D();
    }
}
