#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include "otp_core.h"

int main(int argc, char * argv[]) {

	//define variables
	int i, idx;

	//check for there being 1 arg
	if (argc != 2){
		fprintf(stderr, "Error: Improper Arguments Provided. Quitting Keygen.\n");
		exit(3);
	}

	//seed random
	srand(time(0));

	//for each needed character, choose random from the dict
	//putchar to stdout.
	for (i = 0; i < atoi(argv[1]); ++i){

		idx = rand()%(charlist_len);
		putchar(charlist[idx]);
	}

	//get that terminating newline on there
	putchar('\n');
	return 0;
}