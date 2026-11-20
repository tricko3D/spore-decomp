// 0x00d16620 GetGameplayMarkerTypes
// SP::`anonymous namespace'::GetGameplayMarkerTypes
// (u32 __cdecl(u32 propID, u32* pTypes))
//
// Fetches the "LevelEditor" property list through SP::PropertyManager, looks up
// the marker-type array property `propID`, and converts every entry's name
// (a wide string stored in the 0x30-byte record at +0x10) back to its gameplay
// marker id with the name->id registry (0x00d14600).  The inlined Property
// accessors (flags at +0x10, array count at +0x08, base at +0x00) and the
// ref-counted property-list pointer are taken from the same headers the
// neighbouring s00d15440 slice uses for 0x00d15b30.
//
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast /GS-
#include "types.h"

typedef unsigned int u32;
typedef unsigned short u16;

struct Property;
struct PropertyList;
struct IPropertyManager;

// ---- engine data globals ---------------------------------------------------
extern "C" wchar_t DAT_01479b30[] = L"LevelEditor"; // 0x01479b30  tuning set name

// ---- engine entry points ---------------------------------------------------
u32 __cdecl EA_Hash_FNV1_String16(const wchar_t* s, u32 basis, int flag); // 0x00932f30
u32 __cdecl FUN_00d14600(char* name);                                    // 0x00d14600 name -> id
void __cdecl operator_delete__(void* p);                                 // 0x00f47380
IPropertyManager* __cdecl SP_PropertyManager();                          // 0x0067de30

// ---- string8 / property types ---------------------------------------------
struct String8 {          // EASTL string8: heap block when capacity - begin > 1
    char* mpBegin;        // +0x00
    u32   mnSize;         // +0x04
    char* mnCapacity;     // +0x08
    u32   mAllocator;     // +0x0c
};

String8* __cdecl ConvertToString8(String8* out, const void* wstr); // 0x0093c570

struct Property {
    union {
        struct { void* mpData; u32 mnItemSize; u32 mnItemCount; };
        u32 mValue[4];
    };
    unsigned short mnFlags;   // +0x10  (0x30 mask => pointer valued)
    unsigned short mnType;    // +0x12
    void* GetValue() { return (mnFlags & 0x30) ? mpData : (mnType ? (void*)this : 0); }
};

struct PropertyList {
    virtual int AddRef();
    virtual int Release();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual bool HasProperty(u32 propID);       // +0x1c
    virtual void v20();
    virtual void v24();
    virtual Property* GetProperty(u32 propID);  // +0x28
};

struct IPropertyManager {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual bool GetPropertyList(u32 instanceID, PropertyList** ppList); // +0x30
};

template <class T> struct AutoRefCount {
    T* mpObject;
    AutoRefCount() : mpObject(0) {}
    ~AutoRefCount() { if (mpObject) mpObject->Release(); }
    void Reset() { if (mpObject) { T* p = mpObject; mpObject = 0; p->Release(); } }
    T** operator&() { return &mpObject; }
    T* operator->() const { return mpObject; }
};

// @ 0x00d16620 GetGameplayMarkerTypes
bool __cdecl GetGameplayMarkerTypes(u32 propID, u32* pTypes)
{
    AutoRefCount<PropertyList> list;
    IPropertyManager* pm = SP_PropertyManager();

    list.Reset();

    u32 key = EA_Hash_FNV1_String16(DAT_01479b30, 0x811c9dc5, 1);

    if (pm->GetPropertyList(key, &list) && list->HasProperty(propID)) {
        Property* prop = list->GetProperty(propID);

        u32 count;
        if (prop->mnFlags & 0x30)
            count = prop->mnItemCount;
        else
            count = (prop->mnType != 0);

        // A pointer-valued property holds an array of at most 0x60 strings;
        // anything larger is a corrupt tuning file.
        if (count <= 0x60) {
            const char* base = (const char*)prop->GetValue();
            for (u32 i = 0; i < count; i += 3) {
                String8 name;
                char* begin = ConvertToString8(&name, base + 0x10 + i * 0x10)->mpBegin;
                pTypes[i / 3] = FUN_00d14600(begin);
                if (name.mnCapacity - name.mpBegin > 1 && name.mpBegin != 0)
                    operator_delete__(name.mpBegin);
            }

            return true;
        }
    }
    return false;
}
