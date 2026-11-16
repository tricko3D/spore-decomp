
#include "types.h"

namespace SP {
    class cCivilization_s00cf_e160;
}

class cCivRefTable {
public:
    SP::cCivilization_s00cf_e160* SetField10(
        SP::cCivilization_s00cf_e160* value);
};

extern cCivRefTable* gCivRefTable_0169d3c8;

namespace SP {

class cSPUIEventLog {
public:
    int PostFeedbackEvent(
        uint32_t eventID,
        uint32_t eventType,
        void* context,
        int a,
        int b,
        int c);
};

cSPUIEventLog* __cdecl FUN_00b3d3e0();

class cCivModeStrategy {
public:
    int PostFeedback(
        cCivilization_s00cf_e160* civilization,
        uint32_t eventID,
        uint32_t eventType,
        void* context,
        int a,
        int b,
        int c);
};

// @ 0x00cf9040
int cCivModeStrategy::PostFeedback(
    cCivilization_s00cf_e160* civilization,
    uint32_t eventID,
    uint32_t eventType,
    void* context,
    int a,
    int b,
    int c)
{
    cCivRefTable* table = gCivRefTable_0169d3c8;

    cCivilization_s00cf_e160* previous =
        table->SetField10(civilization);

    int result = FUN_00b3d3e0()->PostFeedbackEvent(
        eventID, eventType, context, a, b, c);

    table->SetField10(previous);

    return result;
}

} // namespace SP
