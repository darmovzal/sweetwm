#ifndef _SWEETWM_X11_H_
#define _SWEETWM_X11_H_

#include <X11/Xlib.h>


extern Display * dpy;

void x_init(void);
void x_destroy(void);
void x_loop(void);


#endif


