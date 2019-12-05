
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <assert.h>

#ifndef OTP_CORE_H
#define OTP_CORE_H

	static const int SIZE = 100000;

	//define the wordlist of valid characters to be used
	static const char charlist[28] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const int charlist_len = 27;

	//define functions that will be used by the daemon
	void otp_encrypt (char * pt, char * k, char * ct, int len);
	void otp_decrypt (char * ct, char * k, char * pt, int len);
	
	//define the functions to use by the clients
	int check_input (char * input, int * len);
	void error(const char * msg);
	FILE * open_file(char * filename, char * method);

	//define the otp and otp_d functions to be used
	void otp (int port; char * A, char * k, int enc);
	void otp_d(int port, int enc);

#endif