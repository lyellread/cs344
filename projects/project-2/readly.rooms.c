#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h> 
#include <errno.h>


//============================= D I R E C T O R Y ===================================//


void makeDirectory(){

	char pid[10];
	char * dirname_base = "./readly.rooms.";
	char dirname[40];

	//zero the first character so strcat() is happy! 
	dirname[0]='\0';

	//add the base to the empty array
	strcat(dirname, dirname_base);

	//convert int to cstr~ and add to the filename
	sprintf(pid, "%d", getpid());
	strcat(dirname, pid);

	//printf("-->%s\n", dirname);

	//make that directory
	mkdir(dirname, 0777);

	//go to that directory
	chdir(dirname);

	return;
}


int in (int val, int * array, int arraySize){

	int j;
	//immitation python in() fx. Straightforward.

	for (j = 0; j < arraySize; ++j){

		//if vals are the same, return true its in that array
		//printf("Comparing %d, %d", val, array[j]);

		if (array[j] == val){
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


//============================= F I L E   C R E A T I O N =================================//


void createRoomFiles(char ** chosenRooms){

	FILE * file = NULL;
	int i;

	//we need 7 rooms....

	for (i = 0; i < 7; ++i){

		//open that room name (create file + stream)
		file = openFile(chosenRooms[i], (char *)"w");

		//add to the top of the file this init data:
		fprintf(file, "%s", "ROOM NAME: ");
		fprintf(file, "%s\n", chosenRooms[i]);

		//close that file.
		fclose(file);
	}

	return;
}


void createRooms(char ** roomNames, int roomNamesSize, char ** chosenRooms){

	//this will store the past choices so that we dont choose the same 
	//choice twice
	int chosen[7];
	int i;

	for (i = 0; i < 7; ++i){

		//rand%10 ---> 0..9
		int choice = rand()%10;

		//enforce random !in chosen
		while(in(choice, chosen, i)){
			choice = rand()%10;
		}

		//printf("Choice: %d, Name: %s\n", choice, roomNames[choice]);
		//assign name and number fields
		chosen[i] = choice;
		chosenRooms[i] = roomNames[choice];

	}

	//call create the room files
	createRoomFiles(chosenRooms);
	return;
}


//===================================== R O O M   C O N N E C T I O N S ====================================//


int getNumberOfConnections(char * filename){

	//printf("====== Finding how many connections exist in file: %s\n", filename);

	FILE * file;
	int lines = 0;
	char c;

	//open the target fike ti read it
	file = openFile(filename, (char *)"r");

	//do a characterwise read to find newlines
	for (c = getc(file); c != EOF; c = getc(file)) {
		//printf("  checking char: %c\n", c);
		if (c == '\n') {
			lines = lines + 1;
		}

	}


	//printf("*===== Connections in file %s: %i\n",filename, (lines-1));
	fclose(file);

	//its not clean or good form by any means, but it will work.
	return (lines-1);
}


void connectTwoRooms (int a, int b, char ** chosenRooms){

	//printf("\n==== Connecting rooms %d and %d.", a, b);
	//link a to b
	FILE * file = openFile(chosenRooms[a], (char *)"a");
	fprintf(file, "CONNECTION %d: %s\n",getNumberOfConnections(chosenRooms[a])+1, chosenRooms[b]);
	fclose(file);

	//link b to a
	file = openFile(chosenRooms[b], (char *)"a");
	fprintf(file, "CONNECTION %d: %s\n",getNumberOfConnections(chosenRooms[b])+1, chosenRooms[a]);
	fclose(file);

	return;
}


int indexOf(char * name, char ** chosenRooms){

	int i;

	//return the index of the name name in chosenRooms
	for (i = 0; i < 7; ++i){
		if (strcmp(name, chosenRooms[i]) == 0){
			return i;
		}
	}

	//not found; break!
	return -1;
}


int findFreeRoomNotSelf(int self, int * selfConnections, int selfConnectionsLength, char ** chosenRooms){

	//printf("=== Called find room. Self: %d, self connections #: %d\n", self, selfConnectionsLength);

	int randomRoom = rand()%7;
	//printf("*== Room Rand: %d\n", randomRoom);

	//while the generated room does not meet the requirements, find a new one.
	while (randomRoom == self || in(randomRoom, selfConnections, selfConnectionsLength) || getNumberOfConnections(chosenRooms[randomRoom]) > 6 ){
		randomRoom = rand()%7;
		//printf("*== Room Rand: %d : %s\n", randomRoom, chosenRooms[randomRoom]);
		//printf("*== Room Rand: %d\n", randomRoom);
	}
	//printf("*=> Settled on room %d (connections: %d)\n", randomRoom, getNumberOfConnections(chosenRooms[randomRoom]));
	return randomRoom;
}


void printConnections(char * filename, int * array, int arrayIndex, char ** chosenRooms){

	//unused function - prints the contents of the array of connections passed. 
	printf("====== Printing Connections for file: %s:\n", filename);

	int i;

	for (i = 0; i < arrayIndex; ++i){

		printf("*===== Connects to: %s\n", chosenRooms[array[i]]);
	}

	printf("====== END\n");
	return;
}


void getConnections(char * filename, int * array, int * arrayIndex, char ** chosenRooms){

	//printf("==== Called getConnections on room %s (%d)\n", filename, indexOf(filename, chosenRooms));
	memset(array, 0, 10);
	*arrayIndex = 0;

	FILE * file;
	char * line = NULL;
	char * token;
	size_t len = 0;
	ssize_t read;

	//open that file
	file = openFile(filename, (char *)"r");

	//printf("\n\n-------\nFilename: %s\n", filename);

	//read it line by line ...
	while ((read = getline(&line, &len, file)) != -1){
		//printf("Line: %s\n", line);

		//... parse before and after : inclusive-or ' ' with strtok
		token = strtok (line, ": \n");
		while (token != NULL){
			
			if (strcmp(token, "CONNECTION") == 0){
				token = strtok (NULL, ": \n");		//pass integer value
				token = strtok (NULL, ": \n");		//get that name into token
				//printf("CONNECTION FOUND: %s\n", token);
				array[*arrayIndex] = indexOf(token, chosenRooms);
				*arrayIndex = *arrayIndex + 1;
			}
			token = strtok (NULL, ": \n");			//get next line start word.
		}

	}

	fclose(file);

	//printConnections(filename, array, *arrayIndex, chosenRooms);

	return;
}


void linkRooms(char ** chosenRooms){

	//for 4 - existing conections, add random connection if
		//other room can accept
		//not to this same room
		//connection does not already exist

	//int connections;
	int newConnection;
	int selfConnections[10];
	int selfConnectionsLength=0;
	int i, j;

	//printf("== Started LinkRooms...\n");

	//each room needs three connections at least, so assure that with this:
	for (i = 0; i < 3; ++i){

		//for each room on this "lap" add a connection if it needs one
		for (j = 0; j < 7; ++j){

			//printf("*=> Connections for room %d, try %d have number %d\n",j, i, getNumberOfConnections(chosenRooms[j]));
			
			if (getNumberOfConnections(chosenRooms[j]) < 3){
				//add connection

				//printf("*=> Adding connections as there are not enough.\n");
				//printf("== Room: %s\n", chosenRooms[j]);
				//printf(" Get Connections\n");
				getConnections(chosenRooms[j], selfConnections, &selfConnectionsLength, chosenRooms);
				//printf(" Find Room\n");
	 			newConnection = findFreeRoomNotSelf(j, selfConnections, selfConnectionsLength, chosenRooms);
	 			//printf(" Connect Two Rooms\n");
	 			connectTwoRooms(newConnection, j, chosenRooms);

	 			//printf("*= Completed adding a new connection between %s and %s (aka %d, %d)\n", chosenRooms[j], chosenRooms[newConnection], j, newConnection);
			}
		}

		//printf("Completed a lap\n");
	}

	//return
	return;
}


void typeRooms(char ** chosenRooms){

	int startRoom, endRoom;
	int i;
	FILE * file;

	//generate start room and end room indicies
	startRoom = rand()%7;
	endRoom = rand()%7;

	//... they cannot be the same of course...
	while (endRoom == startRoom){
		endRoom = rand()%7;
	}

	//iterate over all the available rooms and add their respective room type
	for (i = 0; i < 7; ++i){
		if (i == startRoom){
			//write to this room that it is the start room
			file = openFile(chosenRooms[i], (char *)"a");
			fprintf(file, "ROOM TYPE: START_ROOM\n");
			fclose(file);
		}
		else if (i == endRoom){
			//write to this file that it is the end room
			file = openFile(chosenRooms[i], (char *)"a");
			fprintf(file, "ROOM TYPE: END_ROOM\n");
			fclose(file);
		}
		else{
			//write that this is a mid room
			file = openFile(chosenRooms[i], (char *)"a");
			fprintf(file, "ROOM TYPE: MID_ROOM\n");
			fclose(file);
		}
	}
}


int main(){

	//define our possible room names

	int roomNamesSize = 10;
	char * roomNames[roomNamesSize];
	roomNames[0] = "bacon";
	roomNames[1] = "sweets";
	roomNames[2] = "toast";
	roomNames[3] = "bread";
	roomNames[4] = "onion";
	roomNames[5] = "pepper";
	roomNames[6] = "salt";
	roomNames[7] = "eggs";
	roomNames[8] = "ham";
	roomNames[9] = "sauce";

	char * chosenRooms[7];

	//gotta seed random in a *completely safe* way
	srand(time(0));

	//create directory && chdir to it
	makeDirectory();

	//pick random rooms and create those files with only the ROOM NAME header present
	createRooms(roomNames, roomNamesSize, chosenRooms);

	//link files already created to one another with the CONNECTION n list.
	linkRooms(chosenRooms);

	//assign ( START_ROOM | MID_ROOM | END_ROOM ) ROOM TYPE to each room.
	typeRooms(chosenRooms);

	//call the Game program?
}