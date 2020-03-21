CC = gcc -Wall
LIB_NAME = my_lib
SOURCE_PATH = ../zad2
LIB_PATH = ../zad1


static:
	$(CC) $(SOURCE_PATH)/main.c -o main -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

shared:
	$(CC) $(SOURCE_PATH)/main.c -o main -L$(LIB_PATH) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).so

dynamic:
	$(CC) $(SOURCE_PATH)/main.c -o main -I. -ldl -D DYNAMIC

clean:
	mv raport2.txt raport3a.txt
	rm main tmp.txt

clean_all:
	rm main tmp.txt raport3a.txt raport2.txt

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./main $(shell cat test.txt)

test_all:
	printf "static:\n" > raport2.txt
	make static
	make test
	printf "\nshared:\n" >> raport2.txt
	make shared
	make test
	printf "\ndynamic:\n" >> raport2.txt
	make dynamic
	make test
	make clean
