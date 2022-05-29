CC=gcc
CFLAGS= -Wall -Wextra

all: tcpc


tcpc: client.c
	$(CC) $(CFLAGS) -o tcp client.c

clean:
	rm -rf *.dYSM tcpc