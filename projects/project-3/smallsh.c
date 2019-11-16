#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h> 
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <assert.h>


//============================= D I R E C T O R Y ===================================//


int int_in (int val, int * array, int arraySize){

	int j;
	//immitation python in() fx. Straightforward.

	for (j = 0; j < arraySize; ++j){

		//if vals are the same, return true its in that array
		//printf("Comparing %d, %d", val, array[j]);

		if (array[j] == val){
			return j;
		}

	}

	//not found - return false its not in that array.
	return -1;
}


int char_in (char * val, char ** array, int arraySize){

	int j;
	//immitation python in() fx. Straightforward.

	for (j = 0; j < arraySize; ++j){

		//if vals are the same, return true its in that array
		//printf("Comparing %d, %d", val, array[j]);

		if (array[j] == val){
			return j;
		}

	}

	//not found - return false its not in that array.
	return -1;
}


FILE * openFile(char * filename, char * method){

	//printf("\n==== Function openFile called with args: %s, %s\n", filename, method);
	errno = 0;
	FILE * file = fopen(filename, method);

		if (file == NULL){
			//if error, quit.
			//printf("ERROR ON FOPEN, ERRNO: %d\n", errno);
			exit(-1);
		}
		else{
			//printf("[FOPEN: FD: %d]\n", fileno(file));
			return file;
		}
}


//===================== B U I L T I N   F U N C T I O N S =======================//


void __exit() {

	//kill anything that is running


	//exit the shell
	exit (0);
}


void __cd(char * path) {

	//check that path is valid


	//change directory to that path


}


void __status(){

	//return the value of the exit status

}

void promptInput(int * backgroundFlag, char ** commandArray char * redirIn, char * redirOut){

	//initialize those variables
	char * inputData[500];
	inputData[0] = '\0';
	char * token;

	//print out the prompt for the user
	printf(": ");
	fflush(stdout);

	//read input from 
	scanf("%499s", inputData);

	//if there's nothing, repeat until there is *something*
	while (strcmp(inputData, "") == 0){

		//print out the prompt for the user
		printf(": ");
		fflush(stdout);

		//read input from 
		scanf("%499s", inputData);

	}

	//now we have a non- blank input.
	printf("=== [promptInput] = Value of input: %s\n", inputData);
	fflush(stdout);

	//use strtok to break and process input word by word
	token = strtok(inputData, " ");

	while (token != NULL){

		//check for input redirection ">"
		if (strcmp(token, ">") == 0){

			//the next word will be the output file.
			token = strtok(NULL, " ");
			strcpy(redirOut, token);

		}

		//check for input redirection "<"
		if (strcmp(token, "<") == 0){

			//the next word will be the input file.
			token = strtok(NULL, " ");
			strcpy(redirIn, token);

		}

		//check for the use of the "&" operator, and set the background bool appropriately
		


		//get the next one on the line
		token = strtok(NULL, " ");
	}




}