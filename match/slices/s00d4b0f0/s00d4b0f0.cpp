#include "game/types.h"
#include "SP/cVarListSerializer.h"

extern "C" void EA_IO_WriteUint32(void *stream, const void *data, int count, int flags); // 0x93aa70

struct RefObj { void **m_vtbl; };
typedef void (__thiscall *ReleaseThunk)(void *self);

static __forceinline void AutoRefRelease(void **ppField)
{
    void *pOld = *ppField;
    if (pOld != 0) {
        *ppField = 0;
        ((ReleaseThunk)((RefObj *)pOld)->m_vtbl[1])((RefObj *)pOld);
    }
}

struct cSPDramaManager {
    void  **m_vtbl;         // +0x00
    uint8_t       m_pad04[0x2c];
    void    *m_stream30;    // +0x30

    bool Write(void *stream); // 0x00d4b0f0
};

// @ 0x00d4b0f0
bool cSPDramaManager::Write(void *stream)
{
    void *o = ((void *(__thiscall *)(void *))(*(void ***)stream)[0x20 / 4])(stream);
    uint32_t sig = 0x9226d7d3u;
    void *s = ((void *(__thiscall *)(void *))(*(void ***)o)[0x18 / 4])(o);
    EA_IO_WriteUint32(s, &sig, 1, 0);

    void *pRef = m_stream30;
    if (pRef != 0
        && ((uint8_t (__thiscall *)(void *))(*(void ***)pRef)[0x2c / 4])(pRef) != 0) {
        AutoRefRelease(&m_stream30);
    }

    SP::cVarListSerializer ser((char *)this - 8,
                               (SP::cDataSerializerGeneric *)0x01583b00,
                               SP::cVarListSerializer::kSignature);
    return ser.Serialize(*(SP::cIObjectDatabaseOStream *)stream);
}
