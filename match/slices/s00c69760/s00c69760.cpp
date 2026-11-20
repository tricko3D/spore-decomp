// @ 0x00c69760
#include "game/types.h"

namespace SP {
class cGameData {
public:
    virtual int AddRef();
    virtual int Release();
    virtual ~cGameData();
    virtual void* AsInterface(unsigned int interfaceID);
};
}

namespace Simulator {
// Retail layout for the owner initialization path; the preceding fields are
// not accessed by this function.
class cNanoDrone {
public:
    unsigned char mStorage[0x1d4];
    unsigned int mOwnerID;

    void Refresh(); // 0x00c69600
    void SetGameDataOwner(SP::cGameData* owner); // 0x00b18550
    void InitOwner(SP::cGameData* owner, unsigned int ownerID);
};

void cNanoDrone::InitOwner(SP::cGameData* owner, unsigned int ownerID)
{
    if (owner && owner->AsInterface(0x01186577)) {
        mOwnerID = ownerID;
        Refresh();
        SetGameDataOwner(owner);
    }
}
}
