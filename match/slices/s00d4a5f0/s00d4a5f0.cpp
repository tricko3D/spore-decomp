// verb-tray tooltip update (first pass; see the marker on the definition below)
#include "game/types.h"

typedef unsigned int  u32;
typedef unsigned char u8;

// ---- localized text -------------------------------------------------------
// eastl::wstring: begin/end/capacity pointers (0x10 bytes including allocator)
struct WStr {
    wchar_t* mpBegin;                                  // +0x00
    wchar_t* mpEnd;                                    // +0x04
    wchar_t* mpCapacity;                               // +0x08
    int      mAlloc;                                   // +0x0c
    WStr& operator=(const wchar_t* s);                 // 0x005c3d90
};

// EA localized string (0x14 bytes): a wide string plus its loaded/unloaded flags
struct cString {
    WStr mText;                                        // +0x00
    u8   mFlags;                                       // +0x10
    u8   mFlag11;                                      // +0x11
    u8   mPad[2];
    cString();                                         // 0x006b5060
    cString(u32 id, u32 arg, const wchar_t* def);      // 0x006b5770
    ~cString();                                        // 0x006b5240
    const wchar_t* GetText();                          // 0x006b55c0
    bool Load(u32 id, u32 arg, const wchar_t* def);    // 0x006b54b0
};

// ---- verb tray data -------------------------------------------------------
struct AbilitySlot {
    int mAbility;                                      // +0x00
    u8  mFlag;                                         // +0x04
    u8  pad1[3];
};
extern AbilitySlot* g_169e3ec;                         // 0x0169e3ec ability-slot table (5x5)
extern u32 g_tooltipTarget;                            // 0x0169e438 button whose tooltip is up

struct AbilityUI {
    char     pad0[8];
    int      mType;                                    // +0x08
    int      mField0C;                                 // +0x0c
    char     pad10[4];
    cString  mText;                                    // +0x14
};

// result of sGetButtonCategoryFromButtonID: category plus a found byte (+0x04)
struct ButtonQuery {
    int  mCategory;                                    // +0x00
    char mFound;                                       // +0x04
    char mPad[3];
};

struct SpeciesDB {
    unsigned int __thiscall GetField(int field);       // 0x004d47d0
};
struct CurrentTray {                                   // object returned by 0x00d1e610
    char  pad[0xb20];
    SpeciesDB* mpSpeciesDB;                            // +0xb20
};

// ---- tooltip text lines (0x14 bytes: hash + wide string) -----------------
struct TooltipLine {
    u32  mHash;                                        // +0x00
    WStr mText;                                        // +0x04
};
extern "C" void __stdcall FUN_00401930(void* dst, unsigned stride, int count, void* ctor); // 0x00401930
extern "C" void __stdcall FUN_00401960(void* dst, unsigned stride, int count, void* dtor); // 0x00401960

// ---- tooltip manager ------------------------------------------------------
class cSPUITooltipManager {
public:
    u32 ShowTooltip(char bShow, void* parent, const u32* key, int wndID, float* offset,
                    const wchar_t* text, u32 data, u32 behavior, char detailed); // 0x008356e0
};
extern void* g_tooltipMgrPtr;                          // 0x0164f328 (points at manager+8)

// the tooltip state the whole function shares: laid out in the retail order
struct TooltipFrame {
    cSPUITooltipManager* mMgr;                         // +0x00
    union {
        ButtonQuery      mQuery;                       // +0x04
        void*            mParent;                      // +0x04 (after the category is read)
    };
    float                mOffset[2];                   // +0x0c
    u32                  mKey[3];                      // +0x14
    int                  mIndex;                       // +0x20
};

// tooltip UI holder reached through 0x0067de40 (locale/UI holder)
struct TooltipHolder {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void* GetTooltipWindow();                  // +0x20
};
struct TooltipWindow {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void SetTooltip(cString* title, WStr* text, u32 behavior); // +0x30
};

// ---- retail callees -------------------------------------------------------
extern "C" char  sGetButtonCategoryFromButtonID(int button, int* pCat, int* pIdx); // 0x00d48bc0
extern "C" void* CreatureGUI(const char* name);                    // 0x00d48b50
extern "C" void* FUN_00d1e830(int ability);                        // 0x00d1e830
extern "C" int   FUN_00d49f20(AbilityUI* ui, int category);        // 0x00d49f20
extern "C" void* FUN_00d1e610(void);                               // 0x00d1e610
extern "C" void* SP_GetCurrentGameMode(void);                      // 0x00b5b800
extern "C" void  SetNumberString(int value, int hi, wchar_t* out, int size); // 0x00881ae0
extern "C" void* GetUITooltipHolder(void);                         // 0x0067de40

