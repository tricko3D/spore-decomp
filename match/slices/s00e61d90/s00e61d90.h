// Shared declarations for slice s00e61d90 (0x00e61d90).
#pragma once

// SP::cCellGame -- only the fields this helper reads.
struct CellGameView {
    char pad0000[0x40fc];
    int  mCurCell40fc;              // 0x40fc  cell the camera is in
};

// SP::cCellNotifyState -- the per-cell effect bookkeeping this helper walks.
struct CellNotifyState {
    char  pad0000[0x4c];
    float mBlock4c[3];              // 0x4c  position/transform block
    char  pad0058[0x108 - 0x58];
    void* mOuterMap108;             // 0x108  outer lookup map
    char  pad010c[0x35c - 0x10c];
    int   mCell35c;                 // 0x35c  current cell index
};

// 4-byte zero-initialised lookup handle (0x00743b50 / 0x00e82130 pair).
struct KeyHandle { void* mValue; };

extern CellGameView* gspCellGame;                    // 0x016b3c04

void __fastcall FUN_00743b50(void* handle);           // 0x00743b50  handle ctor (handle in ECX)
void __fastcall FUN_00e82130(void* handle);           // 0x00e82130  handle release (handle in ECX)
void* __cdecl FUN_00e4cc40(void* map, void* key);     // 0x00e4cc40  nested lookup (map, key on the stack)

// 0x00e61b90 -- spawn one directed cell effect.  Its real body is slice s00e61b90;
// the stand-in in the .cpp needs an outside reference to stay emitted.
void KeepSpawnDirectedCellEffect(void* pos, void** entry, int spaceEffects, void* target);

// keeps the static 0x00e61d90 emitted (its real callers are in another slice)
void CellNotifyState_Notify(CellNotifyState* self, char* source, int index);
