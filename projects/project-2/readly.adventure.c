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


//define variables assoc with our mutex
pthread_t thread[1];
pthread_mutex_t lock;


//define a struct to hold the current connections.
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

	//open cd
	cd = opendir(".");

	//read all items
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
			
			//if we have a connection line
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

	//for each connection in the struct except the last, print it followed by a colon
	for (i = 0; i < (currentConnections->count-1); ++i){ //print all but the last one separated by ","

		printf("%s, ", currentConnections->connections[i]);

	}

	//print hte last one with that colon-boi
	printf("%s.\n", currentConnections->connections[currentConnections->count-1]);
	return;
}


void prompt(char * currentRoom, struct connectionsStruct * currentConnections){
	
	//print out the prompt, and leave next char to write at the ">" so that fgets in imput() can get it from there
	printf("\nCURRENT LOCATION: %s\n", currentRoom);
	getCurrentConnections(currentRoom, currentConnections);
	printf("POSSIBLE CONNECTIONS: ");
	printCurrentConnections(currentConnections);
	printf("WHERE TO? >");
	return;
}


void * timeToFile(){

	FILE * file;
	time_t timeNow;
	struct tm * currentTime;
	char output[200];


	//always restart
	while(1){
		//wait for lock, when we can lock that means we wanna write time to file
		pthread_mutex_lock(&lock);

		//timeNow = time(0);
		time(&timeNow);
		currentTime = localtime(&timeNow);

		//print that nicely formatted time
		strftime(output, 200, " %l:%M%p, %A, %B %d, %Y", currentTime);
		
		//open the output file in the parent dir (because rules lol).
		file = openFile("../currentTime.txt", "w");
		fprintf(file, "%s\n", output);
		fclose(file);

		//unlock, then wait a sec
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}


int winCheck (char * currentRoom){

	char winRoom[10];
	//zero the winroom array. whatever.
	memset(winRoom, 0, sizeof(winRoom));

	//get the end room
	findRoomOfType(winRoom, "END_ROOM");

	//if we are in the end room, then we've won.
	if (strcmp(currentRoom, winRoom) == 0){
		return 1;
	}
	return 0;
}


int input(char * currentRoom, struct connectionsStruct * currentConnections){
	
	char newRoom[10];
	//seek user input

	//read of length max 9.
	scanf("%9s", newRoom);

	//repeat until we get valid input
	while ( !(in(newRoom, currentConnections)) && !(strcmp(newRoom, "time") == 0)){
		printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		prompt(currentRoom, currentConnections);
		scanf("%9s", newRoom);
	}

	//if we have a time request, return 1 so game() can catch this and do a time.
	if (strcmp(newRoom, "time") == 0){
		return 1;
	}

	//otherwise, copy the new room into currentRoom.
	strcpy(currentRoom, newRoom);
	return 0;
}


void printDate(FILE * file){

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	//read from the provided file, and only get the first line
	read = getline(&line, &len, file);

	//read error... not that there should be one, unless someone removes the contents
	//of the file.
	assert(read!=-1);

	//print that line.
	printf("\n%s", line);
	return;
}


//=============================== G A M E =================================//


void game(char * startRoom){

	//printf("==>> GAME CALLED STARTING AT: %s\n", startRoom);

	//variable definitions
	char path[1000][10];
	FILE * file;
	memset(path, 0, sizeof(path));
	char currentRoom[10];
	memset(currentRoom, 0, sizeof(currentRoom));
	int winState = 0;
	int steps = 0;
	int k, err, ret;

	//create a mutex
	if (pthread_mutex_init(&lock, NULL) != 0){
		printf("ERROR: MUTEX INIT FAILED\n");
	}

	//grab lock on that mutex
	pthread_mutex_lock(&lock);

	//create a thread of timeToFile()
	err = pthread_create(&thread[0], NULL, &timeToFile, NULL);
	if (err != 0){
		printf("ERROR: THREAD CREATION FAILED");
	}

	//zero the struct of connections
	struct connectionsStruct currentConnections[1];
	memset(currentConnections->connections, 0, sizeof(currentConnections->connections[0][0] * 60));
	currentConnections->count = 0;

	//move current (start) room into currentRoom
	strcpy (currentRoom, startRoom);
	
	//first room does not count because rules lol.
	//strcpy (path[steps], startRoom);
	//steps++;

	while (winState != 1){

		//prompt the user
		prompt(currentRoom, currentConnections);

		//seek the input and check it, and store next room in currentroom, or call the time function
		ret = input(currentRoom, currentConnections);
		while (ret == 1){
			//unlock 
			pthread_mutex_unlock(&lock);

			// printf("[input] UNLOCKED\n");
			// fflush(stdout);

			sleep(0.5);

			//wait till lock again
			pthread_mutex_lock(&lock);

			file = openFile("../currentTime.txt", "r");
			printDate(file);
			fclose(file);


			// printf("[input] LOCKED\n");
			// fflush(stdout);

			//seek next round of input. I know its recursive - it'll all be fine i hope...
			prompt(currentRoom, currentConnections);

			//update what the user wants to do next
			ret = input(currentRoom, currentConnections);

		}

		//update path and len(path) == steps
		strcpy (path[steps], currentRoom);
		steps++;

		//check if this room is the win condition. will print & quit if win reached
		winState = winCheck(currentRoom);
	}

	//we have won.
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

	//print those steps taken
	for (k = 0; k < steps; ++k){
		printf("%s\n", path[k]);
	}

	//proper exit code yeet
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