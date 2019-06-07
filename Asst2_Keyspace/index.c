#include "invertedIndex.h"

struct hash ** letterPtrs;
//struct words * head=NULL;
struct blarp * st=NULL;
struct blarp * nd=NULL;
struct rd * base=NULL;
struct rd * last=NULL;

int bufferCheck(char * buffer){
	int i=0;
	while(i<10){
		if(buffer[i]=='\0' || buffer[i]<32 || buffer[i]>126){
			i++;
			if(i==10)
				return -1;
		}
		else if(isalnum(buffer[i])){
				return 1;
			}
		i++;
	}
	return -1;
}

//good
char * toLowerCase(char * word){
	int length = strlen(word);
	int i=0;
	while(i<length){
		if(isalpha(word[i]))
			word[i]=tolower(word[i]);
		i++;
	}
	return word;
}
void cleanUpOverflow(){
	if(st==NULL)
		return;
	if(st!=NULL && nd==st){
		free(st);
		st=NULL;
		return;
	}
	else if(st!= NULL && nd!=st){
		struct blarp * l=st->next;
		nd=st;
		while(nd!=NULL){
			free(nd);
			nd=l;
			l=l->next;
		}
	
	}
	return;
}

int NULL_check(){
	int hash=0;
	int nonNULL=0;
	struct hash * table = letterPtrs[hash];
	while(hash<36){
		table=letterPtrs[hash];
		if(table->head!=NULL)
			nonNULL++;
		hash++;
	}
	if(nonNULL>0)
		return 1;
	return 0;
}

// unknown
/*void printer(){
	int hash=0;
	while(hash<36){
		struct words * t = letterPtrs[hash]->head;
		printf("HEAD --> ");
		while(t!=NULL){
			printf("%s --> ", t->word);
			t=t->next;
		}	
		printf("\n");
		hash++;
	}
	
	
	return;
}*/
char * cat_new_path(char * path, char * newDir){
	char * in_file = newDir; 
   //printf("file_name: \"%s\"\n",in_file);          
   char *path_fd = malloc(strlen(path)+strlen(in_file)+2);       
	strcpy(path_fd, path);
	int len = strlen(path_fd);
	path_fd[len] = '/';
   strcat(path_fd, in_file); 
   return path_fd;
}

char * readOverflow(){
	struct blarp * traveler = st;
	int total = 0;
	while(traveler!=NULL){
		total = total+traveler->size;
		traveler=traveler->next;
	}
	traveler=st;
	char * combo = (char *)malloc((total+1)*sizeof(char));
	int i=0;//current spot in combo
	int j=0;//current spot in traveler->toke
	while(i<(total)){
		while(j<traveler->size){
			combo[i]=traveler->toke[j];
			i++;
			j++;
			
		}
		j=0;
		traveler=traveler->next;
		if(traveler ==NULL)
			break;
	}
	combo[total]='\0';
	return combo;
}

//this function assumes 'char a' is either 0-9 or a-z
int getHash(char a){
	if(a<97)//if it is a number, add 10 since letters come first
		a=a-48+26;
	else//it is a letter, 
		a=a-97;
	return a;
}

int comparerCaseFinder(char curr, char new){
	if(isdigit(curr) && isdigit(new))
		return 1;
	if(isdigit(curr) && isalpha(new))
		return 2;
	if(isalpha(curr) && isdigit(new))
		return 3;
	if(isalpha(curr) && isalpha(new))
		return 4;
	if(curr=='.' && new=='.')
		return 5;
	if(curr=='.' && new!='.')
		return 6;
	if(curr!='.' && new=='.')
		return 7;
	return -1;
}

