#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

//define the wordlist of valid characters to be used
const char charlist[28] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main(int argc, char * argv[]) {

	//define variables
	int i, idx;

	//seed random
	srand(time(0));

	//for each needed character, choose random from the dict
	//putchar to stdout.
	for (i = 0; i < atoi(argv[1]); ++i){

		idx = rand()%28;
		putchar(charlist[idx]);
	}

	//get that terminating newline on there
	putchar('\n');
	return 0;
}