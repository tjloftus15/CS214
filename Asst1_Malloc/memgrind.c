#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "mymalloc.h"
//given case A
double testA(){
   struct timeval start, end;
   double fncTime = 0;
   gettimeofday(&start, NULL);

   int x = 0;
   int ptrCount=0;
   struct chunk * global=(struct chunk*)getFront();
   struct chunk * new= global;
   new->isfree='1';
   new->size=255;
	while(x < 150){
		void * p = malloc(1);
		ptrCount++;
		free(p);
      x++;
   }
  
   gettimeofday(&end, NULL);
   fncTime = (end.tv_sec - start.tv_sec)*1000;
   fncTime += (end.tv_usec - start.tv_usec) / 1000.0;
   return fncTime;
}
//Given case B
double testB(){
	
   struct timeval start, end;
   double fncTime = 0;
   gettimeofday(&start, NULL);

   int ptrCount=0;
   int x=0;
   struct chunk * global=(struct chunk*)getFront();
   struct chunk * new= global;
   new->isfree='1';
   new->size=255;
   void * ptrArray[150];
   while(x<150){
     	void * p=malloc(1);
     	ptrArray[ptrCount]=p;
     	ptrCount++;
     	x++;
   }
   x = 0;
   while(x<150){
     free(ptrArray[x]);
     x++;
   }
   gettimeofday(&end, NULL);
   fncTime = (end.tv_sec - start.tv_sec)*1000;
   fncTime += (end.tv_usec - start.tv_usec) / 1000.0;
   return fncTime;
}
/*
testCDT()
 - testCDT is the given cases C and D, and our testT
*/
double testCDT(char a){
	//NEW start
    struct timeval start, end;
    double fncTime = 0;
    gettimeofday(&start, NULL);
	int B=1;
   int mcount = 0;
   int ptrCount=0;//keeps track of number of current pointers
   struct chunk * global=getFront();
   struct chunk * new= global;
   new->isfree='1';
   new->size=255;
   void * ptrArray[150];
   //NEW end
   while (mcount<150){
   	int randy=rand()%10;
   	if(ptrCount==0){
   		randy=1;
   	}
     	if(randy==0 || randy==2 || randy==3 || randy==7){
     		int Randy2=0;
         Randy2=rand()%ptrCount;
     	   void * yo = ptrArray[Randy2];
         free(yo);
         //shiftAddr start: eliminates the ptr from our ptr array
         //	- it shifts all addresses in the ptrArray left starting at the position of the freed ptr
         int i=Randy2;
         int j=i+1;
         if(ptrCount==0 || i==(ptrCount-1)){
         	ptrArray[i]=NULL;
         }else{
         	while(j<150){
         		void * p=ptrArray[j];
         		ptrArray[i]=p;
         		j++;
         		i++;
         	}
         }
         //end of shiftAddr
         ptrCount--;
     	}
     	else{
     		void* ptr;
     	  	int bytes;
     	  	switch(a){
     	   	case 'C':
			  		ptr=malloc(1);
			  		break;
          	case 'D':
			  		bytes=rand()%65;
			  		if(bytes==0)
			  			bytes=1;
			  		ptr=malloc(bytes);
			  		break;
			  	case 'T':
			  		if(mcount%2==0)
			  			B=15;
			  		else
			  			B=30;
			  		ptr=malloc(B);
			  		break;
         	default:
     				ptr=malloc(1);
     				break;
     	 	}
	  ptrArray[ptrCount]=ptr;
	  ptrCount++;
	  mcount ++;
	  }
	}
	int i=0;
	while(i<ptrCount){
		void * p = ptrArray[i];
		free(p);
		i++;
	}
   gettimeofday(&end, NULL);
   fncTime = (end.tv_sec - start.tv_sec)*1000;
   fncTime += (end.tv_usec - start.tv_usec) / 1000.0;
 	return fncTime;
}
/*
testK()
 - This is one of our testcases
*/

double testK(){
    
    struct timeval start, end;
    double fncTime = 0;
    gettimeofday(&start, NULL);

   int fullcount=0;
   int mcount =0;
   int i=0;
   int size = 48;
   int ptrCount=0;//keeps track of number of current pointers
   struct chunk * global=getFront();
   struct chunk * new= global;
   new->isfree='1';
   new->size=255;
   void * ptrArray[150];
   while(mcount< 150){
		if(fullcount==100){
			free(ptrArray[i]);
			 i ++;
			 ptrCount --;  
			 fullcount--;
			 continue;
    		
    }

	void *ptr = malloc(size);
   ptrArray[mcount] = ptr;
   ptrCount ++;
   mcount++;
   fullcount++;
   }
	int j=0;
	while(j<ptrCount){
		void * p = ptrArray[j];
		free(p);
		j++;
	}
   gettimeofday(&end, NULL);
   fncTime = (end.tv_sec - start.tv_sec)*1000;
   fncTime += (end.tv_usec - start.tv_usec) / 1000.0;
	return fncTime;     

}


int main(int argc, char * argv[]){
	int runs=0;
        double aAvg = 0, bAvg = 0, cAvg = 0, dAvg = 0, kAvg = 0, tAvg = 0;
	while(runs<101){
		aAvg = aAvg + testA();
		clear();
		
		bAvg = bAvg + testB();
		clear();
		
		cAvg = cAvg + testCDT('C');
		clear();

		dAvg = dAvg + testCDT('D');
		clear();
		
      kAvg = kAvg + testK();
      clear();
      
      tAvg = tAvg + testCDT('T');
      clear();  
		
		runs++;
   }

   printf("Avg time sequence A: %f \n", aAvg/100);
   printf("Avg time sequence B: %f \n", bAvg/100);
   printf("Avg time sequence C: %f \n", cAvg/100);
   printf("Avg time sequence D: %f \n", dAvg/100);
   printf("Avg time sequence K: %f \n", kAvg/100);
   printf("Avg time sequence T: %f \n", tAvg/100);

   return 0;
}
