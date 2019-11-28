#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char handle[11];
	char buffer[500];

	// Check usage and args
	if (argc < 2) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(0);
	}

	return 0;
}