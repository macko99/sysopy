CC = gcc -Wall
LIB_NAME = my_lib
SOURCE_PATH = ../zad2
LIB_PATH = ../zad1
DYN_LIB_PATH = ../zad3a
COMP_OPT = O


static:
	$(CC) $(SOURCE_PATH)/main.c -o main -$(COMP_OPT) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

shared:
	$(CC) $(SOURCE_PATH)/main.c -o main -$(COMP_OPT) -L$(LIB_PATH) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).so

dynamic:
	$(CC) $(SOURCE_PATH)/main.c -o main -I$(DYN_LIB_PATH) -$(COMP_OPT) -ldl -D DYNAMIC

clean:
	mv raport2.txt raport3b.txt
	rm main tmp.txt

clean_all:
	rm main tmp.txt raport3b.txt raport2.txt

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./main $(shell cat ../zad3a/test.txt)

test_all:
	printf "static (O):\n" > raport2.txt
	make static COMP_OPT=O
	make test
	printf "\nshared (O):\n" >> raport2.txt
	make shared COMP_OPT=O
	make test
	printf "\ndynamic (O):\n" >> raport2.txt
	make dynamic COMP_OPT=O
	make test
	printf "static (O2):\n" >> raport2.txt
	make static COMP_OPT=O2
	make test
	printf "\nshared (O2):\n" >> raport2.txt
	make shared COMP_OPT=O2
	make test
	printf "\ndynamic (O2):\n" >> raport2.txt
	make dynamic COMP_OPT=O2
	make test
	printf "static (Os):\n" >> raport2.txt
	make static COMP_OPT=Os
	make test
	printf "\nshared (Os):\n" >> raport2.txt
	make shared COMP_OPT=Os
	make test
	printf "\ndynamic (Os):\n" >> raport2.txt
	make dynamic COMP_OPT=Os
	make test
	make clean
