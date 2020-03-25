test:
	make clean
	gcc -Wall helper.c -o helper
	gcc -Wall main.c -o main
	./helper create 10 70 100
	./main lista 20 1 1 10 10


clean:
	rm -f -r .tmp files helper main lista
