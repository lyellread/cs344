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


//======================= S U P P O R T I N G   F U N C T I O N S ============================//


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

void printArray(char ** array, int len){

	int i;

	for (i = 0; i < len-1; ++i){

		printf("%s, ", array[i]);
		fflush(stdout);
	}

	printf("%s. [END]\n", array[len-1]);
	fflush(stdout);

	return;
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

void promptInput(int * backgroundFlag, char ** commandArray, int * commandArrayIndex, char * redirIn, char * redirOut){

	//initialize those variables
	char inputData[500];
	inputData[0] = '\0';

	*backgroundFlag = 0;

	char spid[7];
	spid[0] = '\0';

	char * token;
	*commandArrayIndex = 0;

	//print out the prompt for the user
	printf(": ");
	fflush(stdout);

	//read input from 
	fgets(inputData, 499, stdin);

	//if there's nothing, repeat until there is *something*
	while (strcmp(inputData, "\n") == 0){
	//while (inputData[0] == '\n'){

		//print out the prompt for the user
		printf(": ");
		fflush(stdout);

		//read input from 
		fgets(inputData, 499, stdin);

	}

	//now we have a non- blank input.
	printf("=== [promptInput] = Value of input: %s\n", inputData);
	fflush(stdout);

	//use strtok to break and process input word by word
	token = strtok(inputData, " \n");

	while (token != NULL){

		//check for input redirection ">"
		if (strcmp(token, ">") == 0){

			//the next word will be the output file.
			token = strtok(NULL, " \n");
			strcpy(redirOut, token);

			//current = redirOut
		}

		//check for input redirection "<"
		else if (strcmp(token, "<") == 0){

			//the next word will be the input file.
			token = strtok(NULL, " \n");
			strcpy(redirIn, token);

			//current = redirIn
		}

		//check for the use of the "&" operator, and set the background bool appropriately
		else if (strcmp(token, "&") == 0){

			*backgroundFlag = 1;

			//current = "&"
		}

		//check for hte $$ and immediately replace it with the pid as a string of this function.
		else if (strcmp(token, "$$") == 0){

			//convert the pid to string, then append it to the nth place in the commandArray
			sprintf(spid, "%d", getpid());
			commandArray[*commandArrayIndex] = strdup(spid);
			*commandArrayIndex = *commandArrayIndex + 1;
			printf("=== [promptInput] = Added: %s at %d\n", spid, *commandArrayIndex);
			fflush(stdout);

			//current = "$$"
		}

		//we got a command or value that we want to append to the string
		else {

			commandArray[*commandArrayIndex] = strdup(token);
			*commandArrayIndex = *commandArrayIndex + 1;
			printf("=== [promptInput] = Added: %s at %d\n", token, *commandArrayIndex);
			fflush(stdout);
		}

		//get the next one on the line
		token = strtok(NULL, " \n");
	}
}


void runSmallsh(){

	int backgroundFlag;
	char *commandArray[20];
	int commandArrayIndex = 0;
	char redirOut[40];
	char redirIn[40];
	int i;

	while (1){

		redirIn[0] = '\0';
		redirOut[0] = '\0';

		//clean up from last round's strudp's which alloc that sweet sweet memory
		for (i = 0; i < commandArrayIndex; ++i){
			if (commandArray[i] != NULL){
				free(commandArray[i]);
			}
		}
		
		//get the input from promptInput()
		promptInput(&backgroundFlag, commandArray, &commandArrayIndex, redirIn, redirOut);

		//interpret the result?


		//execute the chosen command
		printf("== [runSmallsh] = BG:%d; Command:", commandArrayIndex);
		fflush(stdout);
		printArray(commandArray, commandArrayIndex);
		printf("== [runSmallsh] = InFile: %s; OutFile: %s.\n", redirIn, redirOut);
		fflush(stdout);
	}
}


int main(){

	printf("= [main] = Started.\n");
	fflush(stdout);

	runSmallsh();

	printf("= [main] = Ended.\n");
	fflush(stdout);


}