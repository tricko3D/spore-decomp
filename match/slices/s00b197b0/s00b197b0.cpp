// @ 0x00b197b0
#include "game/types.h"

struct IObj;
struct IRootB {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12();
  virtual IObj* Get34();              // +0x34
  virtual void v14();
  virtual void Get3c(void* out, int z);  // +0x3c
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24();
  virtual int Get64(unsigned id);     // +0x64
};
extern IRootB* __cdecl GetRoot();     // 0x00b3d240
extern void __cdecl FUN_00b3d230();   // 0x00b3d230

struct IObj {
  virtual void o0(); virtual void o1(); virtual void o2();
  virtual IObj* Query(unsigned id);   // +0x0c
  virtual void o4(); virtual void o5(); virtual void o6(); virtual void o7();
  virtual void o8(); virtual void o9(); virtual void o10();
  virtual bool Is();                  // +0x2c
  virtual void o12();
  int GetObjId(int arg);              // 0x00b18e70
};

// Input event sink; dispatch occupies vtable slot 12.
struct cInput {
    virtual void c00(); virtual void c04(); virtual void c08(); virtual void c0c();
    virtual void c10(); virtual void c14(); virtual void c18(); virtual void c1c();
    virtual void c20(); virtual void c24(); virtual void c28(); virtual void c2c();
    virtual void Fn30(int type, int id, void* object, void* position, int flags);
    bool F197b0(int a, int b, int c, int d);
};

// @ 0x00b197b0
bool cInput::F197b0(int a, int b, int c, int d) {
    IObj* object = GetRoot()->Get34();
    float position[3];
    GetRoot()->Get3c(position, 0);
    Fn30(8, b, object, position, object->GetObjId(d));
    return false;
}
