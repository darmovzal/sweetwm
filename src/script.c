#include "sweetwm.h"
#include <lualib.h>
#include <lauxlib.h>
#include <stdarg.h>


lua_State * L;

void lua_pushwindow(lua_State * L, Window w){
	*((Window *) lua_newuserdata(L, sizeof(Window))) = w;
	luaL_newmetatable(L, META_WINDOW);
	lua_setmetatable(L, -2);
}

Window lua_checkwindow(lua_State * L, int index){
	return *((Window *) luaL_checkudata(L, index, META_WINDOW));
}

void script_init(void){
	L = luaL_newstate();
	luaL_openlibs(L);
}

void script_run(char * filename){
	char buf[1000];
	
	snprintf(buf, ASIZE(buf), "dofile('%s')", filename);
	luaL_loadstring(L, buf);
	lua_call(L, 0, LUA_MULTRET);
}

void script_destroy(void){
	lua_close(L);
}

void script_args(lua_State * L, char * format, va_list ap){
	char c, * str;
	
	while((c = *(format++))){
		switch(c){
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
		case 'w':
			lua_pushwindow(L, va_arg(ap, Window));
			break;
		default:
			luaL_error(L, "Unknown argument type `%c'", c);
		}
	}
}

void lua_fcall(lua_State * L, char * format, ...){
	va_list ap;
	
	va_start(ap, format);
	script_args(L, format, ap);
	lua_call(L, strlen(format), 0);
	va_end(ap);
}

void script_event(char * format, ...){
	va_list ap;
	
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
	script_args(L, format, ap);
	lua_call(L, strlen(format), 0);
	va_end(ap);
	lua_pop(L, 1);
}


