#include "lua.h"
#include "x11.h"

#define WMEVENT(T, F, V) lua_event("sss" T, "window", "wmhint", F, V);
#define SHEVENT(X) lua_event("sssi", "window", "sizehint", #X, sh->X)
#define PEVENT(T, V) lua_event("sss" T, "window", "property", sname, V)
#define AEVENT(T, N, V) lua_event("sss" T, "window", "attribute", N, V)
#define AEVENTI(F) AEVENT("i", #F, (int) attributes.F)
#define LFUNC(N) int lua_##N(lua_State * L)
#define REG(N) lua_pushcfunction(L, lua_##N); lua_setfield(L, -2, #N);


LFUNC(move_window){
	Window w;
	int x, y;
	
	w = (Window) luaL_checkinteger(L, 1);
	x = luaL_checkint(L, 2);
	y = luaL_checkint(L, 3);
	lua_pop(L, 3);
	XMoveWindow(dpy, w, x, y);
	return 0;
}

LFUNC(resize_window){
	Window w;
	int width, height;
	
	w = (Window) luaL_checkinteger(L, 1);
	width = luaL_checkint(L, 2);
	height = luaL_checkint(L, 3);
	lua_pop(L, 3);
	XResizeWindow(dpy, w, width, height);
	return 0;
}

LFUNC(set_border_width){
	Window w;
	int width;
	
	w = (Window) luaL_checkinteger(L, 1);
	width = luaL_checkint(L, 2);
	XSetWindowBorderWidth(dpy, w, width);
	return 0;
}

LFUNC(query_tree){
	Window w, root, parent, * children;
	unsigned int childcount;
	int i;
	
	w = (Window) luaL_checkinteger(L, 1);
	XQueryTree(dpy, w, &root, &parent, &children, &childcount);
	lua_pushinteger(L, (lint) root);
	lua_pushinteger(L, (lint) parent);
	lua_newtable(L);
	for(i = 0; i < childcount; i++){
		lua_pushinteger(L, i + 1);
		lua_pushinteger(L, (lint) children[i]);
		lua_settable(L, -3);
	}
	XFree(children);
	return 3;
}

LFUNC(root){
	lua_pushinteger(L, (lint) DefaultRootWindow(dpy));
	return 1;
}

LFUNC(map_window){
	Window w;
	
	w = (Window) luaL_checkinteger(L, 1);
	XMapWindow(dpy, w);
	return 0;
}

void process_wm_hints(Window w){
	XWMHints * wmh;
	long f;
	
	wmh = XGetWMHints(dpy, w);
	if(!wmh) return;
	f = wmh->flags;
	if(f & InputHint) WMEVENT("b", "input", wmh->input == True ? 1 : 0);
	if(f & StateHint) WMEVENT("i", "initial_state", (int) wmh->initial_state);
	XFree(wmh);
}

void process_size_hints(Window w){
	XSizeHints * sh;
	long mask = 0;
	
	sh = XAllocSizeHints();
	if(!XGetWMNormalHints(dpy, w, sh, &mask)){
		XFree(sh);
		return;
	}
	if(mask & PMinSize){
		SHEVENT(min_width);
		SHEVENT(min_height);
	}
	if(mask & PMaxSize){
		SHEVENT(max_width);
		SHEVENT(max_height);
	}
	if(mask & PResizeInc){
		SHEVENT(width_inc);
		SHEVENT(height_inc);
	}
	if(mask & PAspect){
		SHEVENT(min_aspect.x);
		SHEVENT(min_aspect.y);
		SHEVENT(max_aspect.x);
		SHEVENT(max_aspect.y);
	}
	if(mask & PBaseSize){
		SHEVENT(base_width);
		SHEVENT(base_height);
	}
	if(mask & PWinGravity) SHEVENT(win_gravity);
	XFree(sh);
}

void process_property(Window w, Atom name){
	Atom type;
	int propformat;
	unsigned long propitemcount, propbytesafter;
	unsigned char * value;
	char * sname, * aname;
	
	XGetWindowProperty(dpy, w, name, 0, 1000, False, AnyPropertyType, &type, &propformat, &propitemcount, &propbytesafter, &value);
	sname = XGetAtomName(dpy, name);
	switch(type){
	case XA_STRING:
	case XA_UTF8_STRING:
	case XA_UTF8_STRING_2:
		PEVENT("s", value);
		break;
	case XA_CARDINAL:
	case XA_WINDOW:
		PEVENT("i", (int)(*((Cardinal *) value)));
		break;
	case XA_INTEGER:
		PEVENT("i", *((int *) value));
		break;
	case XA_ATOM:
		aname = XGetAtomName(dpy, *((Atom *) value));
		PEVENT("s", aname);
		XFree(aname);
		break;
	case XA_WM_HINTS:
	case XA_WM_NORMAL_HINTS:
	case XA_WM_SIZE_HINTS:
		break;
	default:
		PEVENT("s", NULL);
	}
	XFree(sname);
	XFree(value);
}

void process_properties(Window w){
	int propcount, i;
	Atom * propnames;
	
	propnames = XListProperties(dpy, w, &propcount);
	for(i = 0; i < propcount; i++)
		process_property(w, propnames[i]);
	XFree(propnames);
}

void process_attributes(Window w){
	XWindowAttributes attributes;
	
	XGetWindowAttributes(dpy, w, &attributes);
	AEVENTI(width);
	AEVENTI(height);
	AEVENTI(x);
	AEVENTI(y);
	AEVENTI(border_width);
	AEVENTI(depth);
	AEVENTI(root);
	AEVENT("s", "class", attributes.class == InputOutput ? "InputOutput" : "Input");
	AEVENTI(bit_gravity);
	AEVENTI(win_gravity);
}

void process_window(Window w){
	lua_event("si", "window_begin", (int) w);
	process_attributes(w);
	process_properties(w);
	process_wm_hints(w);
	process_size_hints(w);
	lua_event("s", "window_end");
}

LFUNC(process_window){
	Window w;
	
	w = (Window) luaL_checkinteger(L, 1);
	process_window(w);
	return 0;
}

void func_reg(void){
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_GLOBALSINDEX, "sweetwm");
	REG(move_window)
	REG(resize_window)
	REG(set_border_width)
	REG(query_tree)
	REG(root)
	REG(map_window)
	REG(process_window)
	lua_pop(L, 1);
}


