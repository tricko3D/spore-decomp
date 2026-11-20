// @ 0x00cd7010: SP::cTribeModeStrategy::cPosseInfo::cPosseInfo
#include "game/types.h"
#include <string.h>

// The posse name uses the retail EASTL wide-string layout.
class PosseName {
public:
    wchar_t* mpBegin;
    wchar_t* mpEnd;
    wchar_t* mpCapacity;
    unsigned int mAllocator;

    void AllocateSelf(unsigned int capacity); // 0x00429760

    __forceinline PosseName(const PosseName& other)
        : mpBegin(0), mpEnd(0), mpCapacity(0)
    {
        int byteCount = (int)((const char*)other.mpEnd - (const char*)other.mpBegin);
        const wchar_t* first = other.mpBegin;
        AllocateSelf((unsigned int)(byteCount / 2) + 1);
        memcpy(mpBegin, first, (unsigned int)byteCount);
        mpEnd = (wchar_t*)((char*)mpBegin + byteCount);
        *mpEnd = 0;
    }
};

namespace SP {
class cTribeModeStrategy {
public:
    struct cPosseInfo {
        PosseName mName;
        float mField10;
        int mField14;
        int mField18;

        cPosseInfo(const cPosseInfo& other);
    };
};

cTribeModeStrategy::cPosseInfo::cPosseInfo(const cPosseInfo& other)
    : mName(other.mName), mField10(other.mField10),
      mField14(other.mField14), mField18(other.mField18)
{
}
}
