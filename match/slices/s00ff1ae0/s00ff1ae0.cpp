#include "game/types.h"

// Non-owning formatted-key buffer (+0x00..+0x0f, 16 bytes).  Cleaned up with
// the "odd static empty" sentinel test (capacity sentinel 0x01667bad).
struct cKeyString {
    unsigned short* mpBegin;         // +0x00
    unsigned short* mpEnd;           // +0x04
    unsigned short* mpCapacityEnd;   // +0x08
    void*           mpOwner;         // +0x0c
};

void __cdecl FUN_00f47380(void* p);                                  // 0x00f47380 operator delete[]

// Owning wide string (16 bytes).  Released with the "real heap block" test
// ((cap - begin) & ~1) > 2, which is why the odd sentinels below are safe.
struct cWideString {
    unsigned short* mpBegin;         // +0x00
    unsigned short* mpEnd;           // +0x04
    unsigned short* mpCapacityEnd;   // +0x08
    void*           mpOwner;         // +0x0c
    cWideString* FUN_0056e2d0(const cWideString* src);   // 0x0056e2d0 copy-ctor (thiscall, ret 4)
};

extern "C" unsigned short DAT_01667bac[];   // 0x01667bac eastl::gEmptyString16
extern "C" unsigned short DAT_013f36bc[];   // 0x013f36bc L"prop"
extern "C" char           DAT_015d8b4d;     // 0x015d8b4d show-details flag

cWideString* FUN_0093c5a0(cWideString* out, const char* src, int len);       // 0x0093c5a0
cWideString* FUN_0093c6d0(cWideString* out, const void* src);                // 0x0093c6d0
void __cdecl  FUN_00694f20(cKeyString* out, const void* value);              // 0x00694f20 key formatter

static void* vc(void* obj, int off) { return ((void**)*(void**)obj)[off / 4]; }

#define WSTR_FREE(s) do { \
    if ((((char*)(s).mpCapacityEnd - (char*)(s).mpBegin) & ~1) > 2 && (s).mpBegin) \
        FUN_00f47380((s).mpBegin); \
} while (0)

#define KEY_FREE_CMDLINE(s) do { \
    if (((char*)(s).mpCapacityEnd - (char*)(s).mpBegin) > 1 && (s).mpBegin) \
        FUN_00f47380((s).mpBegin); \
} while (0)

// vtable slots of the save-area object: v00(id) v04(id) v08(type,arg) v0c(name)
#define V0(o, a) ((char (__thiscall*)(void*, const void*))vc(o, 0))(o, a)
#define V8(o, a, b) ((char (__thiscall*)(void*, const void*, const void*))vc(o, 8))(o, a, b)
#define V12(o, a) ((char (__thiscall*)(void*, const void*))vc(o, 0xc))(o, a)
#define V4(o, a) ((char (__thiscall*)(void*, const void*))vc(o, 4))(o, a)

// @ 0x00ff1ae0
char FUN_00ff1ae0(void* pSave, const char* pFileName, const void* pValue, const void* pArg)
{
    volatile int nUnused = 0;
    cKeyString cmdLine;
    bool result = true;

    cmdLine.mpBegin = DAT_01667bac;
    cmdLine.mpEnd = DAT_01667bac;
    cmdLine.mpCapacityEnd = (unsigned short*)0x01667bad;

    FUN_00694f20(&cmdLine, pValue);

    if (cmdLine.mpBegin != cmdLine.mpEnd) {
        // The four wide strings share one 64-byte scratch block.  Keeping them
        // in one group preserves the original's slot order.
        struct cScratch {
            cWideString fileName;        // +0x00
            cWideString typedName;       // +0x10
            cWideString name;            // +0x20
            cWideString tmp;             // +0x30
        } ss;
        cWideString& fileName = ss.fileName;
        cWideString& typedName = ss.typedName;
        cWideString& name = ss.name;
        cWideString& tmp = ss.tmp;

        int mode;
        cWideString* pSource;
        if (pFileName) {
            mode = 1;
            pSource = FUN_0093c5a0(&tmp, pFileName, -1);
        } else {
            mode = 2;
            fileName.mpBegin = DAT_01667bac;
            fileName.mpEnd = DAT_01667bac;
            fileName.mpCapacityEnd = (unsigned short*)0x01667bae;
            pSource = &fileName;
        }
        name.FUN_0056e2d0(pSource);

        if (mode & 2) {
            WSTR_FREE(fileName);
            mode &= ~2;
        }
        if (mode & 1)
            WSTR_FREE(tmp);

        FUN_0093c6d0(&typedName, &cmdLine);

        char ok;
        if (name.mpBegin == name.mpEnd)
            ok = V0(pSave, pArg);
        else {
            ok = V0(pSave, name.mpBegin) != 0;
            if (DAT_015d8b4d && ok)
                ok = V8(pSave, DAT_013f36bc, pArg);
        }
        ok = ok && V12(pSave, typedName.mpBegin) != 0;
        result = ok && (name.mpBegin == name.mpEnd ? V4(pSave, pArg) : V4(pSave, name.mpBegin)) != 0;
        WSTR_FREE(typedName);
        WSTR_FREE(name);
    }
    KEY_FREE_CMDLINE(cmdLine);
    return result;
}
