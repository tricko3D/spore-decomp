// Slice s00ff1cb0: SP::cCityData::Init() -- the in-place constructor helper for a city
// record.  0x00ff2e80 allocates 0xc4 bytes, calls this, then reads and stores the classic
// field block: position from a shared default vector, cleared orientation/income/size
// fields, the two empty wide-string sentinels, the 14 building-link words and the three
// member vectors, and finally pre-reserves the building and wall vectors.
//
// Offsets are retail, read off 0xff1cb0..0xff1d9a.  The shared SP::cCityData header still
// carries the dev-PDB layout (its members part company with retail after 0x3c), so the
// record is modelled locally, as in the sibling slice s00ff21d0.
//
// Module flags: /O2 /MD /Gy /TP /arch:SSE
#include "types.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// =========================================================================== shared data
extern float   g_16dad08;             // 0x016dad08 -- default position component x
extern float   g_16dad0c;             // 0x016dad0c -- default position component y
extern float   g_16dad10;             // 0x016dad10 -- default position component z
extern wchar_t gEmptyString16[2];     // 0x01667bac -- eastl shared empty wide string

// =========================================================================== little classes
struct Float3 { float x, y, z; };

struct WStr {                          // eastl::basic_string<wchar_t>, 3 pointers
    wchar_t* mpBegin_00;
    wchar_t* mpEnd_04;
    wchar_t* mpCap_08;
};

struct VecRes {                        // 3-pointer eastl vector with an out-of-line reserve
    void* mpBegin_00;
    void* mpEnd_04;
    void* mpCap_08;
    void reserve(u32 newCap);
};

struct cCityData {                     // retail record, 0xc4 bytes
    Float3 mInitialPos;                // 0x000
    float  mInitialHeight;             // 0x00c (not touched by Init)
    u32    m10;                        // 0x010 (not touched by Init)
    float  m14;                        // 0x014 (not touched by Init)
    float  m18;                        // 0x018 (not touched by Init)
    float  mPosZ;                      // 0x01c
    u32    m20;                        // 0x020
    u32    m24;                        // 0x024
    u8     m28;                        // 0x028
    u8     m29[3];                     // 0x029
    u32    m2c;                        // 0x02c
    float  mSpiceProduction;           // 0x030
    int    mMaxSize;                   // 0x034
    int    mSize;                      // 0x038
    float  m3c;                        // 0x03c
    WStr   mName;                      // 0x040
    u32    m4c;                        // 0x04c (not touched by Init)
    WStr   mOwnerName;                 // 0x050
    u32    m5c;                        // 0x05c (not touched by Init)
    u16    maabBuildingLinks[14];      // 0x060
    VecRes mBuilding;                  // 0x07c (cBuildingData)
    u32    m88;                        // 0x088 (not touched by Init)
    u32    m8c;                        // 0x08c (not touched by Init)
    VecRes mOrnament;                  // 0x090 (cOrnamentData)
    u32    m9c;                        // 0x09c (not touched by Init)
    u32    ma0;                        // 0x0a0 (not touched by Init)
    VecRes mWall;                      // 0x0a4 (4-byte elements)
    u32    mb0;                        // 0x0b0 (not touched by Init)
    u32    mb4;                        // 0x0b4 (not touched by Init)
    u32    mb8;                        // 0x0b8 (not touched by Init)
    u32    mWallStyle;                 // 0x0bc
    u32    mc0;                        // 0x0c0

    void Init();                       // 0x00ff1cb0
};

// @ 0x00ff1cb0
void cCityData::Init()
{
    mInitialPos.x = g_16dad08;
    mInitialPos.y = g_16dad0c;
    mInitialPos.z = g_16dad10;

    m20 = 0;
    m24 = 0;
    m28 = 0;
    m2c = 0;

    mPosZ = 0.0f;
    mSpiceProduction = 0.0f;
    m3c = 0.0f;

    mMaxSize = -1;
    mSize = -1;

    mName.mpBegin_00 = gEmptyString16;
    mName.mpEnd_04 = gEmptyString16;
    mName.mpCap_08 = gEmptyString16 + 1;

    mOwnerName.mpBegin_00 = gEmptyString16;
    mOwnerName.mpEnd_04 = gEmptyString16;
    mOwnerName.mpCap_08 = gEmptyString16 + 1;

    mBuilding.mpBegin_00 = 0;
    mBuilding.mpEnd_04 = 0;
    mBuilding.mpCap_08 = 0;

    mOrnament.mpBegin_00 = 0;
    mOrnament.mpEnd_04 = 0;
    mOrnament.mpCap_08 = 0;

    mWall.mpBegin_00 = 0;
    mWall.mpEnd_04 = 0;
    mWall.mpCap_08 = 0;

    mWallStyle = 0;
    mc0 = 0;

    for (int i = 0; i < 14; ++i)
        maabBuildingLinks[i] = 0;

    mBuilding.reserve(10);
    mWall.reserve(8);
}
