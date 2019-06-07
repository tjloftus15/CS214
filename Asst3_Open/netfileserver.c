//file server

//how to handle write fails between sockets
//figure out makefile correct way
//MUSIC


//USE 'SOCK HERE' for socket errors
//	- just try to write again. If error in read, try read again. if still no, write error back. if not, pthread_exit


#include "libnetfiles.h"
char * host;
struct hash_table table[100];
struct open_mode * head;
pthread_mutex_t errnoLock;

int hash(char * path){
	int i=0;
	unsigned long long sum=0;
	while(i<strlen(path)){
		sum=path[i]*7+sum;
		i++;
	}
	sum=sum/strlen(path);
	sum=sum%100;
	return sum;
}
void insert_open_type(uint32_t clientID, int o_type){
	struct open_mode * new = (struct open_mode *)malloc(sizeof(struct open_mode));
	new->clientID=clientID;
	//printf("!\n!\n");
	//printf("inserting otype of: %d\n", o_type);
	new->o_type=o_type;
	if(head==NULL){
		head=new;
		return;
	}
	struct open_mode * curr=head;
	while(curr->next!=NULL){
		curr=curr->next;
	}
	curr->next=new;
	return;
	
}

int get_open_type(uint32_t clientID){
	//printf("in open type\n");
	struct open_mode * curr = head;
	while(curr!=NULL){
		if(curr->clientID==clientID){
			//printf("!\n!\n");
			//printf("returning otype of: %d\n", curr->o_type);
			return curr->o_type;
		}
		curr=curr->next;
	}
	return 5;
}


int delete_user(int h, void * args){
	struct deleter * stuff=(struct deleter *)args;
	struct node * path_curr=stuff->path_curr;
	struct node * path_before=stuff->path_before;
	struct users * user_curr=stuff->user_curr;
	struct users * user_before=stuff->user_before;
	
	if(path_curr==table[h].first && user_curr==user_before && user_curr->next==NULL && path_curr->next==NULL){
		//the first path, the first user, the only user, the only path
		//delete M
		free(user_curr);
		free(path_curr);
		table[h].first=NULL;
		return 0;
	}
	if(path_curr==table[h].first && user_curr==user_before && user_curr->next==NULL && path_curr->next!=NULL){
		//first path, first user, the only user, there are multiple paths
		//delete J
		free(user_curr);
		table[h].first=path_curr->next;
		free(path_curr);
		return 0;
	}
	if(path_curr!=table[h].first && user_curr==user_before && user_curr->next==NULL && path_curr->next==NULL){
		//the last path, first user, the only user
		//delete T
		free(user_curr);
		free(path_curr);
		path_before->next=NULL;
		return 0;
	}
	if(path_curr!=table[h].first && user_curr==user_before && user_curr->next==NULL && path_curr->next!=NULL){
		//not the first path, not the last path, the first user, the only user
		//delete F 1
		path_before=path_curr->next;
		free(path_curr);
		free(user_curr);
		return 0;
	}
	if(user_curr==user_before && user_curr->next!=NULL){
		//doesnt matter what path, the first user, the user is not the only user, path list length does not matter
		//delete C
		path_curr->first=user_curr->next;
		free(user_curr);
		return 0;
	}
	if(user_curr!=user_before && user_curr->next!=NULL){
		//doesnt matter what path, not the first user, not the only user, not the last user, path list doesnt matter
		//delete Q delete D
		user_before=user_curr->next;
		free(user_curr);
		return 0;
	}
	return -1;

}
//checks if given user has requsted permission on file specified by fdpath
int search_directory(char * fdpath, int userID, int a){
	int h=hash(fdpath);
	//printf("original hash == %d\n", h);
	struct node * t = table[h].first;
	struct node * t2=t;
	while(t!=NULL){
		if(!strcmp(fdpath, t->path)){
			struct users * curr=t->first;
			struct users * before=curr;
			while(curr!=NULL){
				//printf("found a user\n");
				if(curr->user==userID){//found the user
					//printf("found a user\n");
					if(a==3){
						int result=0;
						struct deleter * pass=(struct deleter *) malloc(sizeof(struct deleter));
						pass->path_curr=t;
						pass->path_before=t2;
						pass->user_curr=curr;
						pass->user_before=before;
						result=delete_user(h, (void*)pass);
						if(result==-1){
							//printf("ERROR: nothing matched to delete_user()\n");
							return -1;
						}
						return 0;
					}
					if(curr->mode==a)
						return 0;
					if(curr->mode==2)
						return 0;
					else
						return -1;
				}
				before=curr;
				curr=curr->next;
			}
			return -1;
		}
		t2=t;
		t=t->next;
	}
	return -1;
}
//this makes sure there is no extension A problems
int check_open_type(char * fdpath, int a){
	int h=hash(fdpath);
	//printf("inside check open\n");
	struct node * t = table[h].first;
	while(t!=NULL){
		if(!strcmp(fdpath, t->path)){//file already open
			struct users * u=t->first;
			while(u!=NULL){
				if(u->open_type==2)
					return -1;
				if(u->open_type==1 && (u->mode==1 || u->mode==2) && (a==1 || a==2))
					return -1;
				u=u->next;
			}
			return 0;
		}
		t=t->next;
	}
	return 0;
}


