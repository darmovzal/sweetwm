#include "sweetwm.h"
#include <lualib.h>
#include <lauxlib.h>
#include <stdarg.h>


lua_State * L;

void lua_pushxid(lua_State * L, XID xid){
	*((XID *) lua_newuserdata(L, sizeof(XID))) = xid;
	luaL_newmetatable(L, META_XID);
	lua_setmetatable(L, -2);
}

XID lua_checkxid(lua_State * L, int index){
	return *((XID *) luaL_checkudata(L, index, META_XID));
}

void script_init(void){
	L = luaL_newstate();
	luaL_openlibs(L);
}

void script_run(char * filename){
	(void) luaL_dofile(L, filename);
}

void script_destroy(void){
	lua_close(L);
}

void script_event(char * format, ...){
	va_list ap;
	int count = 0;
	char * str;
	
	lua_getfield(L, LUA_GLOBALSINDEX, "sweetwm");
	if(!lua_istable(L, -1)){
		fprintf(stderr, "Global sweetwm is not a table");
		return;
	}
	lua_getfield(L, -1, "event");
	if(!lua_isfunction(L, -1)){
		fprintf(stderr, "Global sweetwm.event is not a function");
		return;
	}
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
		case 'i':
			lua_pushinteger(L, (lua_Integer) va_arg(ap, int));
			break;
		case 'b':
			lua_pushboolean(L, va_arg(ap, int));
			break;
		case 'x':
			lua_pushxid(L, va_arg(ap, XID));
			break;
		default: count--;
		}
	}
	va_end(ap);
	lua_call(L, count, 0);
	lua_pop(L, 1);
}


