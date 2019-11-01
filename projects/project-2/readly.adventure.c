#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h> 
#include <errno.h>
#include <dirent.h>

//===================== S U P P O R T I N G   F U N C T I O N S =======================//

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









//=============================== G A M E =================================//

void game(char * startRoom){

	printf("==>> GAME CALLED STARTING AT: %s\n", startRoom);

}









//================================ M A I N =================================//

int main() {

	char directoryName[100];
	directoryName[0] = '\0';
	char startRoom;
	startRoom[0] = '\0';

	//find the most recent directory
	findDirecrory(directoryName);
	
	//go to that directory:
	chdir(directoryName);

	//Find the text file that is designated start_room
	findStartRoom(&startRoom);

	//run the game with that start room as the initial room
	//game(startRoom)


}