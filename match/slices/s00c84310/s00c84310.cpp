// Slice s00c84310 -- SP::cSolarSystem::GetStarTemperature.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
//
// Extent note (verified 2026-11-18): this body compiles to the retail instruction stream byte for
// byte -- 188 bytes of code, then the 36-byte switch table the compiler emits after it, which the
// fingerprint masks completely (0 differing unmasked bytes, all 88 relocations on masked offsets;
// the table itself is not byte-compared, so its nine entries were resolved by hand: they land on
// $LN7,$LN7,$LN7,$LN6,$LN5,$LN4,$LN3,$LN2,$LN1 = offsets 0x21,0x21,0x21,0x41,0x4f,0x6a,0x85,
// 0x93,0xae, exactly retail's targets for types 4..12).
// The fingerprint's 271-byte extent also covers 0x00c843f0..0x00c8441e, 47 bytes that belong to a
// different function: `bool f(void*, void*)` (push esi; esi = arg2; release *(void**)arg2 through
// its vtable slot 1; then arg1->vtable[0xa](0x38cfb68, arg2, 0); return true). It is a standalone
// entry -- the same 47-byte shape appears 51 times in .text, and its address is only stored, at
// .text 0x013527a4 / 0x0135280e. Nothing in this function reaches it: all nine switch-table entries
// target blocks before the table and the out-of-range branch targets 0x00c8434c. Because
// fingerprint.extract stops a symbol at the next function symbol, no TU can append that neighbour to
// this function, so this row needs the retail size for 0x00c84310 to be 224 (the generator's
// "ret followed by push ebp; mov ebp, esp" cut does not see a neighbour starting with `push esi`).
// Pass 11 re-verification: F.make(orig, 0xc84310, 271) reproduces the frozen row's sha256 and mask exactly,
// i.e. the row was generated from this image and the 47 extra bytes are compared literally (the mask has no
// run past 188..223, not even for the neighbour's `push 0x38cfb68`). The compiled object's nine table entries
// resolve to $LN7 x3/$LN6/$LN5/$LN4/$LN3/$LN2/$LN1 at 0x21/0x41/0x4f/0x6a/0x85/0x93/0xae, exactly the retail
// table read from the image, so the 224-byte body is byte-identical including the table.
struct PropertyList;

// Reads a float property by name hash; defined elsewhere (0x0040cf10).
bool GetFloatProperty(PropertyList* list, unsigned int id, float& out);

extern PropertyList** gStarProperties; // 0x01694c54

// @ 0x00c84310
float __cdecl GetStarTemperature(int type)
{
    float result = 1.0f;
    switch (type) {
    case 4:
    case 5:
    case 6:
        GetFloatProperty(gStarProperties[type], 0x38bb621, result);
        break;
    case 7:
        return GetStarTemperature(5) * 2.0f;
    case 8:
        return GetStarTemperature(4) + GetStarTemperature(5);
    case 9:
        return GetStarTemperature(5) + GetStarTemperature(6);
    case 10:
        return GetStarTemperature(4) * 2.0f;
    case 11:
        return GetStarTemperature(4) + GetStarTemperature(6);
    case 12:
        return GetStarTemperature(6) * 2.0f;
    }
    return result;
}
