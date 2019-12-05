#include "otp_core.h"

int main(int argc, char ** argv){

	//define the variables
	char A[SIZE];
	char k[SIZE];

	FILE * file;

	//check args
	if (argc != 4){
		error("Improper Arguments Provided. Quitting.");
		exit(2);
	}

	//open file 1
	file = open_file(argv[1], "r");
	fgets(A, SIZE, file);
	fclose(file);

	//open file 2
	file = open_file(argv[2], "r");
	fgets(k, SIZE, file);
	fclose(file);

	//insert nulls
	A[strcspn(A, "\n")] = '\0';
	k[strcspn(k, "\n")] = '\0';

	//check lengths
	if (strlen(A) != strlen(k)){
		error("[ cipher | plain ] text and Key length mismatch.");
	}

	//otp
	otp(atoi(argv[3]), A, k, 1);
	exit(0);

}