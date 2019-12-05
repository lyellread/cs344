
#include "otp_core.h"

int sendData(char * buffer, int socketFD){
	int charsSent = send(socketFD, buffer, strlen(buffer), 0); // Send success back
	if (charsSent < 0) error("ERROR writing to socket");

	return charsSent;
}

int recvData(char * buffer, int socketFD){

	memset(buffer, '\0', SIZE);
	int charsRead = recv(socketFD, buffer, SIZE, 0); // Read the client's message from the socket
	if (charsRead < 0) error("SERVER: ERROR: Failed reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

	return charsRead;
}

void otp_d(int port, int enc){

	int listenSocketFD;
	int establishedConnectionFD;

	int charsToRead;

	socklen_t sizeOfClientInfo;
	char buffer[SIZE];
	struct sockaddr_in serverAddress, clientAddress;

	pid_t pid;

	char A[SIZE];
	char k[SIZE];
	char B[SIZE];
	char mode[4];

	if(enc){
		strcpy(mode, "enc");
	}
	else{
		strcpy(mode, "dec");
	}

	//if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(port); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while (1){

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		pid = fork();

		switch (pid){

			case -1:

				//failure! 
				error("SERVER: ERROR: FORK Failed!");
				exit (4);
				break;

			case 0:

				//this is the child process

				// C H E C K   P R O P E R   S E R V E R

				//get the client type.
				recvData(buffer, establishedConnectionFD);

				//check if we are connected to the right other side
				if (strcmp (buffer, mode) == 0){
					
					memset(buffer, '\0', SIZE);
					strcpy(buffer, "ok");
					sendData(buffer, establishedConnectionFD);
				}	
				
				else {
					
					memset(buffer, '\0', SIZE);
					strcpy(buffer, "error");
					sendData(buffer, establishedConnectionFD);
					close(establishedConnectionFD); // Close the existing socket which is connected to the client
					exit(2);
				}


				// T R A N S M I S S I O N   L E N G T H

				//expect the number of bytes to be read (x2, as files are same size)
				recvData(buffer, establishedConnectionFD);
				charsToRead = atoi(buffer);

				//repeat that to the client
				sendData(buffer, establishedConnectionFD);

				memset(buffer, '\0', SIZE);


				// R E C V   {A}

				int charsRead = recv(establishedConnectionFD, A, charsToRead, MSG_WAITALL);
				if (charsRead < 0) error("ERROR reading from socket");
				//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				strcpy(buffer, "ok");
				sendData(buffer, establishedConnectionFD);


				// R E C V   {K}

				charsRead = recv(establishedConnectionFD, k, charsToRead, MSG_WAITALL);
				if (charsRead < 0) error("ERROR reading from socket");
				//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, "ok");
				sendData(buffer, establishedConnectionFD);


				// R E C V "next"

				recvData(buffer, establishedConnectionFD);
				if (strcmp(buffer, "next") != 0){error("SERVER: ERROR: Expected 'next'");}


				// S E N D   {B}
				if(enc){
					otp_encrypt(A, k, B, strlen(A));
				}
				else{
					otp_decrypt(A, k, B, strlen(A));
				}

				sendData(B, establishedConnectionFD);

				recvData(buffer, establishedConnectionFD);

				//close(establishedConnectionFD); // Close the existing socket which is connected to the client
				exit(0);
				break;

			default:

				wait(NULL);
				
			}
		close(establishedConnectionFD);
	}
	close(listenSocketFD);
}