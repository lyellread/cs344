
#include "otp_core.h"

int sendData(char * buffer, int socketFD){

	//send the data. report error on failure
	int charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	return charsWritten;
}

int recvData(char * buffer, int socketFD){

	//srecv the data. report error on failure
	memset(buffer, '\0', SIZE); // Clear out the buffer again for reuse
	int charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	return charsRead;

}

void otp (int port, char * A, char * k, int enc){

	//init vars
	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[SIZE];
	char mode[4];
	char B[SIZE];

	//set mode
	if(enc){
		strcpy(mode, "enc");
	}
	else{
		strcpy(mode, "dec");
	}

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(port); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	//first, send the attempted s (enc)
	strcpy(buffer, mode);
	sendData(buffer, socketFD);

	// Get return message from server (ok / error)
	recvData(buffer, socketFD);
	if (strcmp(buffer, "error") == 0){
		error("CLIENT: WRONG SERVER. Quitting.");
		exit(2);
	}

	// Send length
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	sprintf(buffer, "%ld", strlen(k));
	//printf("CLIENT: DEBUG: STRLEN(%s) = %ld.\n", k, strlen(k));
	sendData(buffer, socketFD);
	
	//get confirmation of length
	recvData(buffer, socketFD);
	assert(atoi(buffer) == strlen(k));


	//send A
	sendData(A, socketFD);
	recvData(buffer, socketFD);

	//send K
	sendData(k, socketFD);
	recvData(buffer, socketFD);

	//ask for B
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "next");
	sendData(buffer, socketFD);

	//recieve B
	recv(socketFD, B, strlen(A), MSG_WAITALL);
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "ok");
	sendData(buffer, socketFD);

	close(socketFD);

	printf("%s\n", B);
	
	return;
}