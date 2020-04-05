all:
	@gcc -Wall -std=c99 -o main main.c
	@gcc -Wall -std=c99 -o exe exe.c

test:
	./main ignore n
	./main pending n
	./main mask n
	./main handler n
	./main ignore exe
	./main pending exe
	./main mask exe
	./main handler exe

clean:
	rm -rf main exe
