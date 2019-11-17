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
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int backgroundGlobal = 1;

//======================= S U P P O R T I N G   F U N C T I O N S ============================//

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

void cd(char * path){

	// printf("===== [cd] = Called with path %s.\n", path);
	// fflush(stdout);

	if (chdir(path) != 0){

		printf("cd: no such file or directory: %s\n", path);
		fflush(stdout);
		return;
	}

	else{

		chdir(path);
		return;
	}
}

//===================== B U I L T I N   F U N C T I O N S =======================//


void __exit() {

	//exit the shell
	exit (0);
}


void __cd(char ** commandArray, int commandArrayIndex) {

	// printf("==== [__cd] = Called.\n");
	// fflush(stdout);

	if (commandArrayIndex == 1){

		// printf("==== [__cd] = GetEnv(HOME) => %s\n", getenv("HOME"));
		// fflush(stdout);
		cd(getenv("HOME"));
	}

	else{

		cd(commandArray[1]);
	}
	
	return;
}


int __status(int lastReturnValue, int mode){

	int status = 0;

	if (WIFEXITED(lastReturnValue) != 0){

		// the program exited wiht a value
		status = WEXITSTATUS(lastReturnValue);

		//print the value of the exit status
		if (mode){
		printf("exit status %d\n", status);
		fflush(stdout);
		}
	}

	else{

		//a signal killed that boi return the int, but dont set status
		status = WTERMSIG(lastReturnValue);

		if (mode){
		printf("terminated by signal %d\n", status);
		fflush(stdout);
		}

	}
	return status;
}

void catchSIGTSTP (int signalNumber){

	switch (backgroundGlobal){

		case 0:

			//switch to 'on' - background allowed.
			backgroundGlobal = 1;

			//post message. but most be async safe && reentrant ok - use write.
			const char * off_message = "Exiting foreground-only mode\n";
			write(1, off_message, strlen(off_message));
			fflush(stdout);

			break;

		case 1:

			//switch to 'off' - background naahwt allowed.
			backgroundGlobal = 0;

			//post message. but most be async safe && reentrant ok - use write.
			const char * on_message = "Entering foreground-only mode (& is now ignored)\n";
			write(1, on_message, strlen(on_message));
			fflush(stdout);

			break;
	}

	return;
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
	while (strcmp(inputData, "\n") == 0 || inputData[0] == '#'){
	//while (inputData[0] == '\n'){

		//print out the prompt for the user
		printf(": ");
		fflush(stdout);

		//read input from 
		fgets(inputData, 499, stdin);

	}

	//now we have a non- blank input.
	// printf("=== [promptInput] = Value of input: %s", inputData);
	// fflush(stdout);

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
			// printf("=== [promptInput] = Added: %s at %d\n", spid, *commandArrayIndex-1);
			// fflush(stdout);

			//current = "$$"
		}

		//we got a command or value that we want to append to the string
		else {

			//before we can add the token to the array, we must check for a '$$' at the end of the
			//string and replace it with the pid. 
			char temp[100];
			commandArray[*commandArrayIndex] = strdup(token);

			//ok its not elegant but yeah.
			if(strlen(commandArray[*commandArrayIndex]) >= 2){
				if (commandArray[*commandArrayIndex][strlen(commandArray[*commandArrayIndex])-2] == '$' &&
					commandArray[*commandArrayIndex][strlen(commandArray[*commandArrayIndex])-1] == '$'){

						token[strlen(token)-2]='\0';

						//free that old array entry.
						free(commandArray[*commandArrayIndex]);

						//strdup that again with the pid appended after the token
						snprintf(temp, 99, "%s%d", token, getpid());
						commandArray[*commandArrayIndex] = strdup(temp);

				}
			}

			*commandArrayIndex = *commandArrayIndex + 1;
			// printf("=== [promptInput] = Added: %s at %d\n", token, *commandArrayIndex-1);
			// fflush(stdout);
		}

		//get the next one on the line
		token = strtok(NULL, " \n");
	}
}

