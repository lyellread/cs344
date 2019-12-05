#include "otp_core.h"

int main(int argc, char ** argv){

	if (argc != 2){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	otp_d(atoi(argv[1]), 1);
	exit(0);
}