int comparer(char * curr, char * new){
	int r =strcmp(curr, new);
	if(r==0)//if the current word is the same as the new word
		return 1;
	int currLen=strlen(curr);
	int newLen=strlen(new);
	int i=0;
	while(i<currLen && i<newLen){
		int c = comparerCaseFinder(curr[i], new[i]);
		switch(c){
			case 1: //both curr[i] && new[i] are a number
				if(new[i]<curr[i])
					return 1;
				if(new[i]>curr[i])
					return 0;
				break;
			case 2: //curr[i] is a number, new[i] is a letter
				return 1;
			case 3: //curr[i] is a letter, new[i] is a number
				return 0;
			case 4: //both curr[i] && new[i] are a letter
				if(new[i]<curr[i])
					return 1;
				if(new[i]>curr[i])
					return 0;
				break;
			case 5:
				break;
			case 6://curr[i] == '.'
				return 1;
			case 7://new[i] == '.'
				return 0;
		}
		i++;
	}
	if(newLen<currLen && new[i-1]==curr[i-1])//if you didnt find a place where
		return 1;
	return 0;
}


//creating a new word, new word encountered
struct words * createWords(char * a){
	struct words * first = (struct words *) malloc(sizeof(struct words));
	first->word=a;
	return first;
}

//creating a new node, new file encountered
struct node * createNode(char * a){
	struct node * f1 = (struct node *) malloc(sizeof(struct node));
	f1->file=a;
	f1->occ=1;
	return f1;
}
//traverses file list
void * findFile(void * files, char * file){
	struct node * cfile = (struct node *)(files);
	while(cfile!=NULL){
		int r=strcmp(cfile->file, file);
		if(r==0)
			return (void*)(cfile);
		cfile=cfile->next;
	}
	return NULL;
}
//sorts one file list
struct node ** file_sorter(struct node * head){
	//printf("-----------------------\n");
	if(head->next==NULL){
		//printf("head->next==NULL\n");
		return NULL;
	}//printf("after initial NULL check\n");
	int numFiles=0;
	struct node * t1 = head;
	while(t1!=NULL){
		numFiles++;
		t1=t1->next;
	}
	//printf("numFiles == %d\n", numFiles);
	//printf("after file count\n");
	numFiles++;
	struct node ** ans=(struct node **)malloc(numFiles*sizeof(struct node *));
	t1=head;
	char * lowest;
	int i=0;
	struct node * t2=t1->next;
	struct node * smallest;
	while(t1!=NULL){
		//printf("t1->file == %s\n", t1->file);
		
		char * sfile=t1->file;
		lowest=sfile;
		smallest=t1;
		while(t2!=NULL){
			if(t2->file==NULL){
				t2=t2->next;
				continue;
			}
			int result = comparer(t2->file, sfile);
			if(result==0){
				lowest=t2->file;
				smallest=t2;
			}
			t2=t2->next;
			
		}
		if(smallest==t1){
			t1=t1->next;
			if(t1!=NULL){
				while(t1->file==NULL){
					t1=t1->next;
					if(t1==NULL)
						break;
				}
			}
		}
		ans[i]=(struct node *)malloc(sizeof(struct node));
		ans[i]->file=lowest;
		//printf("assigned lowest of %s\n", lowest);
		ans[i]->occ=smallest->occ;
		smallest->file=NULL;
		t2=t1;
		i++;
		
				
	}
	//printf("after\n");
	ans[numFiles-1]=(struct node *)malloc(sizeof(struct node));
	ans[numFiles-1]->occ=-1;
	//printf("assigned -1 occ\n");
	return ans;
}

char * getFileName(char * path){
       //printf("the path sent in is: %s\n", path);
       int fnLen = 0;
       int i = strlen(path)+1;
       int dc = 0;
       
       int j= 0;

       while(i>=0){
            if (path[i] == '/' || i==0){
                fnLen = j-1;
                   //printf("the length of the filname is : %d\n", fnLen);     
                break;
            }
            
          
          i--;
          j++;
       }   

      char *fn = malloc(fnLen *sizeof(char));
      j=0;
      memcpy(fn, &path[strlen(path)-fnLen], fnLen);
     /*while (i <strlen(path)){
           fn[j] = path[i];
           i ++;
           j++;

      } */ 
     //fn[j+1] = '\0';
    // printf("the file name is %s\n", fn);    



return fn;
}

