#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdarg.h>


typedef lua_Integer scint;

lua_State * L;

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
		case 'i': lua_pushinteger(L, (lua_Integer) va_arg(ap, scint)); break;
		case 'b': lua_pushboolean(L, va_arg(ap, int)); break;
		default: count--;
		}
	}
	va_end(ap);
	lua_call(L, count, 0);
	lua_pop(L, 1);
}


