// Slice s00e5f8c0: cell grid-placement test (SPCellGame.obj).
// Retail module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast, no /EHsc.
#include "types.h"
#include "sse_helpers.h"

struct Vector3 { float x, y, z; };

// ---- retail data (absolute addresses; the checker masks the relocations) ----------------
extern const float gKLevelSize[];   // 0x01483bd0  level -> world size
extern const float gKBoxScale;      // 0x013f6a20  0.85f
extern const float gKOne;           // 0x01485720  1.0f

// ---- engine helpers referenced here -----------------------------------------------------
namespace SP {
int   __fastcall sGetSourceLevel(void* unused, int level);                    // 0x00e4ee60
float __cdecl sGetBoxSizeMultiplier2(int level, unsigned flags);             // 0x00e51ff0
bool  __cdecl sBlockIsInRange(const void* block);                            // 0x00e5f810
}

// ---- layouts ----------------------------------------------------------------------------
struct cCellGame;

struct cObjectPool {
  char mReserved[0x38];
  void* GetSafe(int handle);      // 0x00b721d0  thiscall
};

struct cLevelInfo {
  char mReserved[0x1c];
  int  mStage;                    // +0x1c
};

struct cCellGame {
  char        mReserved00[0x1c];
  cObjectPool mCells;             // +0x001c
  char        mReserved54[0x40fc - 0x54];
  int         mField40fc;         // +0x40fc
  char        mReserved4100[0x514c - 0x4100];
  float       mCellSize;          // +0x514c
  char        mReserved5150[0x5190 - 0x5150];
  cLevelInfo* mpLevelInfo;        // +0x5190
};

struct cCell {
  char    mReserved00[0x4c];
  Vector3 mPosition;              // +0x04c
  char    mReserved58[0xac - 0x58];
  float   mFieldAC;               // +0x0ac
  char    mReservedB0[4];
  float   mFieldB4;               // +0x0b4
  char    mReservedB8[0x358 - 0xb8];
  int     mLevel;                 // +0x358
  int     mField35c;              // +0x35c
  char    mReserved360[0x36c - 0x360];
  int     mHandle;                // +0x36c
};

struct cBlock {
  int mFlags;                     // +0x00
  int mIdx;                       // +0x04
  int mRow;                       // +0x08
  int mCol;                       // +0x0c
  int mField10;                   // +0x10  not touched here or by sBlockIsInRange
                                  //        (0x00e5f810 reads only +0x00..+0x0c), but the
                                  //        retail frame (sub esp,0x24) shows a 20-byte
                                  //        block local: 4-byte temp + 0x10 alignment gap
                                  //        + 0x14 block.
};

extern cCellGame* gspCellGame;     // 0x016b3c04

// @ 0x00e5f8c0
bool __cdecl sCellBlockTest(cCell* cell) {
  const int level = cell->mLevel;
  if (level == -1)
    return false;
  cCellGame* game = gspCellGame;
  if (level + 1 < SP::sGetSourceLevel(0, game->mpLevelInfo->mStage)) {
    cell->mFieldB4 = 0.0f;
    cell->mFieldAC = 1.0f;
    return false;
  }
  const int handle = cell->mHandle;
  if (game->mCells.GetSafe(handle) != 0)
    return false;

  cCellGame* grid = gspCellGame;
  const int blockLevel = cell->mLevel;
  cBlock box;
  float gridPos;
  box.mFlags = (cell->mField35c != grid->mField40fc);
  box.mIdx = blockLevel;
  const float scale = (gKLevelSize[blockLevel] / grid->mCellSize) *
                      (SP::sGetBoxSizeMultiplier2(blockLevel, box.mFlags) * gKBoxScale);
  const float inv = gKOne / scale;
  gridPos = cell->mPosition.x * inv;
  box.mRow = SseFloorToInt(gridPos);
  gridPos = cell->mPosition.y * inv;
  box.mCol = SseFloorToInt(gridPos);
  return !SP::sBlockIsInRange(&box);
}
