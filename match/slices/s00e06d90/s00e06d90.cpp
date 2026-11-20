// Slice s00e06d90 -- SP::cSPUILoyaltyMeter (partial).
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"
#include "sse_helpers.h"

namespace SP {

struct cSPUILayout {
    void* mpVtable;                                          // +0x00
    char IsVisible();                                        // 0x00810070
    void SetVisibility(int visible);                         // 0x00810590
    void* FindWindowByID(uint32_t nID, int bFind);           // 0x008105b0
};

// --- stand-ins for the objects cSPUILoyaltyMeter::UpdateMeter touches -----
struct cLoyaltyVec3 { float x, y, z; };
struct cLoyaltyRect { float left, top, right, bottom; };
struct cLoyaltyVec2 { float x, y; };

// Packed colour, read at +0x3c of every noun-like object.
struct cLoyaltyPackedColor {
    uint32_t m_nPacked;                                      // +0x00
    uint32_t Packed();                                       // 0x00b6f310
};

struct cLoyaltyNoun {
    uint8_t  mPad00[0x3c];                                   // +0x00 .. +0x3b
    cLoyaltyPackedColor m_color;                             // +0x3c
    cLoyaltyVec3 GetColor();                                 // 0x00c32cd0 (sret)
};

// Empire-side snapshot the meter is given at +0x34.  The +0x74c/+0x754/+0x75c
// words are noun references (they are passed straight to the noun managers).
struct cLoyaltyContextA {
    uint8_t  mPad000[0x748];                                 // +0x000 .. +0x747
    float    m_fValue0;                                      // +0x748
    uint32_t m_nNounRef0;                                    // +0x74c
    float    m_fValue1;                                      // +0x750
    uint32_t m_nNounRef1;                                    // +0x754
    float    m_fValue2;                                      // +0x758
    uint32_t m_nNounRef2;                                    // +0x75c
    cLoyaltyNoun* GetColorSource();                          // 0x00bd9bf0
};

// City-side snapshot the meter is given at +0x38 (same idea, other offsets,
// plus the model-3 value that only this side exposes).
struct cLoyaltyContextB {
    uint8_t  mPad000[0x214];                                 // +0x000 .. +0x213
    uint32_t m_nNounRef3;                                    // +0x214
    uint32_t m_nNounRef0;                                    // +0x218
    uint32_t m_nNounRef1;                                    // +0x21c
    uint32_t m_nNounRef2;                                    // +0x220
    float    m_fValue0;                                      // +0x224
    float    m_fValue1;                                      // +0x228
    float    m_fValue2;                                      // +0x22c
    cLoyaltyNoun* GetColorSource();                          // 0x00bfdf80
    float    GetValue3();                                    // 0x00bfdff0
};

struct cLoyaltyNounManagerA {
    cLoyaltyNoun* GetNoun();                                 // 0x00ba9370
};

struct cLoyaltyNounManagerB {
    cLoyaltyNoun* GetNoun();                                 // 0x00b25f40
};

struct cSPUILoyaltyMeter {
    void*        mpVtable;                                   // +0x00
    uint32_t     mPad04[3];                                  // +0x04 .. +0x0f
    cSPUILayout* mpLayout;                                   // +0x10
    uint32_t     mPad14[3];                                  // +0x14 .. +0x1f
    uint32_t     mValue20;                                   // +0x20
    uint32_t     mValue24;                                   // +0x24
    int32_t      m_nValue;                                   // +0x28 (-1 = unset, else floor 1)
    void*        mpExtra2c;                                  // +0x2c
    uint32_t     mPad30;                                     // +0x30 .. +0x33
    cLoyaltyContextA* mpContextA;                            // +0x34
    cLoyaltyContextB* mpContextB;                            // +0x38
    int32_t      m_nModel;                                   // +0x3c (0..3 switch key)
    uint32_t     mPad40[3];                                  // +0x40 .. +0x4b
    int32_t      m_nModelExtra2;                             // +0x4c (-1 = none)

