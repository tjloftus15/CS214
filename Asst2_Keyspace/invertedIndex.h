#ifndef _INVERTEDINDEX_H
#define _INVERTEDINDEX_H

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<errno.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<ctype.h>
#include<unistd.h>

struct node{
	char * file;
	struct node * next;
	int occ;
};

struct words{
	char * word;
	struct node * files;
	struct words * next;
        struct words *previous;
};

struct blarp{
	char * toke;
	uint8_t size; //0 means node not full, 1 means node full
	struct blarp * next;
};

struct rd{
	struct dirent * fileptr;
	DIR * dirptr;
	struct rd * next;
	struct rd * prev;
	char * path;
};

struct hash{
	struct words * head;
};

char * toLowerCase(char *);

char * cat_new_path(char *, char *);

char * readOverflow();

int getHash(char);

int comparer(char *, char *);

int bufferCheck(char *);

void cleanUpOverflow();

int NULL_check();

int comparerCaseFinder(char, char);

struct words * createWords(char *);

struct node * createNode(char *);

void * findFile(void *, char *);

char * getFileName(char * path);

void printToFile(char* outFile, int status);

struct node ** file_sorter(struct node *);

#endif
