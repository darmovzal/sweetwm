#include "sweetwm.h"


int main(int argc, char ** argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <handler.lua>\n", argv[0]);
		fflush(stderr);
		exit(1);
	}
	script_init();
	x11_init();
	func_reg();
	script_run(argv[1]);
	x11_loop();
	x11_destroy();
	script_destroy();
	return 0;
}


