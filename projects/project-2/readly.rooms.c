#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h> 

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

	printf("-->%s\n", dirname);

	//make that directory
	mkdir(dirname, 0777);

	//go to that directory
	chdir(dirname);

	return;

}

int in (int val, int * array, int arraySize){

	//immitation python in() fx. Straightforward.

	for (int j = 0; j < arraySize; ++j){

		//if vals are the same, return true its in that array
		//printf("Comparing %d, %d", val, array[j]);

		if (array[j] == val){
			return 1;
		}

	}

	//not found - return false its not in that array.
	return 0;
}

FILE * openFile(char * filename, char method){

	FILE * file = fopen(filename, method);

		if (file == NULL){
			//if error, quit.
			printf("ERROR ON FOPEN");
			exit(-1);
		}
		else{

			return file;
		}
}

//============================= F I L E   C R E A T I O N =================================//

void createRoomFiles(char ** chosenRooms){

	FILE * file = NULL;

	//we need 7 rooms....

	for (int i = 0; i < 7; ++i){

		//open that room name (create file + stream)

		file = openFile(chosenRooms[i], 'w');

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

	for (int i = 0; i < 7; ++i){

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

	FILE * file;
	char * line;
	int lineCount = 0;
	size_t len=0;
	ssize_t read;

	file = openfile(filename, 'r');

	while ((read = getline(&line, &len, fp)) != -1){

		printf("Got Line\n");
		++lineCount;
	}
	fclose(file);
	printf("File %s had %d connections", filename, (lineCount-1));

	return (lineCount-1)

}

void connectTwoRooms (int a, int b, char ** chosenRooms){

	FILE * file = openFile(chosenRooms[a], 'a');
	fprintf(file, "CONNECTION %d: %s\n",getNumberOfConnections(chosenRooms[a])+1, chosenRooms[b]);
	fclose(file);

	FILE * file = openFile(chosenRooms[b], 'a');
	fprintf(file, "CONNECTION %d: %s\n",getNumberOfConnections(chosenRooms[b])+1, chosenRooms[a]);
	fclose(file);

	return;

}


int indexOf(char * name, char ** chosenRooms){

	for (int i = 0; i < 7; ++i){
		if (strcmp(name, chosenRooms[i]) == 0){
			return i;
		}
	}

	return -1;
	exit(-2);
}



int findFreeRoomNotSelf(int self, int * selfConnections, int selfConnectionsLength){

	int randomRoom = rand()%8;
	while (randomRoom == self || in(randomRoom, selfConnections, selfConnectionsLength) || getNumberOfConnections(randomRoom)){
		randomRoom = rand()%8;
	}
	printf("Settled on room %d as connxn for self room %d\n", randomRoom, self)
	return randomRoom

}


void getConnections(char * filename, int ** array, int * arrayIndex, char ** chosenRooms){

	memset(array, 0, 10);
	*arrayIndex = 0;

	FILE * file = openFile(filename, 'r');
	char word[100];
	while(fscanf(file, " %s", word)){

		if (word == ":"){
			fscanf(file, "%s", word);
			*array[*arrayIndex] = indexOf(word, chosenRooms);
		}
	}

	return;
}


void linkRooms(char ** chosenRooms){

	//for 4 - existing conections, add random connection if
		//other room can accept
		//not to this same room
		//connection does not already exist

	int connections;
	int newConnection;
	int selfConnections[10]
	int selfConnectionsLength=0;

	for (int i = 0; i < 7; ++i){

		connections = getNumberOfConnections(chosenRooms[i]);

		if (connections >= 4){
			printf("Skipping room %d\n", i);
		}
		else{

			for (int j = 0; j < (4-connections); ++j){

				getConnections(chosenRooms[i], &selfConnections, &selfConnectionsLength);
				newConnection = findFreeRoomNotSelf(i, selfConnections, selfConnectionsLength);
				connectTwoRooms(newConnection, i);

			
			}
		}
	}

	//return
	return;

}

int main(){

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

	srand(time(0));

	//create directory && chdir to it

	makeDirectory();

	//pick random rooms and create those files with only the ROOM NAME header present

	createRooms(roomNames, roomNamesSize, chosenRooms);

	//link files already created to one another with the CONNECTION n list.

	linkRooms(chosenRooms);

	//assign ( START_ROOM | MID_ROOM | END_ROOM ) ROOM TYPE to each room.

	//typeRooms(chosenRooms);

	//call the Game program?



}