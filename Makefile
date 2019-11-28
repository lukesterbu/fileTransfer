ftclient:
	chmod +x ftclient.py

ftserver:
	gcc -o ftserver ftserver.c

all: ftclient ftserver

clean:
	rm -f ftserver