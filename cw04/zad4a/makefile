CC = gcc -Wall -std=c99

all:
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender

clean:
	rm -rf sender catcher

test:
	@./catcher sigrt&
	@./sender `pidof catcher` 50 sigrt
	@echo "\n"
	@./catcher kill&
	@./sender `pidof catcher` 50 kill
	@echo "\n"
	@./catcher queue&
	@./sender `pidof catcher` 50 queue