    void Show();
    void Hide();
    void ShowExpanded();
    void HideExpanded();
    void UpdateMeter();
};

// @ 0x00e07b20
void cSPUILoyaltyMeter::ShowExpanded()
{
    if (mpExtra2c) {
        ((void (__thiscall*)(void*, int, int))((void**)*(void**)mpExtra2c)[0x7c / 4])
            (mpExtra2c, 1, 1);
    }
    if (mpLayout && mpLayout->IsVisible())
        return;
    mpLayout->SetVisibility(1);
    ((void (__thiscall*)(cSPUILoyaltyMeter*))((void**)mpVtable)[0x24 / 4])(this);
    ((void (__thiscall*)(cSPUILoyaltyMeter*))((void**)mpVtable)[0x28 / 4])(this);
    mValue24 = mValue20;
}

// @ 0x00e07b70
void cSPUILoyaltyMeter::HideExpanded()
{
    if (mpLayout && mpLayout->IsVisible()) {
        mpLayout->SetVisibility(0);
        mValue24 = mValue20;
    }
    if (mpExtra2c) {
        ((void (__thiscall*)(void*, int, int))((void**)*(void**)mpExtra2c)[0x7c / 4])
            (mpExtra2c, 1, 0);
    }
}

// @ 0x00e07950
void cSPUILoyaltyMeter::Show()
{
    if (mpLayout && mpLayout->IsVisible())
        return;
    mpLayout->SetVisibility(1);
    ((void (__thiscall*)(cSPUILoyaltyMeter*))((void**)mpVtable)[0x24 / 4])(this);
    ((void (__thiscall*)(cSPUILoyaltyMeter*))((void**)mpVtable)[0x28 / 4])(this);
    mValue24 = mValue20;
}

// @ 0x00e07990
void cSPUILoyaltyMeter::Hide()
{
    if (mpLayout && mpLayout->IsVisible()) {
        mpLayout->SetVisibility(0);
        mValue24 = mValue20;
    }
}

// ---- entities outside this slice ----------------------------------------
uint32_t __cdecl GetAvatarRef();                                     // 0x00b1fdb0
cLoyaltyNoun* __cdecl GetPlayerEmpire();                             // 0x01021300
cLoyaltyNounManagerA* __stdcall NounManagerA(uint32_t nNounRef);     // 0x00b3d2a0
cLoyaltyNounManagerB* __stdcall NounManagerB(uint32_t nNounRef);     // 0x00b3d300
uint32_t __cdecl GetCurrentGameMode();                               // 0x00b5b800
uint32_t __cdecl MakeColor(int32_t nAlpha, int32_t nR, int32_t nG, int32_t nB); // 0x0067ac80

extern const float g_f13f2100;                                       // 0x013f2100 255.0f
extern const float g_f1486110;                                       // 0x01486110   5.0f
extern const float g_f14853e0;                                       // 0x014853e0   4.0f
extern const float g_f13eb95c;                                       // 0x013eb95c   3.0f

static const uint32_t kSpaceGameMode  = 0x01654c05;
static const uint32_t kEditorGameMode = 0x00dbdba1;

enum {
    kWindowCivConversionBar = 0x04d428c1,   // "CivConversionBar"
    kWindowBarFrame         = 0x04d423ca,
    kWindowBarTrack         = 0x04d6c941,
    kWindowBarSegment0      = 0x04d6c780,
    kWindowBarSegment1      = 0x04d6c78e,
    kWindowBarSegment2      = 0x04e13cbf,
    kWindowBarSegment3      = 0x05122c52,
    kWindowBarFill          = 0x04d6cd79
};

// 0xff000000 | r << 16 | g << 8 | b, built as a shift chain so that only the
// green and blue components need a byte mask.
__forceinline uint32_t LoyaltyPackBytes(int32_t nR, int32_t nG, int32_t nB)
{
    int32_t nPack = (nR | ~0xff) << 8;
    nPack = (nPack | (int32_t)(uint8_t)nG) << 8;
    return (uint32_t)(nPack | (int32_t)(uint8_t)nB);
}

// std::max spelling: it returns a reference, which is why retail materialises
// the literal operand in a stack slot and copies the referenced value back.
template <class T>
inline T const& LoyaltyMax(T const& a, T const& b)
{
    return a > b ? a : b;
}

// @ 0x00e06d90
// Recolours and re-lays out the loyalty meter: picks the bar/fill colours from
// the tracked noun (or from the empire/city snapshot for the four models),
// turns the meter value into a 0..90 pixel width and drives the layout windows.
void cSPUILoyaltyMeter::UpdateMeter()
{
    if (mpLayout == 0)
        return;

    int32_t  nNounRef = -1;
    int32_t  nAmount = 0;
    uint32_t nFillColor = 0xff808080;
    uint32_t nGameMode = GetCurrentGameMode();
    uint32_t nBarColor = nFillColor;
    bool     bHasSource = false;

    if (mpExtra2c != 0) {
        bHasSource = true;
        cLoyaltyNoun* pAvatar = NounManagerA(GetAvatarRef())->GetNoun();
        if (pAvatar != 0) {
            cLoyaltyVec3 colorBar = pAvatar->GetColor();
            cLoyaltyVec3 colorFill;
            if (m_nModelExtra2 != -1) {
                cLoyaltyNoun* pExtra = NounManagerB((uint32_t)m_nModelExtra2)->GetNoun();
                if (pExtra != 0)
                    colorFill = pExtra->GetColor();
            } else {
                colorFill = GetPlayerEmpire()->GetColor();
            }
            float fScale = g_f13f2100;
            int32_t nBlue = SseTruncToInt(colorBar.z * fScale);
            int32_t nGreen = SseTruncToInt(colorBar.y * fScale);
            int32_t nRed = SseTruncToInt(colorBar.x * fScale);
            nBarColor = LoyaltyPackBytes(nRed, nGreen, nBlue);

            nBlue = SseTruncToInt(colorFill.z * fScale);
            nGreen = SseTruncToInt(colorFill.y * fScale);
            nRed = SseTruncToInt(colorFill.x * fScale);
            nFillColor = LoyaltyPackBytes(nRed, nGreen, nBlue);
        }
    } else if (mpContextA != 0 || mpContextB != 0) {
        bHasSource = true;

        cLoyaltyNoun* pBar;
        if (mpContextA != 0)
            pBar = mpContextA->GetColorSource();
        else
            pBar = mpContextB->GetColorSource();

        cLoyaltyNoun* pSegmentA = 0;
        cLoyaltyNoun* pSegmentB = 0;

        switch (m_nModel) {
        case 0:
            nNounRef = (int32_t)(mpContextA != 0 ? mpContextA->m_nNounRef0
                                                 : mpContextB->m_nNounRef0);
            pSegmentA = NounManagerB((uint32_t)nNounRef)->GetNoun();
            pSegmentB = NounManagerA((uint32_t)nNounRef)->GetNoun();
            if (pSegmentA != 0 || pSegmentB != 0) {
                if (mpContextA != 0)
                    nAmount = SseTruncToInt(mpContextA->m_fValue0);
                else
                    nAmount = SseTruncToInt(mpContextB->m_fValue0);
                nAmount = LoyaltyMax(nAmount, 1);
            }
            break;
        case 1:
            nNounRef = (int32_t)(mpContextA != 0 ? mpContextA->m_nNounRef1
                                                 : mpContextB->m_nNounRef1);
            pSegmentA = NounManagerB((uint32_t)nNounRef)->GetNoun();
            pSegmentB = NounManagerA((uint32_t)nNounRef)->GetNoun();
            if (pSegmentA != 0 || pSegmentB != 0) {
                if (mpContextA != 0)
                    nAmount = SseTruncToInt(mpContextA->m_fValue1);
                else
                    nAmount = SseTruncToInt(mpContextB->m_fValue1);
                nAmount = LoyaltyMax(nAmount, 1);
            }
            break;
        case 2:
            nNounRef = (int32_t)(mpContextA != 0 ? mpContextA->m_nNounRef2
                                                 : mpContextB->m_nNounRef2);
            pSegmentA = NounManagerB((uint32_t)nNounRef)->GetNoun();
            pSegmentB = NounManagerA((uint32_t)nNounRef)->GetNoun();
            if (pSegmentA != 0 || pSegmentB != 0) {
                if (mpContextA != 0)
                    nAmount = SseTruncToInt(mpContextA->m_fValue2);
                else
                    nAmount = SseTruncToInt(mpContextB->m_fValue2);
                nAmount = LoyaltyMax(nAmount, 1);
            }
            break;
        case 3:
            nNounRef = (int32_t)mpContextB->m_nNounRef3;
            pSegmentA = NounManagerB((uint32_t)nNounRef)->GetNoun();
            pSegmentB = NounManagerA((uint32_t)nNounRef)->GetNoun();
            if (pSegmentA != 0 || pSegmentB != 0)
                nAmount = LoyaltyMax(SseTruncToInt(mpContextB->GetValue3()), 1);
            break;
        }

        if (pBar != 0)
            nBarColor = pBar->m_color.Packed();
        else
            nBarColor = 0xff808080;

        if (pSegmentA != 0)
            nFillColor = pSegmentA->m_color.Packed();

        if (nGameMode == kSpaceGameMode && pSegmentB != 0) {
            cLoyaltyVec3 colorFill = pSegmentB->GetColor();
            nFillColor = MakeColor(0xff,
                                   SseTruncToInt(colorFill.x * g_f13f2100),
                                   SseTruncToInt(colorFill.y * g_f13f2100),
                                   SseTruncToInt(colorFill.z * g_f13f2100));
        }
    }

    if (m_nValue != -1)
        nAmount = LoyaltyMax(m_nValue, 1);

    int32_t nPercent = nAmount * 90 / 100;

    if (!bHasSource)
        return;
    if (nGameMode == kEditorGameMode)
        return;

    cLoyaltyRect area;

    void* pWin = mpLayout->FindWindowByID(kWindowCivConversionBar, true);
    if (pWin != 0)
        ((void (__thiscall*)(void*, uint32_t))((void**)*(void**)pWin)[0x5c / 4])(pWin, nBarColor);

    pWin = mpLayout->FindWindowByID(kWindowBarFrame, true);
    if (pWin != 0) {
        cLoyaltyRect const* pArea =
            ((cLoyaltyRect const* (__thiscall*)(void*))((void**)*(void**)pWin)[0x34 / 4])(pWin);
        area = *pArea;
        area.right = area.left + (float)nPercent;
        ((void (__thiscall*)(void*, cLoyaltyRect*))((void**)*(void**)pWin)[0x60 / 4])(pWin, &area);
        if (nNounRef != -1)
            ((void (__thiscall*)(void*, uint32_t))((void**)*(void**)pWin)[0x5c / 4])(pWin, nFillColor);
    }

    void* pTrack = mpLayout->FindWindowByID(kWindowBarTrack, true);
    if (pTrack != 0) {
        cLoyaltyRect const* pArea =
            ((cLoyaltyRect const* (__thiscall*)(void*))((void**)*(void**)pTrack)[0x34 / 4])(pTrack);
        area = *pArea;
        area.right = area.left + (float)nPercent;
        ((void (__thiscall*)(void*, cLoyaltyRect*))((void**)*(void**)pTrack)[0x60 / 4])(pTrack, &area);
    }

    // No null check in retail here: this uses the frame window found above.
    {
        cLoyaltyVec3 pos;
        ((void (__thiscall*)(void*, cLoyaltyVec3*, cLoyaltyVec2))((void**)*(void**)pWin)[0xc0 / 4])
            (pWin, &pos, cLoyaltyVec2());
    }

    pWin = mpLayout->FindWindowByID(kWindowBarSegment0, true);
    if (pWin != 0) {
        ((void (__thiscall*)(void*, float, float))((void**)*(void**)pWin)[0x64 / 4])
            (pWin, (float)(nPercent - 52), g_f1486110);
        ((void (__thiscall*)(void*, int32_t, bool))((void**)*(void**)pWin)[0x7c / 4])
            (pWin, 1, m_nModel == 0);
    }

    pWin = mpLayout->FindWindowByID(kWindowBarSegment1, true);
    if (pWin != 0) {
        ((void (__thiscall*)(void*, float, float))((void**)*(void**)pWin)[0x64 / 4])
            (pWin, (float)(nPercent - 52), g_f14853e0);
        ((void (__thiscall*)(void*, int32_t, bool))((void**)*(void**)pWin)[0x7c / 4])
            (pWin, 1, m_nModel == 1);
    }

    pWin = mpLayout->FindWindowByID(kWindowBarSegment2, true);
    if (pWin != 0) {
        ((void (__thiscall*)(void*, float, float))((void**)*(void**)pWin)[0x64 / 4])
            (pWin, (float)(nPercent - 52), g_f1486110);
        ((void (__thiscall*)(void*, int32_t, bool))((void**)*(void**)pWin)[0x7c / 4])
            (pWin, 1, m_nModel == 2);
    }

    pWin = mpLayout->FindWindowByID(kWindowBarSegment3, true);
    if (pWin != 0) {
        ((void (__thiscall*)(void*, float, float))((void**)*(void**)pWin)[0x64 / 4])
            (pWin, (float)(nPercent - 50), g_f1486110);
        ((void (__thiscall*)(void*, int32_t, bool))((void**)*(void**)pWin)[0x7c / 4])
            (pWin, 1, m_nModel == 3);
    }

    pWin = mpLayout->FindWindowByID(kWindowBarFill, true);
    if (pWin != 0) {
        ((void (__thiscall*)(void*, float, float))((void**)*(void**)pWin)[0x64 / 4])
            (pWin, (float)(nPercent - 53), g_f13eb95c);
        ((void (__thiscall*)(void*, int32_t, bool))((void**)*(void**)pWin)[0x7c / 4])
            (pWin, 1, true);
        if (nNounRef != -1)
            ((void (__thiscall*)(void*, uint32_t))((void**)*(void**)pWin)[0x5c / 4])(pWin, nFillColor);
    }
}

} // namespace SP
