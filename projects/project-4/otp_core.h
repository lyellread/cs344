
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

#endif