// @ 0x007db5e0
#include "game/types.h"

struct PtrVec {
    void** mpBegin;
    void** mpEnd;
    void* Find(int key);
};

void* PtrVec::Find(int key)
{
    void** begin = mpBegin;
    int count = (int)(mpEnd - begin);
    int index = 0;
    if (count > 0) {
        void** current = begin;
        do {
            if (*(int*)*current == key)
                return begin[index];
            ++index;
            ++current;
        } while (index < count);
    }
    return 0;
}
