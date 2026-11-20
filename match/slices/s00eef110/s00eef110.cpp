// @ 0x00eef110
#include "game/types.h"

inline void* operator new(unsigned int, void* memory) { return memory; }

// Element layout shared with the editor-model vector in s00eeebd0.
struct Entry32 {
    uint32_t mA;
    float mB;
    float mC;
    float mD;
    float mE;
    float mF;
    float mG;
    float mH;

    __forceinline Entry32(const Entry32& value)
        : mA(value.mA), mB(value.mB), mC(value.mC), mD(value.mD),
          mE(value.mE), mF(value.mF), mG(value.mG), mH(value.mH) {}
};

struct EntryVector {
    Entry32* mpBegin;
    Entry32* mpEnd;
    Entry32* mpCapacity;
    void* mAllocatorData[2];

    void DoInsertValue(Entry32* position, const Entry32& value);
    void push_back(const Entry32& value);
};

// @ 0x00eef110
void EntryVector::push_back(const Entry32& value) {
    Entry32* end = mpEnd;
    if (end < mpCapacity) {
        mpEnd = end + 1;
        if (end)
            ::new (end) Entry32(value);
    } else {
        DoInsertValue(mpEnd, value);
    }
}
