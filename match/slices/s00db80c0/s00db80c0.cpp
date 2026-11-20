// cGroupFollower::ApplyFormation -- hand each group member a free formation slot
// around the group's centre, in the orientation given by the matrix.
// Flags: /O2 /MD /Gy /TP /arch:SSE /fp:fast
#include "types.h"

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)
inline float sqrtf_s00db_80c0(float x) { return (float)sqrt((double)x); }

struct Vector3 {
    float x, y, z;
    Vector3() {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
};

struct Matrix3 {
    float m[3][3];
};

// the shared zero vector the game keeps at 0x0169f3b8
#define ZeroX (*(const float*)0x0169f3b8)
#define ZeroY (*(const float*)0x0169f3bc)
#define ZeroZ (*(const float*)0x0169f3c0)

void __cdecl SortVecImpl(void** a, void** b, Vector3 v);   // 0x00c7dac0

#define PV_s00db_80c0(n) virtual void pad##n();

struct Object_s00db_80c0 {
    PV_s00db_80c0(0) PV_s00db_80c0(1) PV_s00db_80c0(2) PV_s00db_80c0(3) PV_s00db_80c0(4)
    PV_s00db_80c0(5) PV_s00db_80c0(6) PV_s00db_80c0(7) PV_s00db_80c0(8) PV_s00db_80c0(9)
    PV_s00db_80c0(10)
    virtual const Vector3* GetPosition();          // 0x2c
    PV_s00db_80c0(12) PV_s00db_80c0(13) PV_s00db_80c0(14) PV_s00db_80c0(15)
    PV_s00db_80c0(16) PV_s00db_80c0(17) PV_s00db_80c0(18) PV_s00db_80c0(19)
    PV_s00db_80c0(20) PV_s00db_80c0(21) PV_s00db_80c0(22) PV_s00db_80c0(23)
    PV_s00db_80c0(24) PV_s00db_80c0(25) PV_s00db_80c0(26) PV_s00db_80c0(27)
    PV_s00db_80c0(28) PV_s00db_80c0(29) PV_s00db_80c0(30) PV_s00db_80c0(31)
    PV_s00db_80c0(32) PV_s00db_80c0(33) PV_s00db_80c0(34) PV_s00db_80c0(35)
    PV_s00db_80c0(36) PV_s00db_80c0(37) PV_s00db_80c0(38) PV_s00db_80c0(39)
    PV_s00db_80c0(40) PV_s00db_80c0(41) PV_s00db_80c0(42) PV_s00db_80c0(43)
    PV_s00db_80c0(44) PV_s00db_80c0(45)
    virtual void* Cast(unsigned int id);           // 0xb8
};

struct FormationSlot {
    bool mbTaken;                                  // 0x00
    char pad1[7];
    int mOwnerRef;                                 // 0x08
    Vector3 mOffset;                               // 0x0c
    void Assign(int who);                          // 0x00afa030
    void Clear();                                  // 0x00afa0a0
};

struct Formation {
    char pad0[0x14];
    Vector3 mPosition;                             // 0x14
    char pad20[0x2c - 0x20];
    struct Quaternion {
        float x, y, z, w;
    } mOrientation;                                // 0x2c
    char pad3c[0x50 - 0x3c];
    FormationSlot* mpSlotBegin;                    // 0x50
    FormationSlot* mpSlotEnd;                      // 0x54
    char pad58[0x64 - 0x58];
    FormationSlot* GetSlot(int index);             // 0x00af9ff0
    void GenerateRing(int ringCount);              // 0x00afcc80
};

struct MemberInfo {
    float mTimeNear;                               // 0x00
    float mDistSq;                                 // 0x04
    int mSlot;                                     // 0x08
    MemberInfo() : mTimeNear(0.0f), mDistSq(0.0f), mSlot(-1) {}
};

struct MemberNode {
    Object_s00db_80c0* mKey;                        // 0x00
    MemberInfo mInfo;                               // 0x04
    MemberNode* mpNext;                             // 0x10
};

struct MemberValue {
    Object_s00db_80c0* first;                       // 0x00
    MemberInfo second;                              // 0x04
    MemberValue(Object_s00db_80c0* key, const MemberInfo& value)
        : first(key), second(value) {}
};

struct MemberPairResult {
    MemberNode* first;                              // 0x00
    bool second;                                    // 0x04
};

struct MemberMap {                                 // eastl::hash_map<Object*, MemberInfo>
    unsigned int mFunctors;                        // 0x00
    MemberNode** mpBucketArray;                     // 0x04
    unsigned int mnBucketCount;                     // 0x08
    unsigned int mnElementCount;                    // 0x0c
    unsigned int mNextResize;                       // 0x10
    float mfMaxLoadFactor;                          // 0x14
    float mfGrowthFactor;                          // 0x18
    unsigned int mAllocator;                        // 0x1c

