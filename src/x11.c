#include "x11.h"
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>

#define XA_UTF8_STRING 308
#define XA_UTF8_STRING_2 232


Display * dpy;
int loop_running;

int error_handler(Display * dpy, XErrorEvent * eev){
	int code;
	char msg[1000];
	
	code = (int) eev->error_code;
	XGetErrorText(dpy, code, msg, ASIZE(msg));
	lua_event("ssi", "error", msg, code);
	return 0;
}

void x_init(void){
	dpy = XOpenDisplay(NULL);
	if(!dpy) perror("display");
	XSetErrorHandler(error_handler);
}

void x_destroy(void){
	XCloseDisplay(dpy);
}

void x_event(XEvent * ev){
	switch(ev.type){
#define BEVENT(C, N, T) case C: lua_event("ss" T, "X", N
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

void x_loop(void){
	XEvent ev;
	
	loop_running = 1;
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | SubstructureNotifyMask | StructureNotifyMask);
	while(loop_running){
		XNextEvent(dpy, &ev);
		x_event(&ev);
	}
}

void x_stop(void){
	loop_running = 0;
}


