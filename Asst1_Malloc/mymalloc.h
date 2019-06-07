#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>


#define malloc(x) my_malloc((x), __FILE__,__LINE__)
#define free(x) my_free((x), __FILE__,__LINE__)

//our two byte metadata struct
struct chunk{
 uint8_t size; //size, holds max 255, but we only need max 64 bytes
 char isfree; //0 means no not free, 1 means it is free%
};


void * my_malloc(size_t bytes, char* FILE, int LINE);

void my_free(void * ptr,char*FILE, int LINE);

void initialize(size_t size, struct chunk * lastloc);

void clear();

void * getFront();

struct chunk * getNext(struct chunk * next);

#endif
