#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "otp_core.h"

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD;
	int establishedConnectionFD;

	int portNumber;

	int charsRead;
	int charsToRead;

	socklen_t sizeOfClientInfo;
	char buffer[SIZE];
	struct sockaddr_in serverAddress, clientAddress;

	char A[SIZE];
	char k[SIZE];
	char B[SIZE];

	//if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
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

		// Get the message from the client and display it
		memset(buffer, '\0', SIZE);
		charsRead = recv(establishedConnectionFD, buffer, SIZE, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);

		//check if we are connected to the right other side
		if (strcmp (buffer, "enc") == 0){
			charsRead = send(establishedConnectionFD, "ok", 2, 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");		
		
		else {
			charsRead = send(establishedConnectionFD, "error", 2, 0); // Send success back
			error ("SERVER: Wrong Connction");
			if (charsRead < 0) error("ERROR writing to socket");
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			//exit(2);
		}

		//expect the number of bytes to be read (x2, as files are same size)
		memset(buffer, '\0', SIZE);
		charsRead = recv(establishedConnectionFD, buffer, SIZE, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		charsToRead = atoi(buffer);

		//read the first file == A
		memset(buffer, '\0', SIZE);
		charsRead = recv(establishedConnectionFD, buffer, SIZE, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		strcpy(A, buffer);

		//check len OK
		if (!charsRead == charsToRead){
			error("SERVER: Read and Expect Mismatch");
			//exit (2);
		}
		
		//read the second file = k
		memset(buffer, '\0', SIZE);
		charsRead = recv(establishedConnectionFD, buffer, SIZE, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		strcpy(k, buffer);

		//check len OK
		if (!charsRead == charsToRead){
			error("SERVER: Read and Expect Mismatch");
			//exit (2);
		}

		//compute the return value;
		strcpy(B, "THE OUTPUT WILL BE HERE WHEN COMPLETE");

		//send back the result
		charsRead = send(establishedConnectionFD, B, strlen(B), 0);
		if (charsRead < 0) error("ERROR writing to socket");

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
		//exit(0);
	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
