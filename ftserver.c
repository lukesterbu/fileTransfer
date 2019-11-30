#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <limits.h>

#define BUFFER_SIZE 500
#define DIRECTORY_SIZE 2048

// Error handling function
void error(const char *msg) {
	perror(msg);
	exit(1);
}

// Returns all the files in the current directory as a string
char* getDir() {
	char* allDirectories = (char*)malloc(sizeof(char) * DIRECTORY_SIZE);
	struct dirent* entry;

	DIR* directory = opendir(".");

	if (directory == NULL) {
		return allDirectories;
	}

	memset(allDirectories, '\0', DIRECTORY_SIZE);

	while ((entry = readdir(directory)) != NULL) {
		strcat(allDirectories, entry->d_name);
		strcat(allDirectories, "\n");
	}
	return allDirectories;
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	char buffer[BUFFER_SIZE];
	char allDirectories[DIRECTORY_SIZE];
	char serverHostName[HOST_NAME_MAX];
	char clientHostName[HOST_NAME_MAX];

	// Check usage and args
	if (argc < 2) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	// Get this server's host name
	gethostname(serverHostName, HOST_NAME_MAX + 1);

	// Set up the address struct for this process (the server)
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connecting to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
		error("ERROR opening socket");

	// Enable the socket to being listening and connect socket to port
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can no receive up to 5 connections

	// Let the user know that the server is open for connections
	printf("Server open on %d\n", portNumber);

	while (1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0)
			error("ERROR on accept");

		// Get the message from the client and display it
		memset(clientHostName, '\0', BUFFER_SIZE);
		charsRead = recv(establishedConnectionFD, clientHostName, HOST_NAME_MAX - 1, 0);
		if (charsRead < 0)
			error("ERROR reading from socket");
		// Print out the client host name
		printf("Connection from %s.\n", clientHostName);

		// Print the client port number
		printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

		// Get the message from the client and display it
		memset(buffer, '\0', BUFFER_SIZE);
		charsRead = recv(establishedConnectionFD, buffer, BUFFER_SIZE - 1, 0); // Read the client's message from the socket
		if (charsRead < 0)
			error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);

		// If the command received is equal to -l
		if (strcmp(buffer, "-l") == 0) {
			printf("List directory requested on port %d.\n", ntohs(clientAddress.sin_port));
			printf("Sending directory contents to %s:%d\n", clientHostName, ntohs(clientAddress.sin_port));
			memset(allDirectories, '\0', DIRECTORY_SIZE);
			strcpy(allDirectories, getDir());
			charsRead = send(establishedConnectionFD, allDirectories, DIRECTORY_SIZE - 1, 0);
			if (charsRead < 0)
				error("ERROR writing to the socket");
		}
		// If the command received is equal to -g
		else if (strcmp(buffer, "-g") == 0) {
			// Need code here
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
	return 0;
}