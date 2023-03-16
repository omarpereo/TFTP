/********************************************************************************
* This code works like a client/server app
* sending a file under TFTP using UDP
*
*
*	O. Pereo
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/lib_app_tftp.h"


int main(int argc, char * argv[]) {

	int current_mode = 0;														// Mode selected
	int runtime_error = 0;

	current_mode = obtaing_mode_app(argc,argv);

	if(current_mode == SERVER) {
		printf("\nInit server mode");
		init_server_mode();
	} else if(current_mode == CLIENT) {
		printf("\nInit client mode\nsetting conection......");		
		init_client_mode(argv[2],argv[3]);									// argv[2] is Filename, argv[3] is IP_Server
	} else {
		printf("\nNo mode selected");
	}


	
	return 0;

}






