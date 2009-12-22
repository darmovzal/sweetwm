#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <lua.h>

#define ASIZE(X) (sizeof(X) / sizeof((X)[0]))
#define META_XID "xid"


extern Display * dpy;
extern lua_State * L;

void script_init(void);
void script_run(char * filename);
void script_event(char * format, ...);
void script_destroy(void);

void x11_init(void);
void x11_destroy(void);
void x11_loop(void);

void func_reg(void);

void lua_pushxid(lua_State * L, XID xid);
XID lua_checkxid(lua_State * L, int index);


