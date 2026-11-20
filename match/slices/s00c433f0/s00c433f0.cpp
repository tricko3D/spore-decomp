// Spore retail 00c433f0 -- object initialiser (thiscall: ecx = object, no stack args).
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// The owning class could not be identified: no header under src/game/include/ and no
// record in the dev PDB types database matches this retail layout, so the members are
// named after the offsets the original function writes.  Gaps stay as explicit padding;
// the original leaves them untouched too.
//
// 0x20/0x24/0x28 are loaded from three adjacent dwords of a data global at
// 0x168e7e4..0x168e7ec (a default vector).  That global sits past the raw data of the
// image's .data section, so its contents are not recoverable here: it is declared, not
// assigned a made-up constant.  The original returns `this` in EAX (mov eax, ecx on
// entry, kept to the ret), so this function returns the object pointer.

// The default vector the original copies into +0x20..+0x28 (0x168e7e4).
extern float g_defaultVector3[3];

struct cInit_c433f0
{
    unsigned char mPad00[0x0c];   // 0x00  base subobject, not touched here
    int           m0c;            // 0x0c
    int           m10;            // 0x10
    int           m14;            // 0x14
    unsigned char mPad18[0x08];   // 0x18
    float         m20;            // 0x20  (default vector, unknown constant)
    float         m24;            // 0x24
    float         m28;            // 0x28
    float         m2c;            // 0x2c
    int           m30;            // 0x30
    unsigned char mPad34[0x24];   // 0x34 .. 0x57
    bool          m58;            // 0x58
    float         m5c;            // 0x5c
    float         m60;            // 0x60
    float         m64;            // 0x64
    int           m68;            // 0x68
    float         m6c;            // 0x6c
    float         m70;            // 0x70
    float         m74;            // 0x74
    float         m78;            // 0x78
    int           m7c;            // 0x7c
    unsigned char mPad80[0x04];   // 0x80
    int           m84;            // 0x84
    int           m88;            // 0x88
    int           m8c;            // 0x8c
};

// @ 0x00c433f0
cInit_c433f0 *__fastcall FUN_00c433f0(cInit_c433f0 *self)
{
    self->m0c = 0;
    self->m10 = 0;
    self->m14 = 0;

    self->m20 = g_defaultVector3[0];
    self->m24 = g_defaultVector3[1];
    self->m28 = g_defaultVector3[2];

    self->m2c = 1.0f;

    self->m30 = 0;
    self->m58 = false;
    self->m5c = 0.0f;
    self->m60 = 0.0f;
    self->m64 = 0.0f;

    self->m6c = 2.0f;
    self->m70 = 0.9f;
    self->m68 = 0;
    self->m74 = 3.402823466e+38f;   // FLT_MAX
    self->m78 = 0.0f;
    self->m7c = 0;
    self->m84 = 0;
    self->m88 = 0;
    self->m8c = 0;
    return self;
}
