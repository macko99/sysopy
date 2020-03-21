CC = gcc -Wall
LIB_NAME = my_lib
LIB_PATH = ../zad1


static:
	$(CC) main.c -o main -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a

shared:
	$(CC) main.c -o main2 -L$(LIB_PATH) -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).so

all:
	make static
	make shared

test:
	make static
	./main start create_array 10 compare_pairs a.txt:b.txt end test_one
	make clean

clean:
	rm main main2 tmp.txt

clean_all:
	rm main main2 tmp.txt raport2.txt
