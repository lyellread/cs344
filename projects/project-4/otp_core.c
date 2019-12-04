//top level otp program - performs enc, dec 'primitives'

#include <stdio.h>
#include <string.h>

#include "charlist.h"

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
		printf("--[enc]--> PT[i]: [%d]; K[i]: [%d]; CT[i]: [%d]; i = %d.\n", pt[i], k[i], current, i);
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
		printf("--[dec]--> CT[i]: [%d]; K[i]: [%d]; PT[i]: [%d]; i = %d.\n",  ct[i], k[i], current, i);
		pt[i] = (char) charlist[current];
	}
}

int main(){

	char pt[100] = "ASASDFASODJ    FASLDJFLEWRLEWJROFWJFDCOD";
	char k[100] = "KSOELDLPSOEKDTSDFIUSASJDFKASDFKASJDFKDSF";
	char ct[100];
	int len = 40;

	pt[27] = '\0';
	k[27] = '\0';
	ct[27] = '\0';

	otp_encrypt(pt, k, ct, len);

	printf ("-[main]--> Encrypted: %s\n", ct);
	memset(pt, 0, 99);

	otp_decrypt(ct, k, pt, len);

	printf("-[main]--> Decrypted: %s\n", pt);

	return 0;

}