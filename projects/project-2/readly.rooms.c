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

	strcat(dirname, dirname_base);

	sprintf(pid, "%d", getpid());

	strcat(dirname, pid);

	mkdir(dirname, 0777);

	return;

}







int main(){

	//create directory

	makeDirectory();

	//nav to directory




}