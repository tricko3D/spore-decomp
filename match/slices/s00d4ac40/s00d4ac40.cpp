// @ 0x00d4ac40
#include "game/types.h"

// Test a time against an hour-based interval, including intervals that wrap.
int FUN_00d4ac40(float time, float start, float end)
{
    start *= 3600.0f;
    end *= 3600.0f;
    if (start > end) {
        if (start > time)
            time += start;
        end = start + end;
    }
    if (time >= start && time <= end)
        return 1;
    return 0;
}
