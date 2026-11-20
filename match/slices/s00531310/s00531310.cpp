// @ 0x00531310
#include "game/types.h"
#include "EA/Swarm/cComponentBase.h"

// Keep the base destruction visible to the compiler: the retail module
// inlines both base destructors, but calls the effects-vector destructor.
namespace EA { namespace Swarm {
inline cComponentBase::~cComponentBase() {}
} }

class cIVisualEffect;
struct VectorAutoRef {
    cIVisualEffect** mpBegin;
    cIVisualEffect** mpEnd;
    cIVisualEffect** mpCapacity;
    uint32_t mAlloc[2];
    ~VectorAutoRef(); // 0x004b5440
};

class cSPSkinPaintDistributeEffect : public EA::Swarm::cComponentBase {
public:
    void* mpDesc;
    void* mpWorld;
    int mComponentId;
    VectorAutoRef mEffectsList;
    bool mNeedsFirstTick;
    ~cSPSkinPaintDistributeEffect();
};

// @ 0x00531310
cSPSkinPaintDistributeEffect::~cSPSkinPaintDistributeEffect()
{
    // The member and base destructors perform all cleanup.
}
