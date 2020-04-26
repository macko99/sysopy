cc = gcc -Wall -ggdb

all: build_client build_server

build_server:
	$(cc) server.c -o server.out -lrt

build_client:
	$(cc) client.c -o client.out -lrt

clean:
	rm -f *.out