// @ 0x00d4a5f0
// show the verb-tray tooltip for one button and retire the previous one
void VerbTray_UpdateTooltip(u32 button)
{
    TooltipFrame frame;
    frame.mMgr = g_tooltipMgrPtr
        ? (cSPUITooltipManager*)((char*)g_tooltipMgrPtr - 8) : 0;
    frame.mQuery.mFound = sGetButtonCategoryFromButtonID(
        (int)button, &frame.mQuery.mCategory, &frame.mIndex);
    // tooltip key: localized-string id plus the type/group hashes of this window
    frame.mKey[0] = 0xc0152a6d;
    frame.mKey[1] = 0x0510a95b;
    frame.mKey[2] = 0x40464100;
    frame.mOffset[0] = 0.0f;                           // tooltip offset, +0x0c
    frame.mOffset[1] = 0.0f;


    if (button != 0 && frame.mQuery.mFound != 0) {
        int category = frame.mQuery.mCategory;
        AbilitySlot* slot = &g_169e3ec[category + frame.mIndex * 5];
        AbilityUI* ui = (AbilityUI*)FUN_00d1e830(slot->mAbility);
        if (ui != 0) {
            frame.mParent = CreatureGUI((const char*)button);
            int n = FUN_00d49f20(ui, category);
            if (n > 0) {
                cString title(0x841536d5u, (u32)n, L"");
                TooltipLine lines[4];                  // +0x4c: four tooltip lines
                FUN_00401930(lines, 0x14, 4, (void*)0xb6a700);
                lines[0].mHash = 0x36792281;
                lines[0].mText = ui->mText.GetText();
                lines[1].mHash = 0x52aaaaab;
                lines[1].mText =
                    cString(0x841536d5u, (u32)n, L"").GetText();
                u32 tipID = 0x681d202;
                u32 behavior = 2;
                CurrentTray* tray = (CurrentTray*)FUN_00d1e610();
                if (tray != 0) {
                    if (((u32)SP_GetCurrentGameMode() == 0x1654c01u
                         || (u32)SP_GetCurrentGameMode() == 0x1654c10u)
                        && (ui->mField0C == 0 || ui->mField0C == 1)) {
                        __int64 value = tray->mpSpeciesDB->GetField(ui->mType);
                        if (value > 0) {
                            tipID = 0x681d203;
                            behavior = 4;
                            lines[2].mHash = 0x2dcc67d7;
                            lines[2].mText =
                                cString(0xd1726890u, 0x4ace6b0u, L"").GetText();
                            wchar_t buf[0x40];
                            SetNumberString((int)value, 0, buf, 0x40);
                            lines[3].mHash = 0x6958c612;
                            lines[3].mText = buf;
                        }
                    }
                }
                cString tip;
                if (tip.Load(0xc0152a6d, tipID, (const wchar_t*)0x13ec468)) {
                    TooltipHolder* holder = (TooltipHolder*)GetUITooltipHolder();
                    TooltipWindow* win = (TooltipWindow*)holder->GetTooltipWindow();
                    win->SetTooltip(&tip, &lines[0].mText, behavior);
                    frame.mMgr->ShowTooltip(1, frame.mParent, frame.mKey, 0x3754e6c, frame.mOffset,
                                     tip.GetText(), 0xffffffffu, 0, 0);
                }
                FUN_00401960(lines, 0x14, 4, (void*)0x6886e0);
            } else {
                frame.mMgr->ShowTooltip(0, frame.mParent, frame.mKey, 0x3754e6c, frame.mOffset,
                                 ui->mText.GetText(), 0xffffffffu, 0, 0);
            }
        }
    }

    // retire the tooltip of the previously hovered button
    if (button != g_tooltipTarget && g_tooltipTarget != 0
        && CreatureGUI((const char*)g_tooltipTarget) != 0) {
        frame.mMgr->ShowTooltip(0, 0, frame.mKey, 0x3754e6c, frame.mOffset, 0, 0xffffffffu, 0, 0);
    }
    g_tooltipTarget = button;
}
