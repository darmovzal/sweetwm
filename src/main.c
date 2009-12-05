//#include <stdlib.h>
#include <stdio.h>


int main(int argc, char ** argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <handler.lua>\n", argv[0]);
		fflush(stderr);
		exit(1);
	}
	lua_init();
	x_init();
	func_reg();
	lua_run(argv[1]);
	x_loop();
	x_destroy();
	lua_destroy();
	return 0;
}


