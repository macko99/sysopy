all:
	@make test
	@make clean

test:
	@gcc -std=c99 -Wall -o main main.c
	@./main file.txt

clean:
	@rm main
