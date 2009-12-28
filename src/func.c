#include "sweetwm.h"
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <lauxlib.h>

#define XA_UTF8_STRING 308
#define XA_UTF8_STRING_2 232

#define LFUNC(N) int lua_##N(lua_State * L)


Screen * get_screen(int num){
	Screen * ret;
	
	ret = XScreenOfDisplay(dpy, num);
	if(!ret) luaL_error(L, "No screen of number %d", num);
	return ret;
}

LFUNC(move){
	Window w;
	int x, y;
	
	w = lua_checkwindow(L, 1);
	x = luaL_checkint(L, 2);
	y = luaL_checkint(L, 3);
	lua_pop(L, 3);
	XMoveWindow(dpy, w, x, y);
	return 0;
}

LFUNC(resize){
	Window w;
	int width, height;
	
	w = lua_checkwindow(L, 1);
	width = luaL_checkint(L, 2);
	height = luaL_checkint(L, 3);
	lua_pop(L, 3);
	XResizeWindow(dpy, w, width, height);
	return 0;
}

LFUNC(set_border_width){
	Window w;
	int width;
	
	w = lua_checkwindow(L, 1);
	width = luaL_checkint(L, 2);
	XSetWindowBorderWidth(dpy, w, width);
	return 0;
}

double clamp(double d){
	if(d < 0) return 0;
	if(d > 1) return 1;
	return d;
}

int get_pixel(unsigned long * ret, const char * name, double r, double g, double b){
	XColor color;
	Colormap cmap;
	
	cmap = DefaultColormap(dpy, 0);
	if(name){
		if(!XParseColor(dpy, cmap, name, &color)) return 0;
	} else {
#define SCALE(V) ((unsigned short)(clamp(V) * 65535))
		color.red = SCALE(r);
		color.green = SCALE(g);
		color.blue = SCALE(b);
#undef SCALE
	}
	if(!XAllocColor(dpy, cmap, &color)) return 0;
	*ret = color.pixel;
	return 1;
}

LFUNC(set_border_color){
	Window w;
	const char * name = NULL;
	double r, g, b;
	int ok;
	unsigned long pixel;
	
	w = lua_checkwindow(L, 1);
	luaL_checkany(L, 2);
	if(lua_type(L, 2) == LUA_TSTRING){
		name = luaL_checkstring(L, 2);
	} else {
		r = luaL_checknumber(L, 2);
		g = luaL_checknumber(L, 3);
		b = luaL_checknumber(L, 4);
	}
	if((ok = get_pixel(&pixel, name, r, g, b)))
		XSetWindowBorder(dpy, w, pixel);
	lua_pushboolean(L, ok);
	return 1;
}

LFUNC(query_tree){
	Window w, root, parent, * children;
	unsigned int childcount;
	int i;
	
	w = lua_checkwindow(L, 1);
	XQueryTree(dpy, w, &root, &parent, &children, &childcount);
	lua_pushwindow(L, root);
	lua_pushwindow(L, parent);
	lua_newtable(L);
	for(i = 0; i < childcount; i++){
		lua_pushinteger(L, i + 1);
		lua_pushwindow(L, children[i]);
		lua_settable(L, -3);
	}
	XFree(children);
	return 3;
}

LFUNC(get_screen_count){
	lua_pushinteger(L, ScreenCount(dpy));
	return 1;
}

LFUNC(get_root_window){
	int screen;
	
	screen = luaL_checkint(L, 1);
	lua_pushwindow(L, RootWindow(dpy, screen));
	return 1;
}

LFUNC(map){
	Window w;
	
	w = lua_checkwindow(L, 1);
	XMapWindow(dpy, w);
	return 0;
}

void wm_hints(Window w){
	XWMHints * wmh;
	long f;
	
	wmh = XGetWMHints(dpy, w);
	if(!wmh) return;
	f = wmh->flags;
#define WMCALL(T, F, V) lua_dup(L); lua_fcall(L, "s" T, F, V);
	if(f & InputHint) WMCALL("b", "input", wmh->input == True ? 1 : 0);
	if(f & StateHint) WMCALL("i", "initial_state", (int) wmh->initial_state);
#undef WMCALL
	XFree(wmh);
}

