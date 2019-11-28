#!/usr/bin/env python
import socket
import sys
import signal

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
serverName = sys.argv[1]
serverPortNumber = int(sys.argv[2])

if (serverName == 'flip1' || serverName == 'flip2' || serverName == 'flip3'): 
	server_address = (serverName + '.engr.oregonstate.edu', portNumber)
	print 'SERVER: starting up on %s port %s' % server_address
	sock.bind(server_address)