    MemberInfo& operator[](Object_s00db_80c0* const& key);
    MemberPairResult DoInsertValue(const MemberValue& value, bool unique);   // 0x00db47e0
    MemberInfo& FindOrAdd(Object_s00db_80c0* const& key);                    // 0x00db5cf0
};

__forceinline MemberInfo& MemberMap::operator[](Object_s00db_80c0* const& key)
{
    unsigned int bucket = (unsigned int)key % mnBucketCount;
    MemberNode* node = mpBucketArray[bucket];
    while (node != 0 && node->mKey != key)
        node = node->mpNext;
    if (node == mpBucketArray[mnBucketCount]) {
        MemberInfo info;
        MemberValue value(key, info);
        MemberPairResult result = DoInsertValue(value, false);
        node = result.first;
    }
    return node->mInfo;
}

struct cGroupFollower {
    char pad0[8];
    char pad08[0x14];
    Object_s00db_80c0** mMembersBegin;             // 0x1c
    Object_s00db_80c0** mMembersEnd;               // 0x20
    char pad24[0x54 - 0x24];
    MemberMap mInfo;                               // 0x54
    char pad74[0x190 - 0x74];
    Object_s00db_80c0* mpTarget;                   // 0x190
    Vector3 mTargetPos;                            // 0x194
    char pad1a0[0x1ac - 0x1a0];
    float mRepathTimer;                            // 0x1ac
    char pad1b0[0x1d4 - 0x1b0];
    Formation mFormation;                          // 0x1d4

    void ApplyFormation(Matrix3* m);
};

// @ 0x00db80c0
void cGroupFollower::ApplyFormation(Matrix3* m)
{
    unsigned int count = (unsigned int)(mMembersEnd - mMembersBegin);
    Object_s00db_80c0** end = mMembersEnd;
    Vector3 center(ZeroX, ZeroY, ZeroZ);

    for (Object_s00db_80c0** it = mMembersBegin; it != end; ++it) {
        Object_s00db_80c0* object = *it;
        MemberInfo& info = mInfo.FindOrAdd(object);
        if (info.mSlot != -1) {
            FormationSlot* slot = mFormation.GetSlot(info.mSlot);
            if (slot != 0 && slot->mbTaken && slot->mOwnerRef == 0)
                slot->Clear();
        }
        info.mSlot = -1;

        center += *object->GetPosition();
    }

    float invCount = (float)(1.0 / count);
    center *= invCount;

    SortVecImpl((void**)mMembersBegin, (void**)mMembersEnd, center);

    Object_s00db_80c0** stop = mMembersEnd;

    for (Object_s00db_80c0** it = mMembersBegin; it != stop; ++it) {
        Object_s00db_80c0* object = *it;
        const Vector3* pos = object->GetPosition();

        float dx = pos->x - center.x;
        float dy = pos->y - center.y;
        float dz = pos->z - center.z;

        float d1 = m->m[0][0] * dx + m->m[0][1] * dy + m->m[0][2] * dz;
        float d2 = m->m[1][0] * dx + m->m[1][1] * dy + m->m[1][2] * dz;

        float inv = 1.0f / sqrtf_s00db_80c0(d1 * d1 + d2 * d2 + 1e-8f);

        Vector3 dir;
        dir.x = d1 * inv;
        dir.y = d2 * inv;
        dir.z = 0.0f * inv;

        if (dir.x == ZeroX && dir.y == ZeroY && dir.z == ZeroZ) {
            float tx = pos->x - mTargetPos.x;
            float ty = pos->y - mTargetPos.y;
            float tz = pos->z - mTargetPos.z;

            float invDist = 1.0f / sqrtf_s00db_80c0(tx * tx + ty * ty + tz * tz);
            tx = tx * invDist;
            ty = ty * invDist;
            tz = tz * invDist;

            dir.x = m->m[0][0] * tx + m->m[1][0] * ty + m->m[2][0] * tz;
            dir.y = m->m[0][1] * tx + m->m[1][1] * ty + m->m[2][1] * tz;
            dir.z = m->m[0][2] * tx + m->m[1][2] * ty + m->m[2][2] * tz;
        }

        int best = -1;
        float bestScore = 0.0f;
        int i = 0;

        for (;;) {
            int slotCount = (int)(mFormation.mpSlotEnd - mFormation.mpSlotBegin);

            for (; i < slotCount; ++i) {
                FormationSlot* slot = mFormation.mpSlotBegin + i;
                if (slot->mbTaken)
                    continue;

                if (slot->mOffset.x == ZeroX &&
                    slot->mOffset.y == ZeroY &&
                    slot->mOffset.z == ZeroZ) {
                    best = i;
                    break;
                }

                float len = slot->mOffset.x * slot->mOffset.x
                          + slot->mOffset.y * slot->mOffset.y
                          + slot->mOffset.z * slot->mOffset.z;
                float invLen = 1.0f / sqrtf_s00db_80c0(len);
                float score = (1.0f
                            + dir.x * (slot->mOffset.x * invLen)
                            + dir.y * (slot->mOffset.y * invLen)
                            + dir.z * (slot->mOffset.z * invLen)) * invLen;

                if (score > bestScore) {
                    bestScore = score;
                    best = i;
                }
            }

            if (best < 0) {
                mRepathTimer = 0.0f;
                mFormation.GenerateRing(1);
            } else {
                break;
            }
        }

        mInfo[object].mSlot = best;
        FormationSlot* slot = mFormation.mpSlotBegin + best;
        slot->Assign((int)object->Cast(0x017f243b));
    }
}
