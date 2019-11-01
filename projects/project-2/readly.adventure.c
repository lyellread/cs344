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


pthread_t thread[1];
pthread_mutex_t lock;

struct connectionsStruct{
	char connections [6][10];
	int count;
};


//===================== S U P P O R T I N G   F U N C T I O N S =======================//


int in (char * val, struct connectionsStruct * currentConnections ){

	int j;
	//immitation python in() fx. Straightforward.

	for (j = 0; j < currentConnections->count; ++j){

		//if vals are the same, return true its in that array
		//printf("Comparing %d, %d", val, array[j]);

		if (strcmp(currentConnections->connections[j], val) == 0){
			return 1;
		}

	}

	//not found - return false its not in that array.
	return 0;
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

void findDirecrory(char * directoryName){

	//some help from https://stackoverflow.com/questions/42170824/use-stat-to-get-most-recently-modified-directory (and man page)
	struct stat currentStat;
	time_t mostRecent = 0;
	char mostRecentFilename[100];
	mostRecentFilename[0] = '\0';
	struct dirent * item;

	DIR * cd;
	cd = opendir(".");

	while ((item = readdir(cd))){

		//printf("Iterating over %s\n", item->d_name);

		memset(&currentStat, 0, sizeof(currentStat));

		if (stat(item->d_name, &currentStat) == -1){
			//error
			printf("ERROR");
			continue;
		}

		//only if it is a directory, and it has newer time than the last one, and it is not the cd or .., do we then update the most recent one.
		if (S_ISDIR(currentStat.st_mode) && currentStat.st_mtime > mostRecent && (strcmp(".", item->d_name) != 0) && (strcmp("..", item->d_name) != 0)){
			//printf("-------\n");

			mostRecentFilename[0]='\0';
			strcpy(mostRecentFilename, item->d_name);
			mostRecent = currentStat.st_mtime;
		}
	}

	closedir(cd);
	//printf("Most Recent Dir: %s\n", mostRecentFilename);

	strcpy(directoryName, mostRecentFilename);
	
	return;
}

void findRoomOfType(char * room, char * type){

	//some help from https://stackoverflow.com/questions/42170824/use-stat-to-get-most-recently-modified-directory (and man page)
	struct stat currentStat;
	struct dirent * item;
	FILE * file;
	DIR * cd;
	char * line = NULL;
	char * token;
	size_t len = 0;
	ssize_t read;

	cd = opendir(".");

	while ((item = readdir(cd))){

		//printf("Iterating over %s\n", item->d_name);

		memset(&currentStat, 0, sizeof(currentStat));

		if (stat(item->d_name, &currentStat) == -1){
			//error
			printf("ERROR");
			continue;
		}

		//if its a regular file, open that thing and get the type of file it is, return and quit if it is the start file.
		if (S_ISREG(currentStat.st_mode)){

			file = openFile(item->d_name, (char *)"r");


			//read it line by line ...
			while ((read = getline(&line, &len, file)) != -1){
				//printf("Line: %s\n", line);

				//... parse before and after : inclusive-or ' ' with strtok
				token = strtok (line, ": \n");
				while (token != NULL){
					
					if (strcmp(token, type) == 0){
						//printf("START FOUND: %s -- %s\n", item->d_name, token);
						strcpy(room, item->d_name);
						fclose(file);
						return;

					}

					//get next word
					token = strtok (NULL, ": \n");	
				}
			}
			fclose(file);
		}
	}

	closedir(cd);
	return;
}

void getCurrentConnections(char * currentRoom, struct connectionsStruct * currentConnections){

	FILE * file;
	char * line = NULL;
	char * token;
	size_t len = 0;
	ssize_t read;

	memset(currentConnections->connections, 0, sizeof(currentConnections->connections[0][0] * 60));
	currentConnections->count = 0;

	file = openFile(currentRoom, (char *)"r");

	//read it line by line ...
	while ((read = getline(&line, &len, file)) != -1){
		//printf("Line: %s\n", line);

		//... parse before and after : inclusive-or ' ' with strtok
		token = strtok (line, ": \n");
		while (token != NULL){
			
			if (strcmp(token, "CONNECTION") == 0){
				token = strtok (NULL, ": \n"); //get rid of int
				token = strtok (NULL, ": \n"); //store the connection dest in token

				strcpy(currentConnections->connections[currentConnections->count], token);
				currentConnections->count++;

			}

			//get next word
			token = strtok (NULL, ": \n");	
		}
	}
	fclose(file);
}

void printCurrentConnections(struct connectionsStruct * currentConnections){

	int i;
	for (i = 0; i < (currentConnections->count-1); ++i){ //print all but the last one separated by ","

		printf("%s, ", currentConnections->connections[i]);

	}
	printf("%s.\n", currentConnections->connections[currentConnections->count-1]);
	return;
}

void prompt(char * currentRoom, struct connectionsStruct * currentConnections){
	
	printf("\nCURRENT LOCATION: %s\n", currentRoom);
	getCurrentConnections(currentRoom, currentConnections);
	printf("POSSIBLE CONNECTIONS: ");
	printCurrentConnections(currentConnections);
	printf("WHERE TO? >");
	return;
}


void * timeToFile(){

		//wait for lock
		pthread_mutex_lock(&lock);

		printf("YO TIME TO FILE HERE");

		//unlock, then wait a sec
		pthread_mutex_unlock(&lock);

		return NULL;

}

int winCheck (char * currentRoom){

	char winRoom[10];
	memset(winRoom, 0, sizeof(winRoom));

	findRoomOfType(winRoom, "END_ROOM");

	if (strcmp(currentRoom, winRoom) == 0){
		return 1;
	}
	return 0;
}

void input(char * currentRoom, struct connectionsStruct * currentConnections){
	
	char newRoom[10];
	//seek user input
	scanf("%9s", newRoom);
	while ( !(in(newRoom, currentConnections)) && !(strcmp(newRoom, "time") == 0)){
		printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		prompt(currentRoom, currentConnections);
		scanf("%9s", newRoom);
	}

	if (strcmp(newRoom, "time") == 0){
		//unlock
		pthread_mutex_unlock(&lock);

		//wait till lock again
		pthread_mutex_lock(&lock);

		//seek next round of input. I know its recursive - it'll all be fine i hope...
		input(currentRoom, currentConnections);
	}

	strcpy(currentRoom, newRoom);
	return;

}


//=============================== G A M E =================================//

void game(char * startRoom){

	printf("==>> GAME CALLED STARTING AT: %s\n", startRoom);

	char path[1000][10];
	memset(path, 0, sizeof(path));
	char currentRoom[10];
	memset(currentRoom, 0, sizeof(currentRoom));
	int winState = 0;
	int steps = 0;
	int k, err;

	if (pthread_mutex_init(&lock, NULL) != 0){
		printf("ERROR: MUTEX INIT FAILED\n");
	}

	pthread_mutex_lock(&lock);

	err = pthread_create(thread, NULL, &timeToFile, NULL);
	if (err != 0){
		printf("ERROR: THREAD CREATION FAILED");
	}


	//printf("It's not the first set of declarations, I guess ;)");
	//fflush(stdout);

	struct connectionsStruct currentConnections[1];
	memset(currentConnections->connections, 0, sizeof(currentConnections->connections[0][0] * 60));
	currentConnections->count = 0;

	strcpy (currentRoom, startRoom);
	
	//first room does not count because rules lol.
	//strcpy (path[steps], startRoom);
	//steps++;

	while (winState != 1){

		//print out the prompt and such with current room
		//printf("[game] Started prompt\n");
		prompt(currentRoom, currentConnections);
		//printf("[game] Finished prompt\n");

		//seek the input and check it, and store next room in currentroom, or call the time function
		//printf("[game] Started input\n");
		input(currentRoom, currentConnections);
		//printf("[game] Finished input\n");

		strcpy (path[steps], currentRoom);
		steps++;


		//check if this room is the win condition. will print & quit if win reached
		//printf("[game] Started winCheck\n");
		winState = winCheck(currentRoom);
		//printf("[game] finished winCheck\n");


	}

	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

	for (k = 0; k < steps; ++k){
		printf("%s\n", path[k]);
	}

	exit(0);

}









//================================ M A I N =================================//

int main() {

	char directoryName[100];
	directoryName[0] = '\0';
	char startRoom[10];
	startRoom[0] = '\0';

	//find the most recent directory
	findDirecrory(directoryName);
	
	//go to that directory:
	chdir(directoryName);

	//Find the text file that is designated start_room
	findRoomOfType(startRoom, "START_ROOM");

	//run the game with that start room as the initial room
	game(startRoom);


}