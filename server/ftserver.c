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
#include <fcntl.h>

#define BUFFER_SIZE 	500
#define DIRECTORY_SIZE 	2048
#define COMMAND_SIZE 	3
#define MAX_SIZE 		1000

// Function prototypes
int fileExists(char*);
char* readFile(long*, char*);
void error(const char*);
char* getDir();

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	char buffer[BUFFER_SIZE];
	char fileName[BUFFER_SIZE];
	char command[COMMAND_SIZE];
	char allDirectories[DIRECTORY_SIZE];
	char serverHostName[HOST_NAME_MAX];
	char clientHostName[HOST_NAME_MAX];
	char* fileContents; // will allocate dynamically later
	long fileLength = -5;

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

		// Get the client host name
		memset(clientHostName, '\0', HOST_NAME_MAX);
		charsRead = recv(establishedConnectionFD, clientHostName, HOST_NAME_MAX - 1, 0);
		if (charsRead < 0)
			error("ERROR reading from socket");
		// Print out the client host name
		printf("Connection from %s.\n", clientHostName);

		// Get the command from the client. Either "-l" or "-g"
		memset(command, '\0', COMMAND_SIZE);
		charsRead = recv(establishedConnectionFD, command, COMMAND_SIZE - 1, 0); // Read the client's message from the socket
		if (charsRead < 0)
			error("ERROR reading from socket");

		// If the command received is equal to -l
		if (strcmp(command, "-l") == 0) {
			printf("List directory requested on port %d.\n", ntohs(clientAddress.sin_port));
			printf("Sending directory contents to %s:%d\n", clientHostName, ntohs(clientAddress.sin_port));
			memset(allDirectories, '\0', DIRECTORY_SIZE);
			strcpy(allDirectories, getDir());
			charsRead = send(establishedConnectionFD, allDirectories, DIRECTORY_SIZE - 1, 0);
			if (charsRead < 0)
				error("ERROR writing to the socket");
		}
		// If the command received is equal to -g
		else if (strcmp(command, "-g") == 0) {
			// Get requested file name from client
			memset(fileName, '\0', BUFFER_SIZE);
			charsRead = recv(establishedConnectionFD, fileName, BUFFER_SIZE - 1, 0);
			if (charsRead < 0)
				error("ERROR reading from socket");
			printf("File \"%s\" requested on port %d.\n", fileName, ntohs(clientAddress.sin_port));
			
			// File does exist
			if (fileExists(fileName)) {
				// Get file stats
				printf("Sending \"%s\" to %s:%d\n", fileName, clientHostName, ntohs(clientAddress.sin_port));
				fileContents = readFile(&fileLength, fileName);
				
				// Send the file length to the client
				fileLength = ntohl(fileLength);
				charsRead = send(establishedConnectionFD, &fileLength, sizeof(fileLength), 0);
				if (charsRead < 0)
					error("ERROR writing to the socket");
				
				// Will send file contents in chunks if necessary
				int totalWritten = 0;
				/* while (totalWritten <= fileLength) {
					char copy[MAX_SIZE];
					memset(copy, '\0', sizeof(copy));
					// Copies from where the last iteration left off
					strncpy(copy, &fileContents[totalWritten], MAX_SIZE - 1);
					// Send ciphertext to server
					charsRead = send(establishedConnectionFD, &copy, sizeof(copy), 0); // Write to the server
					if (charsRead < 0) 
						error("CLIENT: ERROR writing to socket");
					totalWritten += charsRead - 1; 
				}*/
			}
			
			// File doesn't exists
			else {
				printf("File not found. Sending error message to %s:%d\n", clientHostName, ntohs(clientAddress.sin_port));
				memset(buffer, '\0', BUFFER_SIZE);	
				strcpy(buffer, "FILE NOT FOUND");
				charsRead = send(establishedConnectionFD, buffer, BUFFER_SIZE - 1, 0);
				if (charsRead < 0)
					error("ERROR writing to the socket");
			}
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
	return 0;
}

// Returns 1 if the file exists and 0 if it doesn't exist
int fileExists(char* fileName) {
	int file = open(fileName, O_RDONLY);
	if (file == -1) {
		return 0;
	}
	close(file);
	return 1;
}

// Returns the first line of the file referred to by fileName
char* readFile(long* fileLength, char* fileName) {
	int file = open(fileName, O_RDONLY);
	// Check to see if file was opened
	if (file == -1) {
		fprintf(stderr, "Could not open %s\n", fileName);
		exit(1);
	}
	// Get the length of the file
	*fileLength = lseek(file, 0, SEEK_END);
	// Reset file pointer to beginning of file
	lseek(file, 0, SEEK_SET); 
	// Dynamically allocate buffer based on file length
	char* buffer = malloc(sizeof(char) * (*fileLength));
	// Read the first line of the file
	if (read(file, buffer, *fileLength) == -1) {
		fprintf(stderr, "There is no content in file %s\n", fileName);
		exit(1);
	}
	buffer[(*fileLength) - 1] = '\0'; // Remove the trailing \n that fgets adds
	// Close the file
	close(file);
	return buffer;
}

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