// @ 0x00b1a250
#include "game/types.h"
#include "eastl/string.h"

struct FlagValueNode {
    FlagValueNode* mpNodeRight;
    FlagValueNode* mpNodeLeft;
    FlagValueNode* mpNodeParent;
    unsigned int mColor;
    eastl::string8 first;
    int second;
};

struct FlagValueMap {
    void __thiscall Find(FlagValueNode** result, const eastl::string8* key);
};

extern FlagValueMap gFlagValues; // 0x015680cc
extern FlagValueNode gFlagValuesAnchor; // 0x015680d0
extern "C" __declspec(dllimport) __declspec(nothrow) long __cdecl atol(const char*);

int __cdecl ParseValueFlag(const eastl::string8* value) {
    FlagValueNode* found;
    gFlagValues.Find(&found, value);
    if (found != &gFlagValuesAnchor)
        return found->second;
    return atol(value->c_str());
}
