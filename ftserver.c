#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

// Error handling function
void error(const char *msg) {
	perror(msg);
	exit(1);
}

// Returns all the files in the current directory as a string
char* getDir() {
	char* allDirectories[2048] = {'\0'};
	struct dirent* entry;

	DIR* directory = opendir(".");

	if (directory == NULL) {
		return allDirectories;
	}
	while ((entry = readdir(directory)) != NULL) {
		strcat(allDirectories, entry->d_name);
		strcat(allDirectories, '\n');
	}
	return allDirectories;
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	char buffer[500];
	char allDirectories[2048];

	// Check usage and args
	if (argc < 2) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	// Set up the address struct for this process (the server)
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connectin to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
		error("ERROR opening socket");

	// Enable the socket to being listening and connect socket to port
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can no receive up to 5 connections

	while (1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0)
			error("ERROR on accept");
		printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
		// Get the message from the client and display it
		memset(buffer, '\0', 500);
		charsRead = recv(establishedConnectionFD, buffer, 499, 0); // Read the client's message from the socket
		if (charsRead < 0)
			error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);

		// If the command received is equal to -l
		if (strcmp(buffer, '-l')) {
			memset(allDirectories, '\0', 2048);
			allDirectories = getDir();
			charsRead = send(establishedConnectionFD, allDirectories, 2047, 0);
			if (charsRead < 0)
				error("ERROR writing to the socket");
			prinf("SERVER: I sent all the files in the current directory to the client\n");
		}
		// If the command received is equal to -g
		else if (strcmp(buffer, '-g')) {
			// Need code here
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
	return 0;
}