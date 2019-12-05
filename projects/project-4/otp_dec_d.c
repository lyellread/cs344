#include "otp_core.h"

int main(int argc, char ** argv){

	//check args
	if (argc != 2){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	//run the server
	otp_d(atoi(argv[1]), 0);
	exit(0);
}