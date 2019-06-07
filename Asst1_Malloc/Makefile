all: Test1 Test2

CC = gcc

Cflags = -g -Wall -Werror -fsanitize=address

Test1:
	$(CC) $(Cflags) -c mymalloc.c

Test2: 
	$(CC) $(Cflags) -o memgrind memgrind.c mymalloc.o 

clean: 
	rm memgrind mymalloc.o
