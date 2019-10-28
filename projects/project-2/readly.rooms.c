#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

void makeDirectory(){

	char pid[10];
	char * dirname_base = "./readly.rooms.";
	char dirname[40];

	//add the base to the empty array
	strcat(dirname, dirname_base);

	//convert int to cstr~ and add to the filename
	sprintf(pid, "%d", getpid());
	strcat(dirname, pid);

	//make that directory
	mkdir(dirname, 0777);

	//go to that directory
	chdir(dirname);

	return;

}

int in (int val, int * array, int arraySize){

	//immitation python in() fx. Straightforward.

	for (int j = 0; j < arraySize, ++j){

		//if vals are the same, return true its in that array
		if (array[j] == val){
			return 1;
		}

	}

	//not found - return false its not in that array.
	return 0;
}

void createRoomFiles(char ** chosenRooms){

	FILE * file = NULL;

	for (int i = 0; i < 7; ++i){

		file = fopen(chosenRooms[i], "w");

		if (!file){
			printf("ERROR ON FOPEN");
			exit(-1);
		}

		fprintf(file, "%s", "ROOM NAME: ");
		fprintf(file, "%s\n", chosenRooms[i]);

		fclose(file);
	}

	return;
}

void createRooms(roomNames, roomNamesSize, &chosenRooms){

	int chosen[7];

	for (int i = 0; i < 7; ++i){

		int choice = rand()%10;

		while(in(choice, chosen, (i+1))){
			choice = rand()%10;
		}

		chosenRooms[i] = roomNames[choice];

	}

	createRoomFiles(chosenRooms);
	return;
}




int main(){

	int roomNamesSize = 10;
	char * roomNames[10];
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

	createRooms();



}