struct director * input_decoder(char * buffer){
	struct director * input=(struct director*)malloc(sizeof(struct director));
	
	//get msg_len
	int i=0;
	while(buffer[i]!=',')
		i++;
	char * msg_string=(char *)malloc(i+1*sizeof(char));
	bzero(msg_string, i+1);
	int j=0;
	while(j<i){
		msg_string[j]=buffer[j];
		j++;
	}
	input->msg_len=atoi(msg_string);
	int size_count_len=i;
	/*if(msg_len!=strlen(buffer))
		help
	*/
	//printf("got msg_len of %d\n", input->msg_len);
	
	//get op/sockfd (arg1)
	i++;
	j=i;
	while(buffer[i]!=',')
		i++;
		//printf("j==%d, i==%d\n", j, i);
	char * arg1 = (char*)malloc((10)*sizeof(char));
	bzero(arg1, 10);
	int tj=0;
	while(j<i){
		arg1[tj]=buffer[j];
		j++;
		tj++;
	}

	input->arg1=arg1;
	
	//printf("got arg1 of %s\n", input->arg1);
	//get arg2 via arg3
	int k=input->msg_len+size_count_len;
	int t=0;
	while(buffer[k]!=','){
		//printf("buffer[%d] == %c\n", k, buffer[k]);
		k--;
	}k++;
	char * arg3=(char *)malloc((1000)*sizeof(char));
	bzero(arg3, 1000);
	t=k;
	i=0;
	while(t<strlen(buffer)){
		arg3[i]=buffer[t];
		i++;
		t++;
	}
	input->arg3=arg3;
	//get last arg in message
	
	//see if only 3 args: length of msg, close, fd
	if(k==j)
		return input;
		
	//get path for open
	j++;
	k--;
	char * arg2=(char *)malloc((k-j)*sizeof(char));
	i=0;
	while(j<k){
		arg2[i]=buffer[j];
		j++;
		i++;
	}
	input->arg2=arg2;
	return input;
}
//checks to make sure the full message made it
int check_complete(char * buffer){
	int i=0;
	int j=0;
	while(buffer[i]!=',')
		i++;
	char * msg_string=(char *)malloc(i+1*sizeof(char));
	bzero(msg_string, i+1);
	while(j<i){
		msg_string[j]=buffer[j];
		j++;
	}
	int msg_len=atoi(msg_string);
	//printf("msg_len == %d\n", msg_len);
	i++;
	j=i;
	while(i<strlen(buffer))
		i++;
	if(i-j<msg_len)
		return -1;
	return 0;
}

//create linked list system to keep track of who has what open
//master list is by file name
//second level is the user that has it open, and their permission
//list created upon open request

