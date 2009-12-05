#include "lua.h"
#include <lualib.h>
#include <lauxlib.h>
#include <stdarg.h>


lua_State * L;

void lua_init(void){
	L = luaL_newstate();
	luaL_openlibs(L);
}

void lua_run(char * filename){
	(void) luaL_dofile(L, filename);
}

void lua_destroy(void){
	lua_close(L);
}

void lua_event(char * format, ...){
	va_list ap;
	int count = 0;
	char * str;
	
	lua_getfield(L, LUA_GLOBALSINDEX, "sweetwm");
	lua_getfield(L, -1, "event");
	va_start(ap, format);
	while(*format){
		count++;
		switch(*format++){
		case 's':
			str = va_arg(ap, char *);
			if(str){
				lua_pushstring(L, str);
			} else {
				lua_pushnil(L);
			}
			break;
		case 'i': lua_pushinteger(L, va_arg(ap, lint)); break;
		case 'b': lua_pushboolean(L, va_arg(ap, int)); break;
		default: count--;
		}
	}
	va_end(ap);
	lua_call(L, count, 0);
	lua_pop(L, 1);
}


