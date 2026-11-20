// @ 0x018d2315
#include <intrin.h>

#pragma intrinsic(_AddressOfReturnAddress)

extern "C" unsigned int __cdecl SLgetlicinfo(unsigned int callAddress);

// The caller enters through a five-byte call stub. Resolve that stub's
// destination and replace the saved return address so execution resumes there.
// C++ expresses the dispatch, but cannot request the original save-all-registers
// convention or its manually aligned call frame.
void __stdcall SLnext_body()
{
    unsigned int* returnAddress =
        static_cast<unsigned int*>(_AddressOfReturnAddress());
    unsigned int callAddress = *returnAddress - 5;
    *returnAddress = SLgetlicinfo(callAddress);
}
