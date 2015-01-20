CC=gcc

CFLAGS=-Wall -g 

all:  server client
 server: server.c cache.c
	$(CC) $(CFLAGS) -o server server.c 
 client: client.c 
	$(CC) $(CFLAGS) -o client client.c
clean:
	rm -rf *o server client


