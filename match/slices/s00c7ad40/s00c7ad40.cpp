// @ 0x00c7ad40
#include "game/types.h"
#include "SP/cGonzagoTimer.h"

namespace SP {
// Retail layout views: the generated cPlayer container offset is unverified.
class cStarRecord {
public:
    unsigned char mPad0000[0x70];
    unsigned int mID;
};
struct CaptureProgressEntry {
    unsigned int first;
    unsigned int mAlignment;
    cGonzagoTimer second;
};
struct CaptureProgressList {
    CaptureProgressEntry* mpBegin;
    CaptureProgressEntry* mpEnd;
};
class cPlayer {
public:
    unsigned char mPad0000[0x1214];
    CaptureProgressList mPlayerCaptureProgressStars;
    void RemoveStarFromPlayerCaptureProgressList(cStarRecord* star);
};
}

// eastl range-copy helper, 0x00c771e0.
SP::CaptureProgressEntry* CopyCaptureProgress(SP::CaptureProgressEntry* first,
    SP::CaptureProgressEntry* last, SP::CaptureProgressEntry* result);

void SP::cPlayer::RemoveStarFromPlayerCaptureProgressList(cStarRecord* star)
{
    CaptureProgressEntry* entry = mPlayerCaptureProgressStars.mpBegin;
    CaptureProgressEntry* end = mPlayerCaptureProgressStars.mpEnd;
    if (entry == end)
        return;
    unsigned int id = star->mID;
    do {
        if (entry->first == id)
            goto found;
        ++entry;
    } while (entry != end);
    return;
found:
    // Erase uses a fresh end pointer after the search.
    end = *static_cast<CaptureProgressEntry* volatile*>(
        &mPlayerCaptureProgressStars.mpEnd);
    if (entry + 1 < end)
        CopyCaptureProgress(entry + 1, end, entry);
    --mPlayerCaptureProgressStars.mpEnd;
    mPlayerCaptureProgressStars.mpEnd->second.cGonzagoTimer::~cGonzagoTimer();
}
