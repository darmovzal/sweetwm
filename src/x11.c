#include "common.h"
#include "x11.h"
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>


typedef int scint;
void script_event(char * format, ...);


Display * dpy;
int loop_running;

int error_handler(Display * dpy, XErrorEvent * eev){
	int code;
	char msg[1000];
	
	code = (int) eev->error_code;
	XGetErrorText(dpy, code, msg, ASIZE(msg));
	script_event("ssi", "error", msg, code);
	return 0;
}

void x11_init(void){
	dpy = XOpenDisplay(NULL);
	if(!dpy) perror("display");
	XSetErrorHandler(error_handler);
}

void x11_destroy(void){
	XCloseDisplay(dpy);
}

void x11_event(XEvent * ev){
	switch(ev->type){
#define BEVENT(C, N, T) case C: script_event("ss" T, "xevent", N
#define EEVENT ); break;
#define EVENT(C, N, T) EEVENT BEVENT(C, N, T)
	BEVENT(KeyPress, "key_press", "i")
		,(scint) ev->xkey.keycode
	EVENT(KeyRelease, "key_release", "i")
		,(scint) ev->xkey.keycode
	EVENT(EnterNotify, "enter_notify", "")
	EVENT(LeaveNotify, "leave_notify", "")
	EVENT(MapNotify, "map_notify", "")
	EVENT(UnmapNotify, "unmap_notify", "")
	EVENT(FocusIn, "focus_in", "")
	EVENT(FocusOut, "focus_out", "")
	EVENT(CreateNotify, "create_notify", "")
	EVENT(DestroyNotify, "destroy_notify", "")
	EVENT(MapRequest, "map_request", "i")
		,(scint) ev->xmaprequest.window
	EVENT(ReparentNotify, "reparent_notify", "")
	EVENT(ConfigureRequest, "configure_request", "iiiii")
		,(scint) ev->xconfigurerequest.window
		,(scint) ev->xconfigurerequest.x
		,(scint) ev->xconfigurerequest.y
		,(scint) ev->xconfigurerequest.width
		,(scint) ev->xconfigurerequest.height
	EVENT(ConfigureNotify, "configure_notify", "")
	EEVENT
#undef BEVENT
#undef EEVENT
#undef EVENT
	}
}

void x11_loop(void){
	XEvent ev;
	
	loop_running = 1;
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | SubstructureNotifyMask | StructureNotifyMask);
	while(loop_running){
		XNextEvent(dpy, &ev);
		x11_event(&ev);
	}
}

void x11_stop(void){
	loop_running = 0;
}


