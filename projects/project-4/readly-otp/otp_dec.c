#include "otp_core.h"

int main(int argc, char ** argv){

	char A[SIZE];
	char k[SIZE];

	FILE * file;

	//improper length
	if (argc != 4){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	//check file 1
	file = open_file(argv[1], "r");
	fgets(A, SIZE, file);
	fclose(file);

	//open file 2
	file = open_file(argv[2], "r");
	fgets(k, SIZE, file);
	fclose(file);

	//rep \n with null
	A[strcspn(A, "\n")] = '\0';
	k[strcspn(k, "\n")] = '\0';

	//check that lengths are good
	if (strlen(A) != strlen(k)){
		error("[ cipher | plain ] text and Key length mismatch.");
	}

	//run otp
	otp(atoi(argv[3]), A, k, 0);
	exit(0);

}