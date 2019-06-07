#ifndef _LIBNETFILES_H
#define _LIBNETFILES_H

#ifdef MUTEX
pthread_mutex_t errnoLock;
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>

#define INVALID_FILE_MODE 132 
#define errprinter "INVALID_FILE_MODE"

struct director{
	int msg_len;
	char * arg1; //operation
	char * arg2; //path(open) or fd(read,write,close)
	char * arg3; //mode(open) or bytes(read,write) or NULL if close
};
struct passer{
	int newsockfd;
	uint32_t clientID;
};

struct users{
	uint8_t open_type;
	uint8_t mode;//RD, WR, RDWR
	uint32_t user;//user who opened the file
	struct users * next;
};
struct node{
	char * path;//file
	struct users * first;//list to people who has this file open
	struct node * next;
};

struct hash_table{
	struct node * first;
};

struct deleter{
	struct node * path_curr;
	struct node * path_before;
	struct users * user_curr;
	struct users * user_before;
};
struct open_mode{
	uint32_t clientID;
	int o_type;
	//char * init_path;
	struct open_mode * next;
};

int check_complete(char * h);
int check_completer(char * h);

int hash(char * path);

void insert_open_type(uint32_t clientID, int o_type);

int get_open_type(uint32_t clientID);

int delete_user(int h, void * args);

int search_directory(char * fdpath, int userID, int a);

int check_open_type(char * fdpath, int a);

struct director * input_decoder(char * buffer);

int netserverinit(char * hostname, int filemode);

ssize_t netopen(const char * pathname, int flags);

void * thread_director(void * args);

ssize_t netwrite(int fildes, void* buf, size_t nbyte);

int netclose(int fd);

#endif
