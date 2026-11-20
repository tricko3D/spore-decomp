// Slice s00e750c0 -- cell stage: creates the avatar cell at a position and installs the cell's
// per-cell visual effect.  The cell comes from the 0x00e74a20 factory (group = the player cell
// group), its model key is reconciled with the serialised cell data, and the effect slot in
// cCellGfx is released and re-created through CreateEffectSafe (0x00628450).
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast (cell TU; no /EHsc: the resource-ref local
// gets no EH frame)
#include "types.h"

typedef uint8_t uint8;

struct Vector3 {                          // POD: struct copies go through integer registers
    float x, y, z;
};

// Resource-reference local (ctor 0x00743b50, dtor 0x00e82130).
struct cDefRef {
    cDefRef();
    ~cDefRef();
    void* mp;
};

// Block returned by 0x00e4ce40 (cell-definition / tuning lookup).
struct cDefBlock {
    uint32_t pad00[0x34 / 4];
    int      mField34;                    // +0x34
    uint32_t pad38[(0xe4 - 0x38) / 4];
    int      mFieldE4;                    // +0xe4
};

// SP::cCellUI (0x016b3c0c) -- camera fields only; offsets from SP/cCellUI.h.
struct CellUI {
    uint8_t pad00[0x48];
    Vector3 mCameraLookatPosition;        // +0x48
    Vector3 posFollow1;                   // +0x54
    Vector3 posFollow2;                   // +0x60
    uint8_t pad6c[0x88 - 0x6c];
    int     mCameraOffsetZoomMethod;      // +0x88
    uint8_t pad8c[0xb8 - 0x8c];
    int     mProgressFull;                // +0xb8
    float   mCameraBox;                   // +0xbc
};

// EA::ResourceMan::Key -- resource key (3 dwords, copied whole).
struct ModelKey {
    uint32_t mInstance;                   // +0x0 (0 = unset)
    uint32_t mType;                       // +0x4
    uint32_t mGroup;                      // +0x8
};

// SP::cSPCell -- only the model key is touched here.
struct cCell {
    uint8_t pad00[0xfc];
    ModelKey mModelKey;                   // +0xfc
};

// Serialised cell data (cCellGame + 0x5190).
struct cCellSerializableData {
    uint8_t pad00[0x0c];
    int     mField0c;                     // +0x0c
    ModelKey mModelKey;                   // +0x10
};

struct cObjectPool {                      // cSPObjectPoolT<cSPCell,4096> at cCellGame + 0x1c
    void* Get(int index);                 // 0x00b72210
};

struct CellGame {                         // offsets from the retail s00e73f60 view
    uint8_t pad0000[0x1c];
    cObjectPool mCells;                   // +0x001c
    uint8_t pad0020[0x40fc - 0x20];
    int     midPlayerCell;                // +0x40fc
    uint8_t pad4100[0x411c - 0x4100];
    int     mAvatarCellIndex;             // +0x411c
    uint8_t pad4120[0x5190 - 0x4120];
    cCellSerializableData* mpSerializableData;   // +0x5190
};

// Effect object (EA::Swarm::cIVisualEffect): slot 1 is the ref-count release, slot 2 an enable.
struct cIVisualEffect {
    virtual void v00();
    virtual void Release();               // vtable +0x04
    virtual void SetEnabled(int on);      // vtable +0x08
};

// Cell resource (returned by 0x00e679e0 for a model key); cell-type / stage tag at +0x1230.
struct cCellResource {
    uint8_t pad00[0x1230];
    int     mField1230;                   // +0x1230
};

// SP::cCellGfx (0x016b3c08): +0x161c8 is mpMainEffectsWorld (retail s00e73f60 view); the effect
// slot written here sits at +0x160, inside the generated header's unverified collision-sound map.
struct CellGfx {
    uint8_t pad00[0x160];
    cIVisualEffect* mCellEffect;          // +0x160
    uint8_t pad164[0x161c8 - 0x164];
    void*   mpMainEffectsWorld;           // +0x161c8
};

extern CellUI*   gspCellUI;               // 0x016b3c0c
extern CellGame* gspCellGame;             // 0x016b3c04
extern CellGfx*  gspCellGfx;              // 0x016b3c08
extern const char gCellEffectName_1485954[];   // 0x01485954 effect name hashed below

extern "C" void* __cdecl FUN_00e4ce40(cDefRef* ref);                  // -> cDefBlock*
// 8th slot is the factory's radius-or-flag word: integer 1 asks for the flag form.
extern "C" int   __cdecl CreateCell(int group, const Vector3* pos, float elevation, int defId,
                                    int level, float sizeScale, float size, int radiusFlag,
                                    const void* orient, int filter);  // 0x00e74a20 factory
extern "C" void  __cdecl FUN_00e829a0(ModelKey key);                  // 0x00e829a0
extern "C" cCellResource* __cdecl FUN_00e679e0(ModelKey key);         // 0x00e679e0
extern "C" unsigned __cdecl FNV1_String8(const char* s, unsigned seed, int one);  // 0x00932e80
extern "C" void __cdecl CreateEffectSafe(void* world, unsigned id, int flags,
                                         cIVisualEffect** out);       // 0x00628450

// @ 0x00e750c0
void __cdecl sCreateAvatarCell(const Vector3* pos, float elevation)
{
    cDefRef ref;
    cDefBlock* def = (cDefBlock*)FUN_00e4ce40(&ref);

    gspCellUI->mCameraLookatPosition = *pos;
    gspCellUI->posFollow1 = *pos;
    gspCellUI->posFollow2 = *pos;
    gspCellUI->mProgressFull = 0;
    gspCellUI->mCameraBox = 1.0f;
    gspCellUI->mCameraOffsetZoomMethod = 2;

    int cellIndex = CreateCell(gspCellGame->midPlayerCell, pos, 0.0f, def->mField34,
                               -1, elevation, 0.0f, 1, 0, 0);
    gspCellGame->mAvatarCellIndex = cellIndex;
    cObjectPool& cells = gspCellGame->mCells;   // `this` (+0x1c) materialised before the push
    cCell* cell = (cCell*)cells.Get(cellIndex);

    if (gspCellGame->mpSerializableData->mField0c == 1) {
        cell->mModelKey = gspCellGame->mpSerializableData->mModelKey;
    } else {
        if (gspCellGame->mpSerializableData->mModelKey.mInstance != 0)
            cell->mModelKey = gspCellGame->mpSerializableData->mModelKey;
        else
            gspCellGame->mpSerializableData->mModelKey = cell->mModelKey;
        FUN_00e829a0(gspCellGame->mpSerializableData->mModelKey);
    }

    cCellResource* res = FUN_00e679e0(cell->mModelKey);

    int minimum = 0;
    int order;
    {
        cDefRef ref2;
        order = ((cDefBlock*)FUN_00e4ce40(&ref2))->mFieldE4;
    }
    if (order == 1)
        minimum = 2;

    cIVisualEffect** slot = &gspCellGfx->mCellEffect;
    if (res->mField1230 <= minimum) {
        if (*slot) {
            cIVisualEffect* old = *slot;
            *slot = 0;
            old->Release();
        }
        unsigned effectId = FNV1_String8(gCellEffectName_1485954, 0x811c9dc5, 1);
        CreateEffectSafe(gspCellGfx->mpMainEffectsWorld, effectId, 0, slot);
    } else {
        if (*slot) {
            cIVisualEffect* old = *slot;
            *slot = 0;
            old->Release();
        }
        CreateEffectSafe(gspCellGfx->mpMainEffectsWorld, 0xcd13c457, 0, slot);
    }
    gspCellGfx->mCellEffect->SetEnabled(1);
}
