// @ 0x00c69f80
#include "game/types.h"

namespace SP {
struct cSpeciesProfile;

// Partial retail layouts: generated headers use different offsets here.
struct cSpeciesArchetype {
    unsigned char mBeforePercentBabies[0x364];
    float percentBabies;
};

class cSPEditorSpeciesManager {
public:
    const cSpeciesProfile* GetAvatarProfile(); // 0x004df420
    const cSpeciesArchetype* GetSpeciesArchetype(unsigned int, unsigned int); // 0x004e0050
};

cSPEditorSpeciesManager* GetSpeciesManagerInstance(); // 0x00401090

class cHerd {
public:
    unsigned char mBeforeArchetype[0x88];
    unsigned int mArchetype;
    unsigned char mBeforeOwnerSpecies[0xa4 - 0x8c];
    const cSpeciesProfile* mpOwnerSpecies;
    unsigned char mBeforeGeneration[0xf0 - 0xa8];
    unsigned int mGeneration;

    float GetPercentBabies();
};

float cHerd::GetPercentBabies()
{
    float percent = mpOwnerSpecies == GetSpeciesManagerInstance()->GetAvatarProfile()
        ? 0.0f : 0.3f;
    if (mArchetype) {
        const cSpeciesArchetype* archetype =
            GetSpeciesManagerInstance()->GetSpeciesArchetype(mArchetype, mGeneration);
        if (archetype)
            percent = archetype->percentBabies;
    }
    return percent;
}
}