void execCommand(char ** commandArray, 
					int commandArrayIndex, 
					int backgroundFlag, 
					int * lastReturnValue, 
					char * redirIn, 
					char * redirOut,
					struct sigaction saSigint){


	pid_t forkPid = -42;
	pid_t actualPid;
	pid_t exitedPid;

	int result;

	forkPid = fork();

	switch(forkPid){

		case -1:

			//failure!
			perror("[error] FAILED FORK!");
			exit(1);
			break;

		case 0:

			//success! We're now the child boiiii

			//set it so that the child will get the sigint that we set to 'off' in a caller fxn

			saSigint.sa_handler = SIG_DFL;
			sigaction(SIGINT, &saSigint, NULL);

			//set redirection

			if (strcmp(redirOut, "") != 0){
				//we need to redirect stdout to the outfile
				// printf("======= [execCommand, Child] = Noticed that redirection of stdout is required.\n");
				// fflush(stdout);

				//open the target file to output to.
				int targetFD = open(redirOut, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (targetFD == -1){
					//perror("[error] FAILED TO OPEN FILE FOR OUTPUT REDIRECTION");
					printf("%s: no such file or directory\n", commandArray[0]);
					fflush(stdout);
					exit(1);
				}

				//redirect stdout (1) to targetFD
				result = dup2(targetFD, 1);
				if (result == -1){
					perror("[error] DUP2 COMMAND FAILED TO REDIRECT");
					exit(2);
				}

				//close on exec
				fcntl(targetFD, F_SETFD, FD_CLOEXEC);
			}

			if (strcmp(redirIn, "") != 0){
				//we need to redirect stdout to the outfile
				//printf("======= [execCommand, Child] = Noticed that redirection of stdin is required.\n");
				// /fflush(stdout);

				//open the target file to output to.
				int sourceFD = open(redirIn, O_RDONLY);
				if (sourceFD == -1){
					//perror("open");
					//perror("[error] FAILED TO OPEN FILE FOR INPUT REDIRECTION");
					printf("%s: no such file or directory\n", commandArray[0]);
					fflush(stdout);
					exit(1);
				}

				//redirect stdout (1) to targetFD
				result = dup2(sourceFD, 0);
				if (result == -1){
					perror("[error] DUP2 COMMAND FAILED TO REDIRECT");
					exit(2);
				}

				//close on execc
				fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
			}

			//now that we supposedly have redirection in place, run that command
			result = 0;
			commandArray[commandArrayIndex] = NULL;
			result = execvp(commandArray[0], (char * const *)commandArray);

			// printf("====== [execCommand, Child] = Exec ran and returned: %d\n", result);
			// fflush(stdout);

			//check for erroring out:
			if (result == -1){
				printf("%s: no such file or directory\n", commandArray[0]);
				fflush(stdout);
				// fprintf(stderr,"execvp() failed. errno=%d\n",errno);
				// perror("execvp");
				// fflush(stdout);
				exit(3);
			}

			// printf("====== [execCommand; Child] = Running something.\n");
			// fflush(stdout);
			// sleep(4);
			// printf("====== [execCommand; Child] = Done running something.\n");
			// fflush(stdout);

			exit(0);

			break;


		default:

			//parent will exec this
			
			//if the conditions are right to launch a background process, do that.
			if (backgroundFlag && backgroundGlobal){
				
				actualPid = waitpid(forkPid, lastReturnValue, WNOHANG);
				printf("background pid is %d\n", forkPid);
				fflush(stdout);
			}

			//otherwise, launch as a foreground process.
			else{

				// printf("====== [execCommand; Parent] = Default Started\n");
				// fflush(stdout);
				actualPid = waitpid(forkPid, lastReturnValue, 0);
				// printf("====== [execCommand; Parent] = PID Quit with code: %d; actualPid: %d\n", __status(*lastReturnValue, 0), actualPid);
				// fflush(stdout);
			}	
	}

	//iterate until there are no background procs left that have completed. 
	//this while works right before input - that's why its here.
	while ((exitedPid = waitpid(-1, lastReturnValue, WNOHANG)) > 0){

		//we have process exitedPid that quit. Report that:
		printf("background pid %d is done: ", exitedPid);
		__status(*lastReturnValue, 1);
		fflush(stdout);
	}
}

//void checkBackgroundProcesses()

void runSmallsh(){

	int backgroundFlag;
	
	char *commandArray[20];
	int commandArrayIndex = 0;
	
	char redirOut[40];
	char redirIn[40];

	int i;
	int lastReturnValue = -420;

	//set up the required sigaction() things

	struct sigaction saSigint = {0};
	saSigint.sa_handler = SIG_IGN;	//ignore sigint
	sigfillset(&saSigint.sa_mask);
	saSigint.sa_flags = 0;
	sigaction(SIGINT, &saSigint, NULL);

	struct sigaction saSigtstp = {0};
	saSigtstp.sa_handler = catchSIGTSTP;	//ignore sigtstp
	sigfillset(&saSigtstp.sa_mask);
	saSigtstp.sa_flags = 0;
	sigaction(SIGTSTP, &saSigtstp, NULL);


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

		//check if we need to use a builtin
		if ( (strcmp(commandArray[0], "exit") == 0) || 
			(strcmp(commandArray[0], "status") == 0) || 
			(strcmp(commandArray[0], "cd") == 0)){

			if ((strcmp(commandArray[0], "exit") == 0)){

				__exit();
			}
			else if ((strcmp(commandArray[0], "status") == 0)){

				__status(lastReturnValue, 1);
			}
			else if ((strcmp(commandArray[0], "cd") == 0)){

				__cd(commandArray, commandArrayIndex);
			}
		}

		//else execute the chosen command
		else{
			//Debug:
			// printf("== [runSmallsh] = BG:%d; Command:", backgroundFlag);
			// fflush(stdout);
			//printArray(commandArray, commandArrayIndex);
			// printf("== [runSmallsh] = InFile: %s; OutFile: %s.\n", redirIn, redirOut);
			// fflush(stdout);

			execCommand(commandArray, commandArrayIndex, backgroundFlag, &lastReturnValue, redirIn, redirOut, saSigint);
		}
	}
}


int main(){

	// printf("= [main] = Started.\n");
	// fflush(stdout);

	runSmallsh();

	// printf("= [main] = Ended.\n");
	// fflush(stdout);


}