LFUNC(wm_hints){
	Window w;
	
	w = lua_checkwindow(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	wm_hints(w);
	lua_pop(L, 2);
	return 0;
}

void size_hints(Window w){
	XSizeHints * sh;
	long mask = 0;
	
	sh = XAllocSizeHints();
	if(!XGetWMNormalHints(dpy, w, sh, &mask)){
		XFree(sh);
		return;
	}
#define SHCALL(X) lua_dup(L); lua_fcall(L, "si", #X, sh->X)
	if(mask & PMinSize){
		SHCALL(min_width);
		SHCALL(min_height);
	}
	if(mask & PMaxSize){
		SHCALL(max_width);
		SHCALL(max_height);
	}
	if(mask & PResizeInc){
		SHCALL(width_inc);
		SHCALL(height_inc);
	}
	if(mask & PAspect){
		SHCALL(min_aspect.x);
		SHCALL(min_aspect.y);
		SHCALL(max_aspect.x);
		SHCALL(max_aspect.y);
	}
	if(mask & PBaseSize){
		SHCALL(base_width);
		SHCALL(base_height);
	}
	if(mask & PWinGravity) SHCALL(win_gravity);
#undef SHCALL
	XFree(sh);
}

LFUNC(size_hints){
	Window w;
	
	w = lua_checkwindow(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	size_hints(w);
	lua_pop(L, 2);
	return 0;
}

void property(Window w, Atom name){
	Atom type;
	int propformat;
	unsigned long propitemcount, propbytesafter;
	unsigned char * value;
	char * sname, * aname;
	
	XGetWindowProperty(dpy, w, name, 0, 1000, False, AnyPropertyType, &type, &propformat, &propitemcount, &propbytesafter, &value);
	sname = XGetAtomName(dpy, name);
#define PCALL(T, V) lua_dup(L); lua_fcall(L, "s" T, sname, V)
	switch(type){
	case XA_STRING:
	case XA_UTF8_STRING:
	case XA_UTF8_STRING_2:
		PCALL("s", value);
		break;
	case XA_WINDOW:
		PCALL("w", *((Window *) value));
		break;
	case XA_CARDINAL:
		PCALL("i", (int)(*((Cardinal *) value)));
		break;
	case XA_INTEGER:
		PCALL("i", *((int *) value));
		break;
	case XA_ATOM:
		aname = XGetAtomName(dpy, *((Atom *) value));
		PCALL("s", aname);
		XFree(aname);
		break;
	case XA_WM_HINTS:
	case XA_WM_NORMAL_HINTS:
	case XA_WM_SIZE_HINTS:
		break;
	default:
		PCALL("s", NULL);
	}
#undef PCALL
	XFree(sname);
	XFree(value);
}

void properties(Window w){
	int propcount, i;
	Atom * propnames;
	
	propnames = XListProperties(dpy, w, &propcount);
	for(i = 0; i < propcount; i++)
		property(w, propnames[i]);
	XFree(propnames);
}

LFUNC(properties){
	Window w;
	
	w = lua_checkwindow(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	properties(w);
	lua_pop(L, 2);
	return 0;
}

void attributes(Window w){
	XWindowAttributes attributes;
	
	XGetWindowAttributes(dpy, w, &attributes);
#define ACALL(T, N, V) lua_dup(L); lua_fcall(L, "s" T, N, V)
#define ACALLI(F) ACALL("i", #F, (int) attributes.F)
	ACALLI(width);
	ACALLI(height);
	ACALLI(x);
	ACALLI(y);
	ACALLI(border_width);
	ACALLI(depth);
	ACALL("w", "root", attributes.root);
	ACALL("s", "class", attributes.class == InputOutput ? "InputOutput" : "Input");
	ACALLI(bit_gravity);
	ACALLI(win_gravity);
#undef ACALL
#undef ACALLI
}

LFUNC(attributes){
	Window w;
	
	w = lua_checkwindow(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	attributes(w);
	lua_pop(L, 2);
	return 0;
}

LFUNC(get_screen_size){
	Screen * screen;
	
	screen = get_screen(luaL_checkint(L, 1));
	lua_pushinteger(L, XWidthOfScreen(screen));
	lua_pushinteger(L, XHeightOfScreen(screen));
	lua_pushinteger(L, XWidthMMOfScreen(screen));
	lua_pushinteger(L, XHeightMMOfScreen(screen));
	return 4;
}

void func_reg(void){
#define REG(N) lua_pushcfunction(L, lua_##N); lua_setfield(L, -2, #N);
	lua_newtable(L);
	REG(get_screen_count)
	REG(get_root_window)
	REG(get_screen_size)
	lua_setfield(L, LUA_GLOBALSINDEX, "sweetwm");
	
	luaL_newmetatable(L, META_WINDOW);
	lua_newtable(L);
	REG(move)
	REG(resize)
	REG(set_border_width)
	REG(set_border_color)
	REG(query_tree)
	REG(map)
	REG(attributes)
	REG(properties)
	REG(wm_hints)
	REG(size_hints)
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
#undef REG
}


