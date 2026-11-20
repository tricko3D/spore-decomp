// @ 0x00c448a0
//
// Save/registration path of a cSpatialObject-style writer: drop stale
// id-lookup sources from the object's lookup vector, push one marker through
// the caller's stream, then write the object itself and serialise the
// incoming stream through a cVarListSerializer.
//
// The owner class is not named in the symbol table (FUN_00c448a0); the layout
// below only encodes the two things the code touches: the lookup vector at
// +0x278 and the inherited Write() at 0x00c88a90.
#include "game/types.h"
#include "SP/cVarListSerializer.h"

namespace SP {
class IdLookupSource;              // element type of the +0x278 vector
class cIObjectDatabaseOStream;     // stream handed in by the caller
struct cDataSerializerGeneric;
}

// 0x00c42310: int __cdecl f(SP::IdLookupSource*)  -- keeps one lookup source?
int IdLookupPredicate(SP::IdLookupSource* pSource);

// 0x00c43810: find_if(first, last, pred)
SP::IdLookupSource** FindIf(SP::IdLookupSource** pFirst,
                            SP::IdLookupSource** pLast,
                            int (__cdecl* pPred)(SP::IdLookupSource*));

// 0x0093aa70: EA::IO::WriteUint32(stream, data, count, flags)
void __cdecl WriteUint32(void* pStream, const void* pData, int nCount, int nFlags);

// Objects reached through the argument's virtual slots.
struct IBufferOwner {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void* GetData();          // +0x18
};

struct IStreamProvider {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual IBufferOwner* GetBufferOwner();   // +0x20
};

// vector<SP::IdLookupSource*>; only the erase entry point is out of line.
class LookupSourceVector {
 public:
    SP::IdLookupSource** Erase(SP::IdLookupSource** pFirst,
                               SP::IdLookupSource** pLast);   // 0x00b48e40
 public:
    SP::IdLookupSource** mpBegin;
    SP::IdLookupSource** mpEnd;
    SP::IdLookupSource** mpCap;
};

class cSpatialObjectWriter {
 public:
    void Write(SP::cIObjectDatabaseOStream& stream);           // 0x00c88a90
    void RegisterWithStream(SP::cIObjectDatabaseOStream* pStream);
 private:
    char mPad000[0x278];
    LookupSourceVector mLookupSources;                        // +0x278
};

void cSpatialObjectWriter::RegisterWithStream(SP::cIObjectDatabaseOStream* pStream)
{
    SP::IdLookupSource** pStale =
        FindIf(mLookupSources.mpBegin, mLookupSources.mpEnd, IdLookupPredicate);
    mLookupSources.Erase(pStale, mLookupSources.mpEnd);

    IBufferOwner* pOwner = ((IStreamProvider*)pStream)->GetBufferOwner();
    void* pData = pOwner->GetData();

    // The one dword this path writes ahead of the object itself: retail stores the
    // immediate 0x0116dd1b into the local, so it is a literal tag, not a pointer.
    unsigned int tag = 0x0116dd1bu;
    WriteUint32(pData, &tag, 1, 0);

    Write(*pStream);

    SP::cVarListSerializer serializer(this, (SP::cDataSerializerGeneric*)0x01574258,
                                      0x01a80d26u);
    serializer.Serialize(*pStream);
}
