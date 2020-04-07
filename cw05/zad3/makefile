all:
	@gcc -Wall -std=c99 -o producer producer.c
	@gcc -Wall -std=c99 -o consumer consumer.c
	@gcc -Wall -std=c99 -o controller controller.c
	@./controller
	@make clean
clean:
	@rm fif producer controller consumer
