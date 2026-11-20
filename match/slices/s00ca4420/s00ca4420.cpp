// Slice s00ca4420 -- emit the grid cursor's indices into the source's index sink.
// The source object and the cursor are both passed on the stack (retail loads them
// from [esp+4] / [esp+8], never from ecx), and the frame reuses the second argument
// slot for the emitted values, so the helper takes the source explicitly.
// Module flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

struct cIndexSink
{
    virtual void vpad0();                              // +0x00
    virtual void vpad1();                              // +0x04
    virtual void vpad2();                              // +0x08
    virtual void vpad3();                              // +0x0c
    virtual void vpad4();                              // +0x10
    virtual void vpad5();                              // +0x14
    virtual void* GetContainer();                      // +0x18
};

struct cGridCursor
{
    uint8_t m_pad00[0x08];
    int* mpBegin;                                      // +0x08
    uint8_t m_pad0C[0x04];
    int* mpRowEnd;                                     // +0x10
    int** mppRow;                                      // +0x14
    int* mpEnd;                                        // +0x18
    int* mpEnd2;                                       // +0x1c
    uint8_t m_pad20[0x04];
    int** mppRowEnd;                                   // +0x24
};

struct cIndexSource
{
    virtual void vpad0();                              // +0x00
    virtual void vpad1();                              // +0x04
    virtual void vpad2();                              // +0x08
    virtual void vpad3();                              // +0x0c
    virtual void vpad4();                              // +0x10
    virtual void vpad5();                              // +0x14
    virtual void vpad6();                              // +0x18
    virtual void Flush();                              // +0x1c
    virtual cIndexSink* GetSink();                     // +0x20

    static void EmitCursor(cIndexSource* src, const cGridCursor* cursor);
};

void Insert(void* container, const int* values, int count, int flags);   // 0x0093aa70

// @ 0x00ca4420
void cIndexSource::EmitCursor(cIndexSource* src, const cGridCursor* cursor)
{
    int a = (int)(cursor->mpEnd - cursor->mpEnd2);
    int b = (int)(cursor->mpRowEnd - cursor->mpBegin);
    int c = (int)(cursor->mppRowEnd - cursor->mppRow) - 1;
    int index = c * 64 + a + b;

    Insert(src->GetSink()->GetContainer(), &index, 1, 0);

    int* element = cursor->mpBegin;
    int** row = cursor->mppRow;
    int* rowEnd = cursor->mpRowEnd;
    int* end = cursor->mpEnd;

    while (element != end) {
        int value = *element;
        Insert(src->GetSink()->GetContainer(), &value, 1, 0);
        if (++element == rowEnd) {
            ++row;
            element = *row;
            rowEnd = element + 64;
        }
    }

    src->Flush();
}
