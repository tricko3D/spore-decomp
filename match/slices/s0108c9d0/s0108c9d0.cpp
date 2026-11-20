// @ 0x0108c9d0
#include "../s010eb310/hk31_math.h"

extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)

class hkPhantom : public hkReferencedObject
{
public:
    ~hkPhantom(); // 0x0108e550
};

// Only the storage between the referenced-object header and the overlap array
// is opaque here; the array is at the hkSimpleShapePhantom offset 0x120.
class hkSimpleShapePhantom : public hkPhantom
{
public:
    char m_shapePhantomStorage[0x120 - 8];
    struct hkCollisionDetail { void* m_collidable; };
    hkArray<hkCollisionDetail> m_collisionDetails;
    void* scalarDeletingDtor(unsigned int flags);
};

// @ 0x0108c9d0
void* hkSimpleShapePhantom::scalarDeletingDtor(unsigned int flags)
{
    // Install the actual derived vtable before invoking allocator callbacks.
    *(const void* volatile*)this = (const void*)0x0149e3b8;
    _ReadWriteBarrier();
    if ((m_collisionDetails.m_capacityAndFlags & 0x80000000u) == 0)
    {
        hkThreadMemory_getInstance()->deallocateChunk(
            m_collisionDetails.m_data,
            (m_collisionDetails.m_capacityAndFlags & 0x3fffffff) * 4,
            HK_MEMORY_CLASS_ARRAY_T);
    }
    hkPhantom::~hkPhantom();
    if (flags & 1)
    {
        hkMemory::s_instance->deallocateChunk(
            this, (uint16_t)m_memSizeAndFlags, HK_MEMORY_CLASS_PHANTOM_T);
    }
    return this;
}
