// Event-log item layout batch (drains GetEventLogItemLayout into the pool).
#include "game/types.h"
#include "cSPUILayout.h"
#include <new>

cSPUILayout* GetEventLogItemLayout(); // 0x00dd5990

// The event-log layout pool has the three pointer fields of an EASTL vector.
struct EventLogLayoutPool {
    cSPUILayout** mpBegin;
    cSPUILayout** mpEnd;
    cSPUILayout** mpCapacity;

    void Insert(cSPUILayout** position, cSPUILayout* const& layout); // 0x00cd0550

    __forceinline void push_back(cSPUILayout* const& layout)
    {
        if (mpEnd < mpCapacity) {
            cSPUILayout** p = mpEnd++;
            if (p) *p = layout;
        } else {
            Insert(mpEnd, layout);
        }
    }
};
extern EventLogLayoutPool gEventLogLayoutPool; // 0x015a2c40

void __stdcall FUN_00dd7820();

// @ 0x00dd7820
void __stdcall FUN_00dd7820()
{
    cSPUILayout* layouts[10];
    for (int i = 0; i < 10; ++i)
        layouts[i] = GetEventLogItemLayout();

    cSPUILayout* layout;
    for (int i = 0; i < 10; ++i) {
        layout = layouts[i];
        gEventLogLayoutPool.push_back(layout);
        layout->SetVisibility(false);
    }
}
