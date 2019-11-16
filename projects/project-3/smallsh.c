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


void __status()