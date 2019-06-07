#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//hash table node format
struct hash{
	int length;
	char * word;
	struct hash * next;
};
extern struct hash ** table;

//hash function based on ascii values of the first character of a word
int hash(char firstLetter){
	int pos;
	int a=isupper(firstLetter);
	switch(a){
		case 0:
			pos=(int)firstLetter;
			pos=pos-97;
			break;
		default:
			pos=(int)firstLetter;
			pos=pos-65+26;
			break;
	}
	return pos;
}

//compares each letter of the word we want to insert with the word at our position in the list
int stringCompare(struct hash * curr, char * word){
	char * listword=curr->word;
	int listlength=curr->length;
	int length=strlen(word);
	int i=1;
	while(i<listlength && i<length){
		if(word[i]<listword[i])
			return 1;
		else if(word[i]>listword[i])
			return 0;
		i++;
	}
	if(length<listlength)
		return 1;
	return 0;
}

//finds proper place to insert a new word in the list at its appropriate table position
void insert( int pos, char * word){
	
	struct hash * node = malloc(sizeof(struct hash));
	node->word=word;
   int L= strlen(node -> word);
   node -> next = NULL;
	node->length=(int)L;
	if(table[pos]==NULL){
		table[pos]=node;
		return;
	}
   struct hash * before;
	struct hash * curr;
	curr=table[pos];
	before=curr;
	while(curr!=NULL){
		int result = stringCompare(curr,node-> word);
		if(result==1){
         if(curr==before){
            node->next=curr;
  	    		table[pos]=node;
            return;
         }
			before->next=node;
			node->next=curr;
			return;
		}
		before=curr;
		curr=curr->next;
	}
	before->next=node;
	return;
}

//the function that actually prints the list at each location in the table
int finalPrint(struct hash * curr, int numWords){
	while(curr!=NULL){
		printf("%s \n", curr->word);
		curr=curr->next;
      numWords--;
	}
	return numWords;
}

//searches down the hash table in order from A-Z to a-z to print
void findWords(int numWords){
	int j=26;
	struct hash * curr;
	while(numWords>0){
		if(table[j]==NULL){
			j++;
		}else{
			curr=table[j];
			numWords=finalPrint(curr, numWords);
			j++;      
		}
		if(j==52)
			j=0;
	}
	return;

}