void printToFile(char* outFile, int status){
      const char * oFile = outFile;
      errno =0;
       int len = strlen(oFile);
      //checking the access of the user-provided output file 
       if (len>256){
           printf("The user-provided filename is too long\n");

           return;
       }
       if (strcmp(oFile, "invertedIndex")==0){
             printf("Error: Cannot write to a file in use, Program Terminated Enter New File\n");
             return;
       }
       if (access(oFile, F_OK|R_OK|W_OK)==-1){
            
              
              switch(errno){
                //printf("errno is %d\n", errno);
              	case 13: 
              		printf("Permission has been denied, please enter a new output file\n");
              		return;
              		
              	case 36:
              		printf("The user-provided filename is to long, enter a name less than 256 characters\n");
              		return;
              		break;
              	case 2:
 					//need to create the file and write to it 
              		break;
              	case 30:
              		printf("Permission has been denied, the access on the user-provided file is READONLY\n");
              		return;
              		break;
              	case 26:
              		printf("Permission has been denied, cannot write to a file in use\n");
              		return;

              }
       }
		//printf("errno is = %d\n", errno);
		if(errno==0){
		printf("The user-provided file already exists, do you wish to overwrite the file?\nEnter: 'y'for yes,  OR,  'n' for no. trailing input will be ignored\n");
       	        char answer = 0;
   	   int check = 0;
   	   while(check!=1){
   	    	scanf("%c", &answer);
                fseek(stdin,0,SEEK_END);	
   	   	if (answer=='y'){
                        printf("...Program Complete\n");
       			check =1;
   	   	}else if (answer=='n'){
   	 			printf("Program Terminated: File not written\n");
                                
   	   		return;
   	   	}else 
               if (check !=2){ 
                   printf("Invalid answer, please enter 'y' or 'n'\n"); 
                   check = 2;
                   continue;} else check = 0; continue; 
   	    }
              		//break;
		}
		if(status==-1){
			FILE * of = fopen(oFile, "w");
       	fprintf(of,"<\"?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
         fprintf(of,"<fileIndex>\n");
         fprintf(of,"<fileIndex>\n");
         return;
		}
       //check to s
       //if (errno == 2){
       	 FILE * of = fopen(oFile, "w");
       	 fprintf(of,"<\"?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
         fprintf(of,"<fileIndex>\n");
         int hash=0;
       	while(hash<36){
       		//printf("hash == %d\n", hash);
       		struct words * curr = letterPtrs[hash]->head;
            //struct node *fList = head->files;
            int r=strcmp(curr->word, "!");
				while(curr!=NULL && r!=0){
				 	//printf("current word = %s\n", curr->word);
       			fprintf(of, "\t</word>\n");
		         fprintf(of, "\t<word text=\"%s\">\n", curr->word);  
			    	struct node * cfile = curr->files;
			    	//printf("before file sorter\n");
			    	struct node ** ordered_files=file_sorter(cfile);
                               
			    	if(ordered_files==NULL){
						fprintf(of,"\t\t<file name=\"%s\">%d</file>\n", cfile->file, cfile->occ);
			    	}else{
						cfile=ordered_files[0];
						int f=1;
						while(cfile->occ!=-1){
							//printf("cfile->file == %s\n", cfile->file);
							fprintf(of,"\t\t<file name=\"%s\">%d</file>\n", cfile->file, cfile->occ);
				         cfile=ordered_files[f];
				         f++;
		         	}
             	}
             	//printf("\n");
            
             
             curr=curr->next;
              
          }
              hash++;
              if(hash>=36)
              	break;
              curr=letterPtrs[hash]->head;
                      
       		}
                fprintf(of, "\t</word>\n");
       		fprintf(of,"<fileIndex>\n");
	return;
}



int main(int argc, char ** argv){
	int singleFile=0;
	char * single=0;
	//printf("pathName is:  %s\n", argv[2]);
        char *ouFile = argv[1];
	char * path = argv[2];
        int g=0;
   letterPtrs = (struct hash **)malloc(36*sizeof(struct hash*));
   while(g<36){
   	struct words * start=(struct words *)malloc(sizeof(struct words));
   	struct hash * table=(struct hash *)malloc(sizeof(struct hash));
   	letterPtrs[g] = table;
   	start->word="!";
   	table->head=start;
   	g++;
   }
	struct dirent * dp;
	if(argc!=3){//checking arguements passed in
		printf("Error: Invalid number of arguements\n");
		return 0;
	}
	DIR *dirFD;
	errno=0;
	//this tests the initial path
	dirFD=opendir(path);
	if(dirFD == NULL){
		switch(errno){
			case 13:
				printf("Error: You do not have permission to access the given file path \n");
					return 0;
                                break;
			case 2:
				printf("Error: The given path is not a File or Directory\n");
                                return 0;
                                break; 
			case 9:
				printf("Error: The given input is not a file \n");
					return 0;
                                break;
			case 20:
				 //printf("process the input as a file\n");
				 singleFile=1;
				 int check = strcmp(argv[1], argv[2]);
				 if(check==0){
				 	printf("Error: Cannot attempt to only read the file you are writing to\n");
				 	return 0;
				 }
				 //get file name from path
                                 break;
		}
	}
	//below skips "." and ".."
	if(singleFile==0){
		dp=readdir(dirFD);
		dp=readdir(dirFD);
		while(dp!=NULL){
			errno=0;
			dp=readdir(dirFD);
			if(dp==NULL){
		                      //printf("errno after read attempt is %d \n", errno);
					if(base==NULL)//if no more files to read, no more directories to return to
						break;
					if(last == base){
						dp=last->fileptr;
						dirFD=last->dirptr;
						path=last->path;
						free(last);
						base=NULL;
						last=base;
						continue;
					}else{
						struct rd * back = last;
						//last=last->prev;
					
						last->next=NULL;
						dp=last->fileptr;
						dirFD=last->dirptr;
						path=last->path;
						last=last->prev;
						free(back);
						continue;
					}

			}
			ssize_t bytesRead=0;
			nd=st;
		
		   errno =0;      
			uint8_t foundAlnum=0;
			char * newPath=cat_new_path(path, dp->d_name);
			errno=0;
			int fd= open(newPath, O_RDONLY);
		   if (fd ==-1){
				printf("Error: you do not have read access to %s. Continuing on to additional files\n", dp->d_name);
		   }
		   char * file=dp->d_name;
			file=toLowerCase(file);
			uint32_t type = (uint32_t)(dp->d_type);
			if(type==8){
				//printf("reading in from file \"%s\" \n", file);
			}
			//printf("\n");
			if(type==4){
				//printf("encountered directory \"%s\" \n", file);
				//must check errno
				struct rd * new = (struct rd *) malloc(sizeof(struct rd));
				new->fileptr=dp; //current file ptr NULL IS TEMP
				new->dirptr=dirFD;
				new->path=path;
				if(base==NULL){
					base = new;
					last=new;
				}else{
					last->next=new;
					new->prev=last;
					last=new;
				}
				path = cat_new_path(path, dp->d_name);
				dirFD=opendir(path);
		                   
				if(errno==13){
					printf("You do not have permission to access directory: %s. Resuming most recent directory\n", path);
					if(base==NULL)//if no more files to read, no more directories to return to
						break;
					if(last == base){
						dp=last->fileptr;
						dirFD=last->dirptr;
						path=last->path;
						free(last);
						base=NULL;
						last=base;
						continue;
					}else{
						struct rd * back = last;
						//last=last->prev;
						//free(back);
						last->next=NULL;
						dp=last->fileptr;
						dirFD=last->dirptr;
						path=last->path;
						last=last->prev;
						free(back);
						continue;
					}

				}else{
				//skips "." and ".."
					dp=readdir(dirFD);
					dp=readdir(dirFD);
					continue;
				}
			}
			char * buffer = (char *)calloc(10, sizeof(char));
			cleanUpOverflow();
			int i=0;
			int start=0;
			int end;
			char * word;
			word="!";
			//printf("---------------------------\n");
			//printf("evaluating %s\n", file);
			if(!strcmp(file, argv[1])){
				continue;
			}
			int overflow=0;
			bytesRead++;
			//printf("total bytes = %hu\n", dp->d_reclen);
			unsigned short totalRead=0;
			while(bytesRead!=0){
				int size;
				if(i==0){
					size=0;
					bytesRead=read(fd, buffer, 10);
					totalRead=totalRead+bytesRead;
					//printf("\n");
					//printf("current buffer: %s\n", buffer);
					//printf("bytesRead = %ld\n", bytesRead);
					int b = bufferCheck(buffer);
					while(b==-1){
						bytesRead=read(fd, buffer, 10);
						if(bytesRead==0 || bytesRead==-1)
							break;
						b=bufferCheck(buffer);
				
					}
				}
				if(bytesRead==0 || bytesRead==-1)
					break;
				while(!strcmp(word, "!")){
					if(i>=10)
						break;
					if(isalnum(buffer[i])){
						i++;
						if(i>=10){
							overflow=1;
							break;
						}
						continue;
					}
				
					if(buffer[i]=='\0' && strlen(buffer)<10){
						bytesRead=11;
						break;
					}
					buffer[i]='\0';
					int notAlphaStart=i;
					while(!isalnum(buffer[i])){
						i++;
						if(i>=10){
							//i--;
							break;
						}
					}
					i--;
					end=notAlphaStart;
					end=i;
					size=end-start;
					if(size==0 || !isalnum(buffer[start])){
						if(st==NULL){
							i++;
							start=i; 
							continue;
						}
					}
					//printf("i== %d, start== %d, end == %d, size == %d\n", start, end, size);
					if(st!=NULL){
						char * temp=(char *) malloc(size*(sizeof(char)));
						memcpy(temp, &buffer[start], end);
						//printf("copied temp == %S\n", temp);
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						nd->next = buff;
						nd=buff;
						char * bigWord = readOverflow();
						word=bigWord;
						st=NULL;
						nd=st;
				
					}else{
						char * temp=(char *) malloc(size*(sizeof(char)));
						memcpy(temp, &buffer[start], end);
						word=temp;
					}
				
					i++;
					start=i;
					foundAlnum=0;
				}
				if(start<10 && overflow==1 && !strcmp(word, "!")){
					size=10-start;
					char * temp=(char *) malloc(size*(sizeof(char)));
					memcpy(temp, &buffer[start], 9);
					if(st==NULL){
						//printf("temp = %s, assigning to st\n", temp);
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						st=buff;
						nd=st;
					}else{
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						nd->next = buff;
						nd=buff;
					}
					start=0;
					end=0;
					word="!";
					overflow=0;
					i=0;
					while(i<10){
						buffer[i]='\0';
						i++;
					}
					i=0;
					continue;
				}
				if(!strcmp(word, "!"))
					break;
				word = toLowerCase(word);
				//printf("evaultating %s\n", word);
				if(word==NULL || !isalnum(word[0])){
				
					continue;
				}
				int hash = getHash(word[0]);
				file=dp->d_name;
				struct node * f1 = createNode(file);
				struct words * head =letterPtrs[hash]->head;//word node
				int h= strcmp(head->word, "!");
				void * ptr;
				if(h==0){//if that pos is null in letterPtrs
					head->word=word;
					head->files=f1;		
				}else{
					struct words * w1 = createWords(word);
					struct words * curr=head;
					struct words * before=curr;
					int result=0;
					while(curr!=NULL){
						result = comparer(curr->word, word);
						if(result==1)
							break;
						before=curr;
						curr=curr->next;
					}
					int r=-2;
					if(before->next!=NULL)
						r = strcmp(before->next->word, word);
					if(result==0){//never found word's place,goes at end
						before->next=w1;
						w1->files=f1;
					}else if(result==1 && before==curr){//word goes first
						r =strcmp(before->word, word);
						if(r!=0){//if word did not match with first in list
							w1->next=head;
							letterPtrs[hash]->head=w1;
							w1->files=f1;
						}else{
							ptr=findFile((void*)(before->files), file);
							if(ptr==NULL){
								f1->next=before->files;
								before->files=f1;
							}else{
								struct node * t3 = (struct node *)(ptr);
								t3->occ=t3->occ+1;
							}
						}
					}else if(r==0){ //found a match
						ptr = findFile((void*)(before->next->files), file);
						if(ptr==NULL){
							before=before->next;
							f1->next=before->files;
							before->files=f1;
						}else{
							//printf("i found the file\n");
							struct node * t3 = (struct node *)(ptr);
							t3->occ=t3->occ+1;
						}
					}else{//found where a new word goes, between before and curr
						w1->next=before->next;
						before->next=w1;
						w1->files=f1;
					}
					
				}
				//printer();
				//printf("---------\n");
				word="!";
				if(i>=10){
					start=0;
					end=0;
					i=0;
					overflow=0;
					while(i<10){
						buffer[i]='\0';
						i++;
					}
					i=0;
				}
			}
		
			close(fd);
		}
	}
	else if(singleFile==1){
		errno=0;
		
		int fd= open(argv[2], O_RDONLY);
		  if (fd ==-1){
			printf("Error: you do not have read access to %s. This is the only passed file. Not able to make Inverted Index\n", dp->d_name);
			return 0;
		  }
		 // printf("in single file\n");
		char * file= getFileName(argv[2]);
                 //printf("HEYYY file name is %s\n", file);
                
		file=toLowerCase(file);
              
		char * buffer = (char *)calloc(10, sizeof(char));
			cleanUpOverflow();
			int i=0;
			int start=0;
			int end;
			char * word;
			word="!";
			//printf("---------------------------\n");
			//printf("evaluating %s\n", file);
			int overflow=0;
			size_t bytesRead;
			bytesRead++;
			int foundAlnum=0;
			//printf("total bytes = %hu\n", dp->d_reclen);
			unsigned short totalRead=0;
			while(bytesRead!=0){
				int size;
				if(i==0){
					size=0;
					bytesRead=read(fd, buffer, 10);
					totalRead=totalRead+bytesRead;
					//printf("\n");
					//printf("current buffer: %s\n", buffer);
					//printf("bytesRead = %ld\n", bytesRead);
					int b = bufferCheck(buffer);
					while(b==-1){
						bytesRead=read(fd, buffer, 10);
						if(bytesRead==0 || bytesRead==-1)
							break;
						b=bufferCheck(buffer);
				
					}
				}
				if(bytesRead==0 || bytesRead==-1)
					break;
				while(!strcmp(word, "!")){
					if(i>=10)
						break;
					if(isalnum(buffer[i])){
						i++;
						if(i>=10){
							overflow=1;
							break;
						}
						continue;
					}
				
					if(buffer[i]=='\0' && strlen(buffer)<10){
						bytesRead=11;
						break;
					}
					buffer[i]='\0';
					int notAlphaStart=i;
					while(!isalnum(buffer[i])){
						i++;
						if(i>=10){
							//i--;
							break;
						}
					}
					i--;
					end=notAlphaStart;
					end=i;
					size=end-start;
					if(size==0 || !isalnum(buffer[start])){
						if(st==NULL){
							i++;
							start=i; 
							continue;
						}
					}
					//printf("i== %d, start== %d, end == %d, size == %d\n", start, end, size);
					if(st!=NULL){
						char * temp=(char *) malloc(size*(sizeof(char)));
						memcpy(temp, &buffer[start], end);
						//printf("copied temp == %S\n", temp);
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						nd->next = buff;
						nd=buff;
						char * bigWord = readOverflow();
						word=bigWord;
						st=NULL;
						nd=st;
				
					}else{
						char * temp=(char *) malloc(size*(sizeof(char)));
						memcpy(temp, &buffer[start], end);
						word=temp;
					}
				
					i++;
					start=i;
					foundAlnum=0;
				}
				if(start<10 && overflow==1 && !strcmp(word, "!")){
					size=10-start;
					char * temp=(char *) malloc(size*(sizeof(char)));
					memcpy(temp, &buffer[start], 9);
					if(st==NULL){
						//printf("temp = %s, assigning to st\n", temp);
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						st=buff;
						nd=st;
					}else{
						struct blarp * buff = (struct blarp *) malloc(sizeof(struct blarp));
						buff->toke = temp;
						buff->size = size;
						nd->next = buff;
						nd=buff;
					}
					start=0;
					end=0;
					word="!";
					overflow=0;
					i=0;
					while(i<10){
						buffer[i]='\0';
						i++;
					}
					i=0;
					continue;
				}
				if(!strcmp(word, "!"))
					break;
				word = toLowerCase(word);
				//printf("evaultating %s\n", word);
				if(word==NULL || !isalnum(word[0])){
				
					continue;
				}
				int hash = getHash(word[0]);
				//file=dp->d_name;
				struct node * f1 = createNode(file);
				struct words * head =letterPtrs[hash]->head;//word node
				int h= strcmp(head->word, "!");
				void * ptr;
				if(h==0){//if that pos is null in letterPtrs
					head->word=word;
					head->files=f1;		
				}else{
					struct words * w1 = createWords(word);
					struct words * curr=head;
					struct words * before=curr;
					int result=0;
					while(curr!=NULL){
						result = comparer(curr->word, word);
						if(result==1)
							break;
						before=curr;
						curr=curr->next;
					}
					int r=-2;
					if(before->next!=NULL)
						r = strcmp(before->next->word, word);
					if(result==0){//never found word's place,goes at end
						before->next=w1;
						w1->files=f1;
					}else if(result==1 && before==curr){//word goes first
						r =strcmp(before->word, word);
						if(r!=0){//if word did not match with first in list
							w1->next=head;
							letterPtrs[hash]->head=w1;
							w1->files=f1;
						}else{
							ptr=findFile((void*)(before->files), file);
							if(ptr==NULL){
								f1->next=before->files;
								before->files=f1;
							}else{
								struct node * t3 = (struct node *)(ptr);
								t3->occ=t3->occ+1;
							}
						}
					}else if(r==0){ //found a match
						ptr = findFile((void*)(before->next->files), file);
						if(ptr==NULL){
							before=before->next;
							f1->next=before->files;
							before->files=f1;
						}else{
							//printf("i found the file\n");
							struct node * t3 = (struct node *)(ptr);
							t3->occ=t3->occ+1;
						}
					}else{//found where a new word goes, between before and curr
						w1->next=before->next;
						before->next=w1;
						w1->files=f1;
					}
					
				}
				//printer();
				//printf("---------\n");
				word="!";
				if(i>=10){
					start=0;
					end=0;
					i=0;
					overflow=0;
					while(i<10){
						buffer[i]='\0';
						i++;
					}
					i=0;
				}
			}
		
			close(fd);
		}
	
	//printer();
	int nullity=NULL_check();
	if(nullity==1)
		printToFile(ouFile, 1);
	else{
		printToFile(ouFile, -1);
	}//printer();

      

	return 0;
}


