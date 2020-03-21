CC = gcc -Wall
LIB_NAME = my_lib
SOURCE_NAME = library


static:
	$(CC) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

all:
	make static
	make shared

clean:
	rm -f *.o

clean_all:
	rm $(LIB_NAME).a $(LIB_NAME).so
