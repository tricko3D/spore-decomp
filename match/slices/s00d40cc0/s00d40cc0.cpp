// Slice s00d40cc0 -- SPGonzago claim-nest action (ClaimNearbyNestForAvatar).
//
// The avatar claims the herd/nest record of the passed creature: it is detached
// from its current herd, the noun manager selects the new herd, the avatar is
// appended to the new herd's member vector and made to point at it.  The herd
// layout (member vector at +0x40, flock flag at +0x84, words at
// +0x88/+0x90/+0xf0) is the cAvatarHerd view used by the sibling herd slice
// (0x00d40930).

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef int int32_t;

#define nullptr 0

class cCreature;
class cAvatarHerd;
class cSpeciesProfile;
class cTerrainSphere;

struct Vec3 {
    float mX;
    float mY;
    float mZ;
};

// Pointer vector holding refcounted creatures (SPVector of handles).
struct cCreatureVector {
    void** mpBegin;                // +0x00
    void** mpEnd;                  // +0x04
    void** mpCapacity;             // +0x08

    void grow(void** ppSlot, cCreature** ppValue);                 // 0x00aea5d0
    __forceinline void push_back(cCreature* pObject);
};

// The player's herd record as the herd helpers see it.  COM-style refcounted
// object: the vtable pointer is the first word and the class itself is not C++
// polymorphic here (the fields keep their retail absolute offsets).
struct cAvatarHerd {
    void*          mpVtable;       // +0x000
    uint8_t        mPad004[0x40 - 4]; // +0x004
    cCreatureVector mMembers;      // +0x040
    uint8_t        mPad04c[0x84 - 0x4c]; // +0x04c
    uint8_t        mbFlockFlag;    // +0x084
    uint8_t        mPad085[0x88 - 0x85]; // +0x085
    uint32_t       mField88;       // +0x088
    uint8_t        mPad08c[0x90 - 0x8c]; // +0x08c
    uint32_t       mField90;       // +0x090
    uint8_t        mPad094[0xf0 - 0x94]; // +0x094
    uint32_t       mFieldF0;       // +0x0f0

    void AddRef() {
        ((void (__thiscall*)(cAvatarHerd*))((void**)mpVtable)[0])(this);
    }
    void Release() {
        ((void (__thiscall*)(cAvatarHerd*))((void**)mpVtable)[1])(this);
    }
};

// Pointer-sized refcounted handle: one word, default-constructed to null, and its
// destructor releases the herd it holds.
class cHerdRef {
public:
    cHerdRef() : mpObject(nullptr) {}
    ~cHerdRef() {
        if (mpObject != nullptr)
            mpObject->Release();
    }
    cAvatarHerd* Get() const { return mpObject; }

    cAvatarHerd* mpObject;             // +0x00
};

class cCreature {
public:
    cAvatarHerd* GetAvatarHerd();                                    // 0x00c04590
    void         SetHerd(cAvatarHerd* pHerd);                        // 0x00c045a0
    void*        FUN_00c0bc00();                                     // 0x00c0bc00

    void AddRef() {
        ((void (__thiscall*)(cCreature*))((void**)mpVtable)[0])(this);
    }
    void Release() {
        ((void (__thiscall*)(cCreature*))((void**)mpVtable)[1])(this);
    }

    void*    mpVtable;             // +0x000
    uint8_t  mPad004[0x128 - 4];   // +0x004
    cHerdRef mHerd;                // +0x128  herd the creature belongs to
};

// eastl::vector<EA::AutoRefCount<cCreature> >::push_back at herd+0x40.  The
// handle keeps its own reference on the pushed object; the temporary built for
// the argument releases it again after the copy (also used by 0x00db5e80).
__forceinline void cCreatureVector::push_back(cCreature* pObject)
{
    cCreature* keep = pObject;
    void**     ppSlot;
    if (pObject != 0)
        pObject->AddRef();
    ppSlot = mpEnd;
    if (ppSlot < (void**)mpCapacity) {
        mpEnd = ppSlot + 1;
        if (ppSlot != 0) {
            *ppSlot = pObject;
            if (pObject != 0)
                pObject->AddRef();
        }
    } else {
        grow(ppSlot, &keep);
    }
    if (keep != 0)
        keep->Release();
}

class cSpeciesManager {
public:
    cSpeciesProfile* GetAvatarProfile();                             // 0x004df420
};

class cNounManager {
public:
    cCreature*      GetAvatar();                                     // 0x00b1fdb0
    void            SetSelectedAnimal(cAvatarHerd* pHerd);           // 0x00b1fdc0
    cTerrainSphere* GetCurrentTerrainSphere();                       // 0x00f67d90
};

class cTerrainSphere {
public:
    void FUN_00c77320(uint32_t nId);                                 // 0x00c77320
};

cSpeciesManager* SpeciesManager();                                   // 0x00401090
cNounManager* NounManager();                                         // 0x00b3d300
cAvatarHerd* GetDesiredAvatarHerd(int nMode);                        // 0x00d40b40
void* FUN_00e39710(uint32_t nEvent, void* pPayload,
                   Vec3* pA, Vec3* pB, Vec3* pC);                    // 0x00e39710
void* remove_1st(cCreatureVector* pVector, cCreature** ppMember);    // 0x00acca00

// @ 0x00d40cc0 ClaimNearbyNestForAvatar
bool ClaimNearbyNestForAvatar(cCreature* pCreature)
{
    cNounManager* pNouns = nullptr;
    cSpeciesManager* pSpecies = SpeciesManager();
    pNouns = NounManager();
    pSpecies->GetAvatarProfile();                   // side effect only
    cCreature* pAvatar = pNouns->GetAvatar();
    cAvatarHerd* pOldHerd = pAvatar->GetAvatarHerd();

    cAvatarHerd* pNewHerd = (pCreature ? pCreature->mHerd : cHerdRef()).Get();
    if (pNewHerd == nullptr || pNewHerd == pOldHerd)
        return false;

    pOldHerd->mbFlockFlag = 0;
    remove_1st(&pOldHerd->mMembers, &pAvatar);
    pNouns->SetSelectedAnimal(pNewHerd);

    pNewHerd->mMembers.push_back(pAvatar);
    pAvatar->SetHerd(pNewHerd);

    pNewHerd->mbFlockFlag = 1;
    pNewHerd->mField88 = pOldHerd->mField88;
    pNewHerd->mFieldF0 = pOldHerd->mFieldF0;
    pNewHerd->mField90 = 0;

    if (pNewHerd == GetDesiredAvatarHerd(1)) {
        cTerrainSphere* pSphere = NounManager()->GetCurrentTerrainSphere();
        pSphere->FUN_00c77320(0x514a219);

        Vec3 v[3] = {};
        FUN_00e39710(0xbd1d1b99, pAvatar->FUN_00c0bc00(),
                     &v[2], &v[1], &v[0]);
    }
    return true;
}
