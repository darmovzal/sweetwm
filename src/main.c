#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define XA_UTF8_STRING 308
#define XA_UTF8_STRING_2 232

#define ASIZE(X) (sizeof(X) / sizeof((X)[0]))


typedef lua_Integer lint;

Display * dpy;
lua_State * L;

void event(char * format, ...){
	va_list ap;
	int count = 0;
	char * str;
	
	lua_getfield(L, LUA_GLOBALSINDEX, "wm");
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

void process_wm_hints(Window w){
	XWMHints * wmh;
	long f;
	
	wmh = XGetWMHints(dpy, w);
	if(!wmh) return;
	f = wmh->flags;
#define WMEVENT(T, F, V) event("ss" T, "window_wmhint", F, V);
	if(f & InputHint) WMEVENT("b", "input", wmh->input == True ? 1 : 0);
	if(f & StateHint) WMEVENT("i", "initial_state", (int) wmh->initial_state);
#undef WMEVENT
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
#define SHEVENT(X) event("ssi", "window_sizehint", #X, sh->X)
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
#undef SHEVENT
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
#define PEVENT(T, V) event("ss" T, "window_property", sname, V)
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
#undef PEVENT
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
#define AEVENT(T, N, V) event("ss" T, "window_attribute", N, V)
#define AEVENTI(F) AEVENT("i", #F, (int) attributes.F)
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
#undef AEVENTI
#undef AEVENT
}

void process_window(Window w){
	event("si", "window_begin", (int) w);
	process_attributes(w);
	process_properties(w);
	process_wm_hints(w);
	process_size_hints(w);
	event("s", "window_end");
}

void loop(){
	XEvent ev;
	
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | SubstructureNotifyMask | StructureNotifyMask);
	for(;;){
		XNextEvent(dpy, &ev);
		switch(ev.type){
#define BEVENT(C, N, T) case C: event("ss" T, "X", N
#define EEVENT ); break;
#define EVENT(C, N, T) EEVENT BEVENT(C, N, T)
		BEVENT(KeyPress, "key_press", "i")
			,ev.xkey.keycode
		EVENT(KeyRelease, "key_release", "i")
			,ev.xkey.keycode
		EVENT(EnterNotify, "enter_notify", "")
		EVENT(LeaveNotify, "leave_notify", "")
		EVENT(MapNotify, "map_notify", "")
		EVENT(UnmapNotify, "unmap_notify", "")
		EVENT(FocusIn, "focus_in", "")
		EVENT(FocusOut, "focus_out", "")
		EVENT(CreateNotify, "create_notify", "")
		EVENT(DestroyNotify, "destroy_notify", "")
		EVENT(MapRequest, "map_request", "i")
			,(lint) ev.xmaprequest.window
		EVENT(ReparentNotify, "reparent_notify", "")
		EVENT(ConfigureRequest, "configure_request", "iiiii")
			,ev.xconfigurerequest.window
			,ev.xconfigurerequest.x
			,ev.xconfigurerequest.y
			,ev.xconfigurerequest.width
			,ev.xconfigurerequest.height
		EVENT(ConfigureNotify, "configure_notify", "")
		EEVENT
#undef BEVENT
#undef EEVENT
#undef EVENT
		}
	}
}

int error_handler(Display * dpy, XErrorEvent * eev){
	int code;
	char msg[1000];
	
	code = (int) eev->error_code;
	XGetErrorText(dpy, code, msg, ASIZE(msg));
	event("ssi", "error", msg, code);
	return 0;
}

#define LFUNC(N) int lua_##N(lua_State * L)

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

LFUNC(process_window){
	Window w;
	
	w = (Window) luaL_checkinteger(L, 1);
	process_window(w);
	return 0;
}

#undef LFUNC

void register_functions(void){
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_GLOBALSINDEX, "wm");
#define REG(N) lua_pushcfunction(L, lua_##N); lua_setfield(L, -2, #N);
	REG(move_window)
	REG(resize_window)
	REG(set_border_width)
	REG(query_tree)
	REG(root)
	REG(map_window)
	REG(process_window)
#undef REG
	lua_pop(L, 1);
}

int main(int argc, char ** argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <handler.lua>\n", argv[0]);
		fflush(stderr);
		exit(1);
	}
	L = luaL_newstate();
	luaL_openlibs(L);
	register_functions();
	dpy = XOpenDisplay(NULL);
	if(!dpy) perror("display");
	XSetErrorHandler(error_handler);
	(void) luaL_dofile(L, argv[1]);
	loop(dpy);
	XCloseDisplay(dpy);
	lua_close(L);
	return 0;
}


