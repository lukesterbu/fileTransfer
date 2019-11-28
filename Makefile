ftclient:
	chmod +x ftclient

ftserver:
	gcc -o ftserver ftserver.c

all: ftclient ftserver

clean:
	rm -f ftserver