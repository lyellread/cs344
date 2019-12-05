//top level otp program - performs enc, dec 'primitives'

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "otp_core.h"

int get_index_of_char (char a){

	int j;

	for (j = 0; j < charlist_len; ++j){

		if (charlist[j] == a){

			return j;
		}
	}

	return -1;
}

void otp_encrypt (char * pt, char * k, char * ct, int len){

	int i;
	int current;

	for (i = 0; i < len; ++i){

		//for each letter, perform (int)k[i] + (int)pt[i] and mod it by the length of the character
		//list + 1 so that if 27%28 = 27; 28%28 (oob) = 0 (ib).

		current = (get_index_of_char(pt[i]) + get_index_of_char(k[i])) % (charlist_len-1);
		//printf("--[enc]--> PT[i]: [%d]; K[i]: [%d]; CT[i]: [%d]; i = %d.\n", pt[i], k[i], current, i);
		//printf("->%d<-\n", current);
		ct[i] = (char)charlist[current];
	}
}

void otp_decrypt (char * ct, char * k, char * pt, int len){

	int i;
	int current;

	for (i = 0; i < len; ++i){

		//for each letter, perform ct[i] - k[i] %28
		//C's 'modulo (%)' is actually remainder. I'll do it manually

		current = (get_index_of_char(ct[i]) - get_index_of_char(k[i]));
		if (current < 0){current += charlist_len - 1;}
		//printf("--[dec]--> CT[i]: [%d]; K[i]: [%d]; PT[i]: [%d]; i = %d.\n",  ct[i], k[i], current, i);
		pt[i] = (char) charlist[current];
	}
}

int check_input (char * input, int * len){

	int i;

	//printf("Length: %d\n", *len);

	//correct for newline
	if (input[*len-1] == '\n'){
		input[*len-1] = '\0';
		*len = *len - 1;

	}

	for (i = 0; i < *len; ++i){

		if (get_index_of_char(input[i]) == -1){

			fprintf(stderr, "Invalid Character at Index %d. Quitting Program.", i);
			return 0;
		}
	}

	return 1;
}

// int main(int argc, char ** argv){

// 	int a = strlen(argv[1]);

// 	int j = check_input(argv[1], &a);

// 	printf("%d", j);

// 	return 0;

// }