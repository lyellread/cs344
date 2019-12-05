#include "otp_core.h"

int main(int argc, char ** argv){

	char A[SIZE];
	char k[SIZE];

	FILE * file;

	if (argc != 4){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	file = open_file(argv[1], "r");
	fgets(A, SIZE, file);
	fclose(file);

	file = open_file(argv[2], "r");
	fgets(k, SIZE, file);
	fclose(file);

	A[strcspn(A, "\n")] = '\0';
	k[strcspn(k, "\n")] = '\0';

	if (strlen(A) != strlen(k)){
		error("[ cipher | plain ]text and Key length mismatch.")
	}

	otp(atoi(argv[3]), A, k, 0);
	exit(0);

}