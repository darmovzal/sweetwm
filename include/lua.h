#ifndef _SWEETWM_LUA_H_
#define _SWEETWM_LUA_H_

#include <lua.h>


extern lua_State * L;
typedef lua_Integer lint;

void lua_init(void);
void lua_run(char * filename);
void lua_event(char * format, ...);
void lua_destroy(void);


#endif


