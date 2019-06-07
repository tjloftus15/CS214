#include "mymalloc.h"
static char myblock[5000];

//returns the address of myblock to memgrind to allow access in memgrind
void * getFront(){
	return (void*)myblock;
}
//gets the next metadata block
struct chunk * getNext(struct chunk * next){
	uint8_t size  = next-> size;
	int dist= sizeof(struct chunk)+ size;
	dist=dist/2;
   struct chunk *N=next+dist;
	return N;
}
//initializes a size to a metadata block
void initialize(size_t size, struct chunk * lastloc){ //now assigns to given address, passed in lastloc
    struct chunk *node;
    node=lastloc;
    node-> isfree = '0';
    node-> size = size;
    return;
}

//our version of malloc()
void * my_malloc(size_t size, char* FILE, int LINE){
	if(size>=4999){
		return NULL;
	}
	struct chunk * global_ptr=(void*)myblock;//reference to front of block
	struct chunk * lastloc;//current location
	if(size%2!=0)
		size++;    
	/*if(size==0)
		size=2;*/
	lastloc=(void*)myblock;
	while(lastloc<(global_ptr+(4998/2))){//main loop. starts at myblock every time, goes to the end
		if(lastloc->isfree=='1'){//found a free chunk
			if(lastloc->size==255){	//found a 'never assigned' chunk (255 is initial never assigned value)
				initialize(size, lastloc);
				struct chunk * next=getNext(lastloc);//gets the next chunk
				next->isfree='1';//makes next chunk free
				next->size=254;//assignes the 'free' size value (after initial chunk)
				return (void*)(lastloc+1);
			}
			if(lastloc->size>=size){//found a location
				if(lastloc->size!=254){//if you found a chunk and it has already been 'assigned'
					lastloc->isfree='0';
				}else{
					//first part makes sure we have the bytes at the end of the array to store the amount we want to
					void* p=(void*)lastloc;
					void* end=(void*)(myblock);
					end=end+5000;	
					if((p+size+2)>end){//if this data block is partially in bounds and out of bounds OR totally out of bounds
						//printf("lastloc      = %p\nmyblock+5000 = %p\np+size+2     = %p\n ------------------\n", p, end, (p+size+2));
						return NULL;
					}
					initialize(size, lastloc);
				}
				struct chunk * next=getNext(lastloc);
				if((next>=(global_ptr+(4998/2)))){//if the next you have found above is out of bounds 
					return (void*)(lastloc+1);// do no initialize next,return your ptr. 
				}
				next->isfree='1'; //if the next you got isnt out of bounds, initialize next
				next->size=254;
				return (void*)(lastloc+1);
			}
		}
		lastloc=getNext(lastloc);
		if(lastloc>=(global_ptr+(4998/2))){//if you reached the end and didnt find it
			return NULL;
		}
		
	}
	return NULL;
}	

//our version of free()
void my_free(void * ptr,char*FILE, int LINE){
	void * global = (void*)myblock;
	
	if(ptr==NULL){//error, trying to free a NULL ptr
		printf("ERROR in %s line: %d: Attempt to free() a NULL pointer\n", __FILE__,__LINE__);
		return;
	}
	//printf("ptr = %p, global = %p, global+5000 = %p\n", ptr, global, (global+(5000)));
	if(ptr<=global || ptr>=(global+(5000))){
		printf("ERROR in %s line: %d: Attempt to free() a pointer not allocated by malloc()\n", __FILE__,__LINE__);
		return;
	}
	unsigned long long addr =  (unsigned long long )ptr; //gets address of given ptr
	addr=addr-2;
	struct chunk *curr;
   curr = (void*)addr;//creates struct reference to enable changing of isfree
	curr->isfree='1';
   return;
}
//resets myblock
void clear(){
	int i=0;
	while(i<5000){
		unsigned long long addr= (unsigned long long)myblock[i];
		addr=(char)'0';
		addr++;
		i++;
	}
	return;
}



