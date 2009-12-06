#ifndef _SWEETWM_SCRIPT_H_
#define _SWEETWM_SCRIPT_H_


void script_init(void);
void script_run(char * filename);
void script_event(char * format, ...);
void script_destroy(void);


#endif


