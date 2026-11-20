// @ 0x00c19ee0  FUN_00c19ee0
//
// Flags: /O2 /MD /Gy /EHsc /TP /arch:SSE /fp:fast
//
// Creature behaviour attempt handler: resolves the creature model component,
// checks that the target creature is still usable, and steers the model at the
// creature (or at a body part / bounding-box centre of the target's body).
//
// Retail's control flow keeps its single "return false" epilogue at the end of
// the function and shares one "return true" epilogue with the done block, so
// the guards are written with a forward goto and the body is nested under the
// e070 check.  The doubled holder3c test (cmp [ctx+0x3c],0 then a reload) needs
// the compiler's memory barrier between the two reads.
#include <intrin.h>
#include "types.h"

struct Vec3 { float x, y, z; };
struct Box  { Vec3 min; Vec3 max; };
struct Vtbl { void* mSlots[0x120 / 4]; };
#define VSLOT(o, n) (((Vtbl*)*(void**)(o))->mSlots[(n) / 4])
typedef void* (__thiscall *VFn_pi)(void*, int);
typedef char  (__thiscall *VFn_c)(void*);
typedef void* (__thiscall *VFn_pp)(void*, void*);
struct AnimGoal { char pad_000[0xa0]; float field_a0; };
struct AnimCreature {
    char  pad_000[0x184];
    void* goalTable;
    void  __thiscall SetTargetBody(void* target);
};
struct CCM {
    char  pad_000[0xb54];
    AnimCreature* anim;
    char  pad_b58[0xb66 - 0xb58];
    bool  flag_b66;
    char  __thiscall e2c0(int);
    bool  __thiscall e070(int);
    void  __thiscall d1e0(const Vec3*);
    void  __thiscall d200(int, int);
};
struct Creature { unsigned __thiscall GetBodyPartIndex(unsigned, unsigned); };
struct CreatureBody { char pad_000[0x75]; bool alive; };
struct ActionCtx {
    void* creature;
    int   done;
    int   bodyPartIndex;
    int   pad_0c;
    Vec3  targetPos;
    int   pad_1c;
    int   pad_20;
    int   pad_24;
    int   pad_28;
    int   pad_2c;
    int   pad_30;
    int   pad_34;
    int   pad_38;
    void* holder3c;
    void* holder40;
};
extern float DAT_013eb1bc;
extern void* __cdecl FUN_009ff6f0(void*, unsigned int);
extern void* __cdecl FUN_00f19200(void*);
extern int   __cdecl FUN_00b2a390(void*);
extern void* __cdecl FUN_00b18e00(void*);
extern void* __cdecl FUN_00b18700(void*);
extern void* __cdecl FUN_00b676e0(void*);
extern void* __cdecl FUN_00c0c340(void*);
extern void* __cdecl FUN_00ae6760(void*);
extern void* __stdcall FUN_00c3e790(int);
extern void* __cdecl FUN_00d9a4a0(Vec3*, void*, void*);

bool __cdecl FUN_00c19ee0(ActionCtx* ctx)
{
    if (!ctx->holder3c)
        goto fail;
    _ReadWriteBarrier();
    CCM* model = ctx->holder3c
               ? (CCM*)((VFn_pi)VSLOT(ctx->holder3c, 0x0c))(ctx->holder3c, 0xce9f6639)
               : 0;
    void* other = ctx->holder40
                ? ((VFn_pi)VSLOT(ctx->holder40, 0x0c))(ctx->holder40, 0x017f243b)
                : 0;
    if (model) {
        if (!other || !((VFn_c)VSLOT(other, 0x2c))(other)) {
            void* creature = ctx->creature;
            if (creature) {
                if (model->e2c0((int)creature)) {
                    AnimCreature* anim = model->anim;
                    AnimGoal* goal = (AnimGoal*)FUN_009ff6f0(anim->goalTable, (unsigned int)creature);
                    if (!goal)
                        return false;
                    if (goal->field_a0 != DAT_013eb1bc)
                        return false;
                }
                if (model->e070((int)creature)) {
                    if (!other) {
                        model->d1e0(&ctx->targetPos);
                    } else {
                        void* cr   = FUN_00f19200(other);
                        int   part = FUN_00b2a390(other);
                        void* body = FUN_00b18e00(other);
                        if (!body)
                            return false;
                        if (!((CreatureBody*)body)->alive)
                            return false;

                        if (cr) {
                            if (ctx->bodyPartIndex == -1)
                                ctx->bodyPartIndex = (int)((Creature*)cr)->GetBodyPartIndex(2, 0);
                            model->d200((int)cr, ctx->bodyPartIndex);
                        } else if (part) {
                            AnimCreature* anim = model->anim;
                            if (anim)
                                anim->SetTargetBody(FUN_00c3e790(ctx->bodyPartIndex));
                        } else {
                            Vec3 out;
                            void* a = FUN_00b18700(other);
                            void* b = FUN_00b676e0(other);
                            void* c = FUN_00c0c340(other);
                            void* d = FUN_00ae6760(other);
                            if (a || b || c || d) {
                                FUN_00d9a4a0(&out, model, body);
                            } else {
                                Box box;
                                Box* dst = (Box*)((VFn_pp)VSLOT(body, 0x6c))(body, &box);
                                out.x = (dst->min.x + dst->max.x) * 0.5f;
                                out.y = (dst->max.y + dst->min.y) * 0.5f;
                                out.z = (dst->max.z + dst->min.z) * 0.5f;
                            }
                            model->d1e0(&out);
                        }
                    }
                    ctx->done = 1;
                    model->flag_b66 = true;
                }
                return true;
            }
        }
    }
fail:
    return false;
}
