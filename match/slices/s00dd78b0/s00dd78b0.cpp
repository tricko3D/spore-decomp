// 0x00dd78b0 -- refresh the event log's main-window area and, the first time
// through, build the shared award-icon layout.
//
// The layout pool is drained first, then the area is taken from the UI's main
// window.  The award-icon cSPUILayout is created lazily and only initialised on
// the creation path.
// @ 0x00dd78b0
#include "game/types.h"

// Main-window area, {left, top, right, bottom} (0x00805ea0 fills it).
struct Area4 {
    float x;
    float y;
    float z;
    float w;
};

// EA::ResourceMan::Key: the name hash plus the group and type ids.
struct ResourceKey {
    unsigned int mName;
    unsigned int mGroup;
    unsigned int mType;
};

struct cSPUILayout {
    cSPUILayout* Construct();                                        // 0x00810000
    bool Init(const ResourceKey& key, bool visible, unsigned int id); // 0x008120d0
};

struct cSPUIEventLog {
    char  pad000[0x48];
    float mArea[4];             // +0x48..+0x57
    void Refresh();
};

extern cSPUILayout* gAwardIconLayout;   // 0x016a095c

void         __cdecl FUN_00dd7820();                                    // 0x00dd7820 (pool drain)
Area4*       __cdecl SPUIHelpers_GetMainWindowArea(Area4* out);         // 0x00805ea0
void*        __cdecl EaAlloc(unsigned int size, const char* name,
                             int, int, int, int);                       // 0x00f473a0
unsigned int __cdecl FNV1_String8(const char* text, unsigned int seed,
                                  int charCase);                        // 0x00932e80

// The two locals are laid out together: the resource key sits at the bottom of
// the frame and the queried area immediately above it.
struct FrameLocals {
    ResourceKey key;
    Area4       area;
};

// @ 0x00dd78b0
void cSPUIEventLog::Refresh()
{
    FrameLocals locals;

    FUN_00dd7820();
    SPUIHelpers_GetMainWindowArea(&locals.area);

    // The four values are built as one Area4; the zero in xmm1 stays live for
    // the three zero fields while the height lives in xmm0.
    Area4 area = { 0.0f, locals.area.w - locals.area.y, 0.0f, 0.0f };

    mArea[0] = area.x;
    mArea[1] = area.y;
    mArea[2] = area.z;
    mArea[3] = area.w;

    if (gAwardIconLayout == 0) {
        cSPUILayout* mem = (cSPUILayout*)EaAlloc(0x18, "cSPUILayout", 0, 0, 0, 0);
        if (mem != 0)
            gAwardIconLayout = mem->Construct();
        else
            gAwardIconLayout = 0;

        // Retail name string at 0x0147cec8; only its hash reaches the code.
        locals.key.mName = FNV1_String8("iconic", 0x811c9dc5, 1);
        locals.key.mGroup = 0x510a95b;
        locals.key.mType = 0x40464100;
        gAwardIconLayout->Init(locals.key, false, 0x5b598fa);
    }
}