void * thread_director(void * args){
	//printf("-----------------------------------------\n");
	char buffer[1024];
	struct passer * p=(void*)args;
	int newsockfd=p->newsockfd;
	//printf("newsockfd == %d\n", newsockfd);
	int n = -2;
	uint32_t clientID=p->clientID;
	//printf("clientID == %zu\n", clientID);
	pthread_mutex_lock(&errnoLock);
	//printf("after lock\n");
	n=read(newsockfd, buffer, 1023);
	if(n<=0){
		bzero(buffer, 1024);
		char err[100];
		bzero(err, 100);
		sprintf(err, "%d", errno);
		pthread_mutex_unlock(&errnoLock);
		strcat(buffer, "-1");
		strcat(buffer, ",");
		strcat(buffer, err);
		n=write(newsockfd, buffer, strlen(buffer));
		if(n<=0)
			n=write(newsockfd, buffer, strlen(buffer));
		pthread_exit(0);
	}	
	pthread_mutex_unlock(&errnoLock);
	//printf("buffer== %s\n", buffer);
	int msg_check = check_complete(buffer);
	//printf("after msg check\n");
	if(msg_check==-1){
		bzero(buffer, 1024);
		char err[100];
		bzero(err, 100);
		//printf("ERROR: full message not set\n");
		sprintf(err, "%d", 22);
		strcat(buffer, "-1");
		strcat(buffer, ",");
		strcat(buffer, err);
		n=write(newsockfd, buffer, strlen(buffer));
		//SOCK HERE
		if(n<=0)
			n=write(newsockfd, buffer, strlen(buffer));
		pthread_exit(0);
		
	}
	struct director * input;
	input=input_decoder(buffer);
	int msg_len=input->msg_len;
	char op=input->arg1[0];
	//printf("----\n");
	//printf("full message = %s\n", buffer);
	//printf("the following is the decoded input:\n\tinput->msg_len=%d\n\tinput->arg1=%s\n\tinput->arg2=%s\n\tinput->arg3=%s\n", input->msg_len, input->arg1, input->arg2, input->arg3);
	if(op=='T'){
		errno=0;
		int o_type=atoi(input->arg3);
		host=input->arg2;
		insert_open_type(clientID, o_type);
		bzero(buffer, 1024);
		
		n=write(newsockfd, "0", 1);
		//SOCK HERE
		if(n<=0)
			n=write(newsockfd, buffer, strlen(buffer));
		pthread_exit(0);
	}
	if(op=='O'){//return: "new_fd, errno"
		//printf("in open\n");
		char * path=input->arg2;
		bzero(buffer,1024);
		int o_type=5;
		o_type=get_open_type(clientID);
		//printf("after otype, otype == %d\n", o_type);
		char err[100];
		int mode=atoi(input->arg3);
		pthread_mutex_lock(&errnoLock);
		int tempfd = open(path, mode);
		if(tempfd==-1){
			//encode errno. write back status
			bzero(buffer, 1024);
			bzero(err, 100);
			sprintf(err, "%d", 22);
			pthread_mutex_unlock(&errnoLock);
			strcat(buffer, "-1");
			strcat(buffer, ",");
			strcat(buffer, err);
			char temp[1000];
			bzero(temp, 1000);
			int size=strlen(buffer);
			sprintf(temp, "%d", size);
			strcat(temp, ",");
			strcat(temp, buffer);
			n=write(newsockfd, temp, strlen(temp));
			//SOCK HERE
			if(n<=0)
				n=write(newsockfd, buffer, strlen(buffer));
			pthread_exit(0);
		}
		pthread_mutex_unlock(&errnoLock);
		close(tempfd);
		int h=hash(path);
		if(table[h].first==NULL){//the path hasnt been opened yet
			struct node * new=(struct node *)malloc(sizeof(struct node));
			new->path=path;
			struct users * temp=(struct users *)malloc(sizeof(struct users));
			new->first=temp;
			temp->mode=mode;
			temp->user=clientID;
			temp->open_type=o_type;
			table[h].first=new;
		}
		else{//the path has been opened by someone, or there are path colisions.
			int result=0;
			result=check_open_type(path, mode);
			if(result==-1){
				bzero(buffer, 1024);
				//char err[100];
				bzero(err, 100);
				sprintf(err, "%d", 13);
				strcat(buffer, "-1");
				strcat(buffer, ",");
				strcat(buffer, err);
				char temp[1000];
				bzero(temp, 1000);
				int size=strlen(buffer);
				sprintf(temp, "%d", size);
				strcat(temp, ",");
				strcat(temp, buffer);
				n=write(newsockfd, temp, strlen(temp));
				if(n<=0)
					n=write(newsockfd, buffer, strlen(buffer));
				pthread_exit(0);
			}
			
			struct node * curr=table[h].first;
			struct node * before=curr;
			struct users * t1;
			struct users * t2;
			//printf("!!!! searching for path\n");
			while(curr!=NULL){
				//printf("inside curr\n");
				if(!strcmp(path, curr->path)){//you found the path, now go to the end of user list
					t1=curr->first;
					t2=t1;
					while(t1!=NULL){
						t2=t1;
						//printf("t2->user = %zu\n", clientID);
						t1=t1->next;
						
					}
					
					break;
				}
				before=curr;
				curr=curr->next;
			}
			if(curr==NULL){//went to the end, did not find path
				//printf("------->path not open yet\n");
				struct node * new=(struct node *)malloc(sizeof(struct node));
				new->path=path;
				struct users * temp=(struct users *)malloc(sizeof(struct users));
				new->first=temp;
				temp->mode=mode;
				temp->user=clientID;
				temp->open_type=o_type;
				before->next=new;
			}else if(t1==NULL){//no file has write access
				//printf("assigning at end\n");
				struct users * new=(struct users *)malloc(sizeof(struct users));
				t2->next=new;
				new->user=clientID;
				new->mode=mode;
				new->open_type=o_type;
				t2->next=new;
			}
		}	
		errno=0;
		
		//lock errno
		pthread_mutex_lock(&errnoLock);
		int newfd=open(path,mode);
		if(newfd==-1){//didnt write all the bytes
			sprintf(err, "%d", errno);
			pthread_mutex_unlock(&errnoLock);
		}else{
			pthread_mutex_unlock(&errnoLock);
			sprintf(err, "%d", errno);
		}
		//return values: encode newsockfd, errno/status
		bzero(buffer, 1024);		
		char nfd[1000];
		sprintf(nfd, "%d",newfd);
		
		strcat(buffer,nfd);
		strcat(buffer,",");
		strcat(buffer,err);
		n=-2;
		char temp[strlen(buffer)+10];
		bzero(temp, strlen(buffer)+10);
		int msg_len=strlen(buffer);
		sprintf(temp, "%d", msg_len);
		strcat(temp, ",");
		strcat(temp, buffer);
		n=write(newsockfd, temp, strlen(temp));
		//printf("buffer i am returning: %s\n", temp);
		//n=write(newsockfd, buffer, strlen(buffer));
		//SOCK HERE
		if(n<=0)
			n=write(newsockfd, buffer, strlen(buffer));
	}
	else if(op=='R' || op=='W'){ //return= "bytes_not_written,buff"
		errno=0;
		//printf("i have made it into 'R' || 'W'\n");
		int clientfd=atoi(input->arg2);
		int bytes;
		//clientfd=(clientfd*-1)-1;
		int result=0;
		int operation;
		if(op=='R')
			operation=0;
		else
			operation=1;
			
		//printf("after clientfd assignment, where == %d\n", clientfd);
		char * fdpath=(char*)malloc(1000*sizeof(char));
		bzero(fdpath, 1000);
		strcat(fdpath, "/proc/self/fd/");
		char cfd[1000];
		sprintf(cfd, "%d", clientfd);
		strcat(fdpath, cfd);
		char orig_path[1024];
		ssize_t R;
		R=readlink(fdpath, orig_path, 1023);
		char trimpath[R+10];
		bzero(trimpath, R+10);
		strncpy(trimpath, orig_path, R);
		result=search_directory(trimpath, clientID, operation);
		
		
		if(result==-1){//this user cannot read from the given file
			bzero(buffer, 1024);
			char err[100];
			bzero(err, 100);
			sprintf(err, "%d", 13);
			strcat(buffer, "-1");
			strcat(buffer, ",");
			strcat(buffer, err);
			char temp[1000];
			bzero(temp, 1000);
			int size=strlen(buffer);
			sprintf(temp, "%d", size);
			strcat(temp, ",");
			strcat(temp, buffer);
			n=write(newsockfd, temp, strlen(temp));
			if(n<=0)
				n=write(newsockfd, buffer, strlen(buffer));
			pthread_exit(0);
		}
		if(op=='R'){
			errno=0;
			//printf("inside R if\n");
			bytes=atoi(input->arg3);
			char Read[bytes+2];//the bytes you read from clientfd are stored in Read
			bzero(Read, bytes+2);
			char err[100];
			
			pthread_mutex_lock(&errnoLock);
			n=read(clientfd, Read, bytes);
			if(n<bytes){//didnt write all the bytes
				sprintf(err, "%d", errno);
				pthread_mutex_unlock(&errnoLock);
			}else{
				pthread_mutex_unlock(&errnoLock);
				sprintf(err, "%d", errno);
			}
			
			bzero(buffer, 1024);
			char bytesRead[bytes];
			sprintf(bytesRead, "%d", n);
			strcat(buffer,bytesRead);
			strcat(buffer,",");
			strcat(buffer,Read);
			strcat(buffer,",");
			strcat(buffer,err);
			//printf("buffer to write to client = %s\n", buffer);
			//n=write(newsockfd, buffer, strlen(buffer));
			char temp[strlen(buffer)+10];
			bzero(temp, strlen(buffer)+10);
			int msg_len=strlen(buffer);
			sprintf(temp, "%d", msg_len);
			strcat(temp, ",");
			strcat(temp, buffer);
			n=write(newsockfd, temp, strlen(temp));
			//SOCK HERE
			if(n<=0)
				n=write(newsockfd, buffer, strlen(buffer));
		}
		else{//'W'
			errno=0;
			//printf("inside W if\n");
			char * buff=input->arg3;
			bytes=strlen(buff);
			
			//printf("attempted to write/actually wrote = %d/%d\n", bytes, n);		
			char bytesLeft[bytes];
			char err[100];
			
			pthread_mutex_lock(&errnoLock);
			n=write(clientfd, buff, bytes);
			if(n<bytes){//didnt write all the bytes
				sprintf(err, "%d", errno);
				pthread_mutex_unlock(&errnoLock);
			}
			else{
				pthread_mutex_unlock(&errnoLock);
				sprintf(err, "%d", errno);
			}
			bzero(buffer, 1024);
			sprintf(bytesLeft, "%d", n);
			strcat(buffer,bytesLeft);
			strcat(buffer,",");
			strcat(buffer,err);
			//printf("writing %s back to client\n", buffer);
			//n=write(newsockfd, buffer, strlen(buffer));
			char temp[strlen(buffer)+10];
			bzero(temp, strlen(buffer)+10);
			int msg_len=strlen(buffer);
			sprintf(temp, "%d", msg_len);
			strcat(temp, ",");
			strcat(temp, buffer);
			n=write(newsockfd, temp, strlen(temp));
			//SOCK HERE
			if(n<=0)
				n=write(newsockfd, buffer, strlen(buffer));
		}
	}else if(op=='C'){
		errno=0;
		int clientfd=atoi(input->arg3);
		//printf("client fd == %d\n", clientID);
		//printf("inside 'C' if\n");
		int result=0;
		char * fdpath=(char*)malloc(1000*sizeof(char));
		bzero(fdpath, 1000);
		strcat(fdpath, "/proc/self/fd/");
		char cfd[1000];
		sprintf(cfd, "%d", clientfd);
		strcat(fdpath, cfd);
		char orig_path[1024];
		ssize_t R;
		R=readlink(fdpath, orig_path, 1023);
		char trimpath[R+10];
		bzero(trimpath, R+10);
		strncpy(trimpath, orig_path, R);
		char err[100];
		
		pthread_mutex_lock(&errnoLock);
		result=close(clientfd);
		if(result==-1){//didnt write all the bytes
				sprintf(err, "%d", errno);
				pthread_mutex_unlock(&errnoLock);
		}else{
			pthread_mutex_unlock(&errnoLock);
			sprintf(err, "%d", errno);
			
		}int res=0;
		res=search_directory(trimpath, clientID, 3);
		
		//above: we need the client who is attempting to access this file
		/*
		if(res==-1){
			printf("ERROR: could not eliminate file from data structure __\n");
			pthread_exit(0);
		}*/
		char * a= "0";
		if(res==-1)
			a="N";
		bzero(buffer, 1024);
		strcat(buffer,a);
		strcat(buffer,",");
		strcat(buffer,err);
		char temp[strlen(buffer)+10];
		bzero(temp, strlen(buffer)+10);
		int msg_len=strlen(buffer);
		sprintf(temp, "%d", msg_len);
		strcat(temp, ",");
		strcat(temp, buffer);
		n=write(newsockfd, temp, strlen(temp));
		//SOCK HERE
		if(n<=0)
			n=write(newsockfd, buffer, strlen(buffer));
	}
	pthread_exit(0);
	
}


