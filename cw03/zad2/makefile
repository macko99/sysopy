test:
	make clean
	gcc -Wall helper.c -o helper
	gcc -Wall main.c -o main
	./helper create 10 40 100
	./main lista 20 2 2

clean:
	rm -f -r .tmp files helper main lista
