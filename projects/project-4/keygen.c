#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

const char charlist[28] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main(int argc, char * argv[]) {

	int i, idx;

	srand(time(0));

	for (i = 0; i < atoi(argv[1]); ++i){

		idx = rand()%28;
		putchar(charlist[idx]);
	}

	putchar('\n');
	return 0;
}