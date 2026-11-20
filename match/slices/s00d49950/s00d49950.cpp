// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
// Detail-window slot driver: finds the "image" window in a layout, resolves
// the drawable behind each and copies the source drawable's per-index slots
// (image id, colour, offset pair, image index) onto the destination drawable.
// The five whole-drawable appearance values are copied as they are; the eight
// per-slot offsets are scaled by the ratio of the two windows' widths
// (destination / source, the source span clamped to 0x1485720 = 1.0f).

#include "game/types.h"
#include "cSPUILayout.h"
#include "EA/UTFWin/IWindow.h"

// EA::RectT<float>, as returned by IWindow slot 14 (GetPhysicalArea).
struct Area {
    float left, top, right, bottom;
};

// eastl::max shape (reference result): the retail divisor is selected between
// the source span and the constant by address.
template <class T>
inline T const& Max(T const& a, T const& b) { return a > b ? a : b; }

// The appearance interface embedded at drawable+0x0c (retail vtable 0x141a7c0):
// an IUnknown-like head followed by getter/setter pairs.  Only the pairs this
// driver uses are named; the slot comments give the retail vtable offsets.
struct DrawableLook {
    virtual ~DrawableLook();                        // 0x00
    virtual int AddRef();                           // 0x04
    virtual int Release();                          // 0x08
    virtual void* AsInterface(unsigned int id);     // 0x0c
    virtual int GetImage(int index);                // 0x10
    virtual void SetImage(int index, int value);    // 0x14
    virtual int GetValue18();                       // 0x18
    virtual void SetValue18(int value);             // 0x1c
    virtual int GetValue20();                       // 0x20
    virtual void SetValue20(int value);             // 0x24
    virtual int GetValue28();                       // 0x28
    virtual void SetValue28(int value);             // 0x2c
    virtual int GetValue30();                       // 0x30
    virtual void SetValue30(int value);             // 0x34
    virtual float GetValue38();                     // 0x38
    virtual void SetValue38(float value);           // 0x3c
};

// cSPUIStdDrawable as seen by the slot loop (see src/game/Spui/SPUIStdDrawable.cpp).
struct DrawableView {
    void* vptr0;            // +0x00
    void* vptr4;            // +0x04
    void* f8;               // +0x08
    DrawableLook mLook;     // +0x0c

    int    GetField14(int index);                 // 0x00830c80
    int    GetField18(int index);                 // 0x00830d10
    float* GetPair38(float* out, int index);      // 0x00830d30
    void   SetImageColor(int index, int color);   // 0x00831170
    void   SetField18At(int index, int value);    // 0x008311f0
    void   SetPair38At(int index, float* pair);   // 0x008312f0
};

// @ 0x00d49950
void FUN_00d49950(cSPUILayout* pSourceLayout, EA::UTFWin::IWindow* pDestWindow)
{
    EA::UTFWin::IWindow* pSourceWindow = pSourceLayout->FindWindowByID(0x06283c92u, true);

    // IWindow slot 42 is the fill drawable; slot 3 casts it to the interface
    // 0x053eb526 (cSPUIStdDrawable).
    void* pSourceFill = ((void* (__thiscall*)(void*))(*(void***)pSourceWindow)[0x0a8 / 4])(pSourceWindow);
    DrawableView* pSource = pSourceFill
        ? (DrawableView*)((void* (__thiscall*)(void*, int))(*(void***)pSourceFill)[0x00c / 4])(pSourceFill, 0x053eb526)
        : 0;

    void* pDestFill = ((void* (__thiscall*)(void*))(*(void***)pDestWindow)[0x0a8 / 4])(pDestWindow);
    DrawableView* pDest = pDestFill
        ? (DrawableView*)((void* (__thiscall*)(void*, int))(*(void***)pDestFill)[0x00c / 4])(pDestFill, 0x053eb526)
        : 0;

    if (!pSource || !pDest)
        return;

    // Whole-drawable appearance: every source getter has its setter four bytes
    // later in the destination's appearance vtable.
    pDest->mLook.SetValue18(pSource->mLook.GetValue18());
    pDest->mLook.SetValue20(pSource->mLook.GetValue20());

    // The source's full-drawable getter also fills a scratch value.
    int scratch;
    pDest->mLook.SetValue28(
        ((int (__thiscall*)(void*, int*))(*(void***)pSource)[0x020 / 4])(pSource, &scratch));

    pDest->mLook.SetValue30(pSource->mLook.GetValue30());
    pDest->mLook.SetValue38(pSource->mLook.GetValue38());

    // Width ratio of the two windows (the destination window's own physical area
    // against the source window's), with a span of at least one unit (0x1485720).
    Area const* pDestArea = (Area const*)((float* (__thiscall*)(void*))(*(void***)pDestWindow)[0x38 / 4])(pDestWindow);
    float destLeft = pDestArea->left;
    float destRight = pDestArea->right;

    Area const* pSourceArea = (Area const*)((float* (__thiscall*)(void*))(*(void***)pSourceWindow)[0x38 / 4])(pSourceWindow);
    float sourceLeft = pSourceArea->left;
    float sourceWidth = pSourceArea->right - sourceLeft;
    float scale = (destRight - destLeft) / Max(sourceWidth, 1.0f);

    unsigned index = 0;
    do {
        pDest->mLook.SetImage(index, pSource->mLook.GetImage(index));
        pDest->SetImageColor(index, pSource->GetField14(index));

        float pair[2];
        pSource->GetPair38(pair, index);
        pair[0] *= scale;
        pair[1] *= scale;
        pDest->SetPair38At(index, pair);

        pDest->SetField18At(index, pSource->GetField18(index));
        ++index;
    } while (index < 8);
}
