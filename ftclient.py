#!/usr/bin/env python
import socket
import sys
import signal

# Shows the user the correct usage and then exits with code 1
def printUsage()
	print 'USAGE: %s [serverName] [serverPort] [command (-l or -g)] [fileName (optional if command is -l)] [clientPort]' % argv[0]
	exit(1)

# Get necessary variables from the command line
if (len(sys.argv) < 5 or len(sys.argv) > 6):
	printUsage()

# Check serverName
if (sys.argv[1] != 'flip1' and sys.argv[1] != 'flip2' and sys.argv[1] != 'flip3'): 
	printUsage()
else:
	serverName = sys.argv[1] + '.engr.oregonstate.edu'

# Check serverPort
if (not(sys.argv[2].isnumeric()):
	printUsage()
else:
	serverPort = int(sys.argv[2])

# Check command
if (sys.argv[3] != '-l' and sys.argv[3] != '-g'):
	printUsage()
else:
	command = sys.argv[3]

# If command was -l then check for clientPort
# Else sys.argv[4] should be the fileName
if (sys.argv[3] == '-l'):
	# Check clientPort in position 4
	if (not(sys.argv[4].isnumeric())):
		printUsage()
	else:
		clientPort = sys.argv[4]
# Command was -g so fileName is in sys.argv[4] and clientPort should be in sys.argv[5]
else:
	fileName = sys.argv[4]
	# Check clientPort in position 5
	if (not(sys.argv[5].isnumeric())):
		printUsage()
	else:
		clientPort = sys.argv[5]

print 'CLIENT: starting up on %s port %s' % server_address

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the socket
sock.connect((serverName,serverPort))

# Close the socket
sock.close()