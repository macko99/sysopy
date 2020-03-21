CC = gcc -std=c11


clean:
	rm dane dane2 main


test:
	$(CC) -o main main.c

	./main generate dane 1000 1
	./main copy dane dane2 1000 1 sys
	./main copy dane dane2 1000 1 lib
	./main sort dane 1000 1 sys
	./main sort dane2 1000 1 lib

	./main generate dane 1000 4
	./main copy dane dane2 1000 4 sys
	./main copy dane dane2 1000 4 lib
	./main sort dane 1000 4 sys
	./main sort dane2 1000 4 lib

	./main generate dane 1000 512
	./main copy dane dane2 1000 512 sys
	./main copy dane dane2 1000 512 lib
	./main sort dane 1000 512 sys
	./main sort dane2 1000 512 lib

	./main generate dane 1000 1024
	./main copy dane dane2 1000 1024 sys
	./main copy dane dane2 1000 1024 lib
	./main sort dane 1000 1024 sys
	./main sort dane2 1000 1024 lib

	./main generate dane 1000 4096
	./main copy dane dane2 1000 4096 sys
	./main copy dane dane2 1000 4096 lib
	./main sort dane 1000 4096 sys
	./main sort dane2 1000 4096 lib

	./main generate dane 1000 8192
	./main copy dane dane2 1000 8192 sys
	./main copy dane dane2 1000 8192 lib
	./main sort dane 1000 8192 sys
	./main sort dane2 1000 8192 lib


		./main generate dane 5000 1
		./main copy dane dane2 5000 1 sys
		./main copy dane dane2 5000 1 lib
		./main sort dane 5000 1 sys
		./main sort dane2 5000 1 lib

		./main generate dane 5000 4
		./main copy dane dane2 5000 4 sys
		./main copy dane dane2 5000 4 lib
		./main sort dane 5000 4 sys
		./main sort dane2 5000 4 lib

		./main generate dane 5000 512
		./main copy dane dane2 5000 512 sys
		./main copy dane dane2 5000 512 lib
		./main sort dane 5000 512 sys
		./main sort dane2 5000 512 lib

		./main generate dane 5000 1024
		./main copy dane dane2 5000 1024 sys
		./main copy dane dane2 5000 1024 lib
		./main sort dane 5000 1024 sys
		./main sort dane2 5000 1024 lib

		./main generate dane 5000 4096
		./main copy dane dane2 5000 4096 sys
		./main copy dane dane2 5000 4096 lib
		./main sort dane 5000 4096 sys
		./main sort dane2 5000 4096 lib

		./main generate dane 5000 8192
		./main copy dane dane2 5000 8192 sys
		./main copy dane dane2 5000 8192 lib
		./main sort dane 5000 8192 sys
		./main sort dane2 5000 8192 lib

		make clean
