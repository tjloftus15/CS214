#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tj0.h"
struct hash** table;
int main(int argc, char** argv){      
	int numWords = 0;
	if( argc!=2){//if too many arguments passed
		printf("Error: Invalid number of arguements\n");
		return 0;
	}
	table = (struct hash**)malloc(52*sizeof(struct hash*));
	int a=0;
	while(a<52){
		table[a]=NULL;
		a++;
	}
	int i = 0;
	int j = 0;
	char* word; 
	int len = strlen(argv[1]);
	while(i < len){//while we still have words/characters to read in
		if(!isalpha(argv[1][i])){
			i++;
			continue;
		}
     	j = i; 
     	int letters = 0;
     	while(isalpha(argv[1][i])){
		if(i == len){
         	break;
                }
     		letters ++;
     		i++;
     	}
     	word = (char*) malloc(((letters+1) * sizeof(char)));
     	int y = 0;
     	while(j < i){
     		word[y] = argv[1][j];
     		j++;
     		y++;
     	}
     	word[y]='\0';
     	int pos = hash(word[0]);
     	insert(pos , word);
     	numWords++;
     	i++;
	}
findWords(numWords); //final words print         
return 0;
}
