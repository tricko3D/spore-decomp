// Verb-tray hotkey lookup: walks the button table and returns the ability id
// whose key binding equals `key`.  The active verb panel (+0x169e394) selects
// which button-table category is skipped.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /GS-

typedef unsigned char u8;
typedef unsigned int  u32;
typedef int           i32;

// The 5x5 global button table (0x0169e3ec).
struct ButtonEntry
{
    i32 mID;          // +0x00 (-1 = unused slot)
    u8  mFlag;        // +0x04
    u8  pad05[3];     // +0x05
};

struct ButtonCategory
{
    ButtonEntry mEntries[5];   // +0x00 .. +0x27
};

struct ButtonTable
{
    union
    {
        ButtonEntry    mAllEntries[0x19];   // +0x000
        ButtonCategory mCategories[5];      // +0x000
    };
    u32 mFieldC8;          // +0x0c8 (category count, signed)
    u32 mCategoryCount;    // +0x0cc (entries per category, signed)
    u32 mBase0;            // +0x0d0
    u32 mBase1;            // +0x0d4
};

extern ButtonTable* gButtonTable;                                  // 0x0169e3ec

// The ability record an id resolves to.
struct AbilityRec
{
    u8  pad00[0x0c];
    i32 mKind;                                                 // +0x0c
    u8  pad10[0x38 - 0x10];
    i32 mLocalizedId;                                          // +0x38 (localized button id)
};

void* __cdecl FUN_00d1e830(i32 id);                                // 0x00d1e830 ability record lookup
i32   __cdecl FUN_00d49270(i32 id);                                // 0x00d49270 verb-slot of an id

void* __cdecl GetLocaleManager();                                  // 0x0067de40
u8    __cdecl Locale_IsFrench(void* node, const wchar_t* tag);      // 0x006ab760
extern const wchar_t g_1403c98[];                                  // 0x01403c98 L"fr-fr"

struct LocaleNode
{
    virtual void* v00();
    virtual void* v04();
    virtual void* v08();
    virtual void* v0c();
    virtual void* v10();
    virtual void* Slot14();                                        // +0x14
};

// The button id shown for an ability: the French build renames 'fr-fr' id 0x5a to 0x57.
static i32 LocalizeButtonId(AbilityRec* rec)
{
    if (Locale_IsFrench(((LocaleNode*)GetLocaleManager())->Slot14(), g_1403c98)
        && rec->mLocalizedId == 0x5a)
        return 0x57;
    return rec->mLocalizedId;
}

// @ 0x00d4a370
i32 __cdecl FUN_00d4a370(int panel, int key, bool localized)
{
    if (gButtonTable == 0)
        return -1;

    int other = (panel != 1);
    for (int i = 0; i < (i32)gButtonTable->mFieldC8; ++i)
    {
        if (other == i)
            continue;
        for (int j = 0; j < (i32)gButtonTable->mCategoryCount; ++j)
        {
            i32 id = gButtonTable->mCategories[i].mEntries[j].mID;
            if (id == -1)
                continue;
            AbilityRec* rec = (AbilityRec*)FUN_00d1e830(id);
            if (rec == 0)
                continue;

            int base = j + 0x31;
            int hot;
            switch (rec->mKind)
            {
            case 2:
            case 3:
            {
                i32 shown = LocalizeButtonId(rec);
                hot = shown;
                break;
            }
            case 4:
                hot = base + 0x3f;
                break;
            default:
                hot = base;
                break;
            }

            if (key == hot)
            {
                if (!localized)
                    return id;
                if (FUN_00d49270(id) == 0)
                    return id;
            }
        }
    }
    return -1;
}
