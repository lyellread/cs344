#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <assert.h>

#include "otp_core.h"

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int sendData(char * buffer, int socketFD){

	int charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	return charsWritten;
}

int recvData(char * buffer, int socketFD){

	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	int charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	return charsRead;

}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[SIZE];
    
	//if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	char A[SIZE];
	FILE * A_file = fopen(argv[1], "r");
	fgets(A, SIZE, A_file);
	char * A_real = A;
	
	A[strcspn(A, "\n")] = '\0';

	char k[SIZE];
	FILE * k_file = fopen(argv[2], "r");
	fgets(k, SIZE, k_file);
	
	k[strcspn(k, "\n")] = '\0';
	char * k_real = k;


	char B[SIZE];

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
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
	strcpy(buffer, "enc");
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

	// // Send first message to server
	// int charsToSend = strlen(A);
	// charsWritten = 0;

	// while (charsWritten < charsToSend){
		
	// 	memset(buffer, '\0', sizeof(buffer));
	// 	if (strlen(A_real) <2048){
	// 		charsWritten += sendData(A_real, socketFD);
	// 	}
	// 	else{
	// 		strncpy(buffer, A_real, 2048);
	// 			buffer[2048] = '\0';
	// 		A_real += 2048 * sizeof(char);
	// 		charsWritten += sendData(buffer, socketFD);
	// 	}

	// 	memset(buffer, '\0', sizeof(buffer));
	// 	recvData(buffer, socketFD);
	// }

	// //tell the server to break
	// strcpy(buffer, "next");
	// sendData(buffer, socketFD);

	// //wait for reply
	// recvData(buffer, socketFD);

	// // Send second message to server
	// charsToSend = strlen(k);
	// charsWritten = 0;

	// while (charsWritten < charsToSend){
		
	// 	memset(buffer, '\0', sizeof(buffer));
	// 	if (strlen(k_real) <2048){
	// 		charsWritten += sendData(k_real, socketFD);
	// 	}
	// 	else{
	// 		strncpy(buffer, k_real, 2048);
	// 			buffer[2048] = '\0';
	// 		A_real += 2048 * sizeof(char);
	// 		charsWritten += sendData(buffer, socketFD);
	// 	}

	// 	memset(buffer, '\0', sizeof(buffer));
	// 	recvData(buffer, socketFD);
	// }

	// strcpy(buffer, "eot");
	// sendData(buffer, socketFD);

	// recvData(buffer, socketFD);

	// strcpy(buffer, "start");
	// sendData(buffer, socketFD);

	// //copy the len
	// recvData(buffer, socketFD);
	// sendData(buffer, socketFD);

	// int charsToRecv = atoi(buffer);
	// charsRead = 0;

	// //sendData(buffer, socketFD);

	// while (1){

	// 	recvData(buffer, socketFD);

	// 	if(strcmp(buffer, "eoc") == 0){

	// 		// memset(buffer, '\0', SIZE);
	// 		// strcpy(buffer, "eoc");
	// 		// sendData(buffer, socketFD);
			
	// 		break;
	// 	}
	// 	else{
	// 		strcat(B, buffer);
	// 	}

	// 	memset(buffer, '\0', SIZE);
	// 	strcpy(buffer, "ok");
	// 	sendData(buffer, socketFD);
	// }

	// if (charsToRecv != strlen(B)){
	// 	error("CLIENT: ERROR: Read Length Discrepancy");
	// }

	close(socketFD);

	printf("%s\n", B);

	
	return 0;
}
