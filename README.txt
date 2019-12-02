# fileTransfer
Project 2 for CS 372
Author: Luke Burris

Preface:
	- In my opinion, it only makes sense for ftclient and ftserver to be placed in 2 separate directories (folders). During implementation, I placed ftclient in a folder named client and ftserver in a folder named server (the folder names are irrelevant). This allowed me to more accurately see if the file was being transferred correctly. Otherwise, the file would just overwrite the file with the same name because it was transferring the file to the same directory where it already existed.

How to compile all files:
	Run the command "make all" while ftclient.py and ftserver.c are in the same directory as the Makefile

How to clean up files (you shouldn't need this):
	Run the command "make clean" while all files are in the same directory as the Makefile

Usage for ftserver:
	ftserver [port]

Usage for ftclient:
	ftclient [serverName] [serverPort] [command (-l or -g)] [fileName (if command is -g)] [clientPort]

Instructions:
	- Run the ftserver command followed by the relevant ftclient command. In the ftclient command line argument list, the serverName argument should match the server name that ftserver is being run on. Likewise, the serverPort argument should match the port number that ftserver is being run on. 

Command descriptions:
	"-l" will cause ftserver to send a list of all files in the current working directory to ftclient
	"-g" will cause ftserver to send the passed file to ftclient

Extra Credit:
	I implemented a username/password login for the server side. The username is "username" and the password is "password".