int main(int argc, char ** argv){
	int sockfd=-1;
	int newsockfd=-1;
	int port = 10286;
	int clientAddrLen=-1;
	sockfd=-1;
	struct sockaddr_in serverAddrInfo;
	struct sockaddr_in clientAddrInfo;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		printf("ERROR: could not build socket --> %s\n", strerror(errno));
		return -1;
	}
	
	bzero((char *)&serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_port=htons(port);//set remote port
	serverAddrInfo.sin_family=AF_INET;//set universal int
	serverAddrInfo.sin_addr.s_addr= INADDR_ANY;//flag for type of network address we want to connect to

	//everything is set up. we must now build the server
	
	//this binds to requested port
	if(bind(sockfd, (struct sockaddr *)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		printf("ERROR: could not use specified port --> %s\n", strerror(errno));
		return -1;
	}
	clientAddrLen=sizeof(clientAddrInfo);
	listen(sockfd, 5);
	errno=0;
	while(1){
		newsockfd=accept(sockfd, (struct sockaddr *)&clientAddrInfo, &clientAddrLen);
		if(newsockfd==-1){
			printf("ERROR: could not accept new connection --> %s\n", strerror(errno));
			errno=0;
			continue;
		}
		struct passer * params=(struct passer *)malloc(sizeof(struct passer));
		params->newsockfd=newsockfd;
		//printf("initial newsockfd = %d, params->newfd = %d\n", newsockfd, params->newsockfd);
		params->clientID=clientAddrInfo.sin_addr.s_addr;
		pthread_t serv;
		pthread_create(&serv, NULL, thread_director, (void*)params);
	}
	return 0;
	
}
