CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -pthread

all: 
	make clean
	make server
	make client
	./server 5344 socketName

client:
	$(CC) $(CFLAGS) -o client client.c

server:
	$(CC) $(CFLAGS) -o server server.c


client1:
	./client Mateusz local socketName

client2:
	./client Tomek local socketName

client1_host:
	./client Mateusz localhost 5344

client2_host:
	./client Tomek localhost 5344

clean:
	rm -f $(obj) client server socketName
