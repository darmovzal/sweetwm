#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <lua.h>

#define ASIZE(X) (sizeof(X) / sizeof((X)[0]))
#define META_WINDOW "window"

#define lua_dup(L) lua_pushvalue((L), -1)


extern Display * dpy;
extern lua_State * L;

void lua_pushwindow(lua_State * L, Window window);
Window lua_checkwindow(lua_State * L, int index);
void lua_fcall(lua_State * L, char * format, ...);

void script_init(void);
void script_run(char * filename);
void script_event(char * format, ...);
void script_destroy(void);

void x11_init(void);
void x11_destroy(void);
void x11_loop(void);

void func_reg(void);


