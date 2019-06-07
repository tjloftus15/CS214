all: Libnetfiles Netfileserver

CC = gcc

Cflags = -g -Wall -Werror -fsanitize=address

Cflagsc = -fsanitize=address

Netfileserver:
	$(CC) -o netfileserver netfileserver.c libnetfiles.o -lpthread
	
Libnetfiles:
	$(CC) -c libnetfiles.c -lpthread

Client:
	$(CC)  -o newclient newclient2.c libnetfiles.o -lpthread 
	
clean:
	rm netfileserver libnetfiles.o 
