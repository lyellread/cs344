#include "otp_core.h"

int main(int argc, char ** argv){

	//check args
	if (argc != 2){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	//start server
	otp_d(atoi(argv[1]), 1);
	exit(0);
}