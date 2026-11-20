#include "game/types.h"

namespace EA { namespace IO {
class IStream;
void __cdecl WriteUint32(IStream*, const unsigned int*, int, int); // 0x0093aa70
} }

// Serializer interfaces of the SP serialization stream. The slots after
// GetStreamHolder are not named by the headers yet, so they keep their retail
// order here (WriteObject is the one the writers call with a cast object).
struct ISerializerStream {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual EA::IO::IStream* GetStream();           // +0x18
};

struct ISerializer {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void EndItem();                         // +0x1c
    virtual ISerializerStream* GetStreamHolder();   // +0x20
    virtual void v09();                             // +0x24
    virtual void v0a();                             // +0x28
    virtual void WriteObject(void* pObject);        // +0x2c
};

// Prefix of the record: the referenced objects, as a vector of pointers.
struct ObjectRefVector {
    void** mpBegin;     // +0x00
    void** mpEnd;       // +0x04
    void** mpCap;       // +0x08

    unsigned int Count() const { return (unsigned int)(mpEnd - mpBegin); }
};

// Writes the object count of a record and then every referenced object. The
// references are cast to the serializer interface (id 0x179c807) through slot
// 0xb8 of their vtable, which the generated headers do not name yet.
// @ 0x00c75ba0
ISerializer* __cdecl WriteObjectRefVector(ISerializer* pWriter, ObjectRefVector* pItems)
{
    unsigned int nCount = pItems->Count();
    EA::IO::IStream* pStream = pWriter->GetStreamHolder()->GetStream();
    EA::IO::WriteUint32(pStream, &nCount, 1, 0);
    for (unsigned int i = 0; i < pItems->Count(); ++i) {
        void* pObject = pItems->mpBegin[i];
        pWriter->WriteObject(pObject ? ((void* (__thiscall*)(void*, unsigned int))
            (*(void***)pObject)[0xb8 / 4])(pObject, 0x0179c807) : 0);
    }
    return pWriter;
}
