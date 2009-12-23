#include "sweetwm.h"


Display * dpy;
int loop_running;

int error_handler(Display * dpy, XErrorEvent * eev){
	int code;
	char msg[1000];
	
	code = (int) eev->error_code;
	XGetErrorText(dpy, code, msg, ASIZE(msg));
	script_event("ssi", "xerror", msg, code);
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
		,(int) ev->xkey.keycode
	EVENT(KeyRelease, "key_release", "i")
		,(int) ev->xkey.keycode
	EVENT(EnterNotify, "enter_notify", "w")
		,ev->xcrossing.window
	EVENT(LeaveNotify, "leave_notify", "w")
		,ev->xcrossing.window
	EVENT(MapNotify, "map_notify", "")
	EVENT(UnmapNotify, "unmap_notify", "")
	EVENT(FocusIn, "focus_in", "")
	EVENT(FocusOut, "focus_out", "")
	EVENT(CreateNotify, "create_notify", "")
	EVENT(DestroyNotify, "destroy_notify", "")
	EVENT(MapRequest, "map_request", "w")
		,ev->xmaprequest.window
	EVENT(ReparentNotify, "reparent_notify", "")
	EVENT(ConfigureRequest, "configure_request", "wiiii")
		,ev->xconfigurerequest.window
		,(int) ev->xconfigurerequest.x
		,(int) ev->xconfigurerequest.y
		,(int) ev->xconfigurerequest.width
		,(int) ev->xconfigurerequest.height
	EVENT(ConfigureNotify, "configure_notify", "")
	EEVENT
#undef BEVENT
#undef EEVENT
#undef EVENT
	}
}

void x11_loop(void){
	XEvent ev;
	long mask = 0;
	
	loop_running = 1;
	mask |= SubstructureRedirectMask;
	mask |= SubstructureNotifyMask;
	mask |= StructureNotifyMask;
	mask |= FocusChangeMask;
	mask |= KeyPressMask | KeyReleaseMask;
	mask |= EnterWindowMask | LeaveWindowMask;
	mask |= ButtonPressMask | ButtonReleaseMask;
	mask |= PointerMotionMask | Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask | ButtonMotionMask;
	mask |= ExposureMask;
	mask |= ResizeRedirectMask;
	mask |= PropertyChangeMask;
	XSelectInput(dpy, DefaultRootWindow(dpy), mask);
	while(loop_running){
		XNextEvent(dpy, &ev);
		x11_event(&ev);
	}
}

void x11_stop(void){
	loop_running = 0;
}


