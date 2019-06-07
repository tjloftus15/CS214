#include "libnetfiles.h"
int o_type;
char * host;

//use 'HERE' for socket return check. 
// - if no return, set errno or h_errno


int check_completer(char * buffer){
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

int netserverinit(char * hostname, int filemode){
	//printf("IN NET SERVER INIT MY FRIENDS\n");
	o_type=filemode;
	host=hostname;
	h_errno=0;
	errno=0;
	struct hostent * host_info;
	host_info = gethostbyname(hostname);
	if(host_info==NULL){
		//printf("The h_errno value is: %d\n", h_errno);
		//pthread_mutex_lock(&errnoT);  //locking down the thread 
		switch(h_errno){
			case HOST_NOT_FOUND:
				printf("Cannot locate host\n");
				break;
			case NO_ADDRESS:
				printf("Could not trace hostname to valid IP\n");
				break;
			case NO_RECOVERY:
				printf("Unknown error\n");
				break;
			case TRY_AGAIN:
				printf("Please try again, error occured\n");
				break;
		//pthread_mutex_unlock(&errnoT);
		}
		//errno=h_errno;
		return -1;
	}
	if(filemode>2 || filemode<0){
		errno=INVALID_FILE_MODE;
		return -1;
	}
	errno=0;
	h_errno=0;
	int sockfd = 0;
	int port = 10286;
	int n=0;
	char buffer[1024];
	struct sockaddr_in serverAddrInfo;
	struct hostent * serverIPAddr;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	//char * host;
	//host =argv[1];
	serverIPAddr = gethostbyname(host);
	if(serverIPAddr == NULL){ // didnt find the host
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero((char*) &serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_family = AF_INET;
	serverAddrInfo.sin_port=htons(port);
	bcopy((char *)serverIPAddr->h_addr, (char*)&serverAddrInfo.sin_addr.s_addr, serverIPAddr->h_length);
	if(connect(sockfd, (struct sockaddr*)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero(buffer,1024);
	char ot[2];
	ot[1]='\0';
	sprintf(ot, "%d", filemode);
	char * a= "T";
	strcat(buffer, "3");
	strcat(buffer, ",");
	strcat(buffer, a);
	strcat(buffer, ",");
	//strcat(buffer, host);
	//strcat(buffer, ",");
	strcat(buffer, ot);
//REMOVE
	//printf("sending over buffer = %s\n", buffer);
	n=write(sockfd, buffer, strlen(buffer));
	if(n<=0)
		return -1;
	bzero(buffer, 1024);
	n=read(sockfd, buffer, 1023);
	//HERE
	close(sockfd);
	return 0;
}

	
//use threadtest.c in home directory for example on how to:
//	 	- create: (use make value returnable link)

//	 	- join: https://stackoverflow.com/questions/17548455/in-c-how-to-make-a-thread-wait-for-other-threads-to-finish

//		- make a value returnable: https://stackoverflow.com/questions/2251452/how-to-return-a-value-from-thread-in-c

ssize_t netopen(const char * pathname, int flags){
	int sockfd = 0;
	int port = 10286;
	int n=0;
	char buffer[1024];
	errno=0;
	h_errno=0;
	struct sockaddr_in serverAddrInfo;
	struct hostent * serverIPAddr;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	serverIPAddr = gethostbyname(host);
	if(serverIPAddr == NULL){ // didnt find the host
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero((char*) &serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_family = AF_INET;
	serverAddrInfo.sin_port=htons(port);
	bcopy((char *)serverIPAddr->h_addr, (char*)&serverAddrInfo.sin_addr.s_addr, serverIPAddr->h_length);
	if(connect(sockfd, (struct sockaddr*)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero(buffer,1024);
	//printf("buffer == %s\n", buffer);
	int msg_size=strlen(pathname)+4;
	
	//send encoding
	char bytesnum[msg_size];
	sprintf(bytesnum, "%d", msg_size);
	char op[2];
	op[0]='O';
	op[1]='\0';
	char flag[2];
	char otype[3];
	bzero(otype, 3);
	bzero(flag, 2);
	sprintf(otype, "%d", o_type);
	sprintf(flag, "%d", flags);
	char trim[strlen(pathname)+10];
	bzero(trim, strlen(pathname)+10);
	memcpy(trim, pathname, strlen(pathname));
	strcat(buffer, bytesnum);
	strcat(buffer,",");
	strcat(buffer, op);
	strcat(buffer, ",");
	strcat(buffer, trim);
	strcat(buffer, ",");
	strcat(buffer, flag);
	//printf("about to write to socket: %s\n", buffer);//REMOVE
	n=write(sockfd, buffer, strlen(buffer));
	if(n<=0)
		return -1;

	bzero(buffer, 1024);
	n=read(sockfd, buffer, 1023);
	//printf("read in %s\n", buffer);
	if(n<=0)
		return -1;
	close(sockfd);
	int s=0;
		int t=0;
	while(buffer[s]!=',')
		s++;
	char msg_s [5];
	bzero(msg_s, 5);
	while(t<s){
		msg_s[t]=buffer[t];
		t++;
	}
	s++;
	t++;
	int msg_len=atoi(msg_s);
	int msg_check = check_completer(buffer);
	if(msg_check==-1){
		//printf("msg check failure\n");
		return -1;
	}
	//printf("after msg check\n");
	char sfd[10];
	bzero(sfd, 10);
	//printf("s before == %d\n", s);
	while(buffer[s]!=',')
		s++;
	//printf("t==%d s==%d\n", t, s);
	int jk=0;
	while(t<s){
		sfd[jk]=buffer[t];
		//printf("buffer[%d] == %c\n", t, buffer[t]);
		jk++;
		t++;
	}
	//printf(
	int newfd=atoi(sfd);
	//printf("sfd = %s\n buffer == %s\n", sfd, buffer);
	//printf("newfd = %d\n", newfd);
	//copied and pasted below from netwrite
	int d=strlen(buffer)-1;
	int end=0;
	while(buffer[d]!=',')
		d--;
	char err[strlen(buffer)-d+1];
	bzero(err, strlen(buffer)-d+1);
	end=d;
	d++;
	int e=0;
	while(d<strlen(buffer)){
		err[e]=buffer[d];
		e++;
		d++;
	}
	//sprintf(err, "%d", errno);
	//end of copied code
	
	if(newfd!=-1){
		newfd=(newfd+1)*-1;
		return newfd;
	}else{//error occured. read error num
		errno=atoi(err);
	}
	return newfd;
}

ssize_t netread(int fildes, void* buf, size_t nbyte){
	int sockfd = 0;
	int port = 10286;
	int n=0;
	char buffer[1024];
	errno=0;
	h_errno=0;
	struct sockaddr_in serverAddrInfo;
	struct hostent * serverIPAddr;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	//char * host;
	//host =argv[1];
	serverIPAddr = gethostbyname(host);
	if(serverIPAddr == NULL){ // didnt find the host
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero((char*) &serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_family = AF_INET;
	serverAddrInfo.sin_port=htons(port);
	bcopy((char *)serverIPAddr->h_addr, (char*)&serverAddrInfo.sin_addr.s_addr, serverIPAddr->h_length);
	if(connect(sockfd, (struct sockaddr*)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	errno=0;
	int fd=fildes;
	fd=(fd*-1)-1;
	//char buffer[1024];
	bzero(buffer, 1024);
	//encode
	char * a ="R";
	char cfd[1000];
	sprintf(cfd, "%d", fd);
	char bytes_to_read[1000];
	sprintf(bytes_to_read, "%d", nbyte);
	strcat(buffer, a);
	strcat(buffer, ",");
	strcat(buffer, cfd);
	strcat(buffer, ",");
	strcat(buffer, bytes_to_read);
	int msg_size=strlen(buffer);
	char temp_buff[1024];
	char msg_string[1000];
	sprintf(msg_string, "%d", msg_size);
	bzero(temp_buff, 1024);
	strcat(temp_buff, msg_string);
	strcat(temp_buff, ",");
	strcat(temp_buff, buffer);
	//buffer=temp_buff; 
	
	size_t bytesLeft=0;
	//printf("in netread, writing to server: %s\n", temp_buff);
	n=write(sockfd, temp_buff, strlen(temp_buff)); 
	if(n<=0)
		return -1;
	bzero(buffer, 1024);
	n=read(sockfd, buffer, 1023);
	if(n<=0)
		return -1;
	//HERE
	//printf("buffer returned from server = %s\n", buffer);
	//decode
	int b=0;
	int c=0;
	while(buffer[b]!=',')
		b++;
	char msg_s [5];
	bzero(msg_s, 5);
	while(c<b){
		msg_s[c]=buffer[c];
		c++;
	}
	b++;
	c++;
	int msg_len=atoi(msg_s);
	int msg_check = check_completer(buffer);
	if(msg_check==-1){
		return -1;
	}
	while(buffer[b]!=',')
		b++;;
	char byte_result[b+1];
	bzero(byte_result, b+1);
	int bb=0;
	while(c<b){
		byte_result[bb]=buffer[c];
		c++;
		bb++;
	}
	int bytes_read;
	//sprintf(byte_result, "%d", bytes_read);
	bytes_read=atoi(byte_result);
	//printf("byte_result== %s\n", byte_result);
	char * result= (char *)buf;
	b++;
	int d=strlen(buffer)-1;
	int end=0;
	while(buffer[d]!=',')
		d--;
	char err[strlen(buffer)-d+1];
	bzero(err, strlen(buffer)-d+1);
	end=d;
	d++;
	int e=0;
	while(d<strlen(buffer)){
		err[e]=buffer[d];
		e++;
		d++;
	}
	close(sockfd);
	//printf("bytes_read = %d, nbyte = %zu\n", bytes_read, nbyte);
	char convert[100];
	sprintf(convert, "%zu", nbyte);
	int nbyte2=atoi(convert);
	if(bytes_read<nbyte2){
		errno=atoi(err);
		return bytes_read;
	}
	c=0;
	while(b<end && c<sizeof(result)){
		result[c]=buffer[b];
		c++;
		b++;
	}
	//printf("result == %s\n", result);
	bzero(convert, 100);
	sprintf(convert, "%zu", nbyte);
	ssize_t retbytes;
	sscanf(convert, "%zu", &retbytes);
	printf("nbyten == %zu, retbytes = %zu\n");
	return retbytes;
	//return nbyte;
}

ssize_t netwrite(int fildes, void* buf, size_t nbyte){
	int sockfd = 0;
	int port = 10286;
	int n=0;
	char buffer[1024];
	errno=0;
	h_errno=0;
	struct sockaddr_in serverAddrInfo;
	struct hostent * serverIPAddr;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	serverIPAddr = gethostbyname(host);
	if(serverIPAddr == NULL){ // didnt find the host
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero((char*) &serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_family = AF_INET;
	serverAddrInfo.sin_port=htons(port);
	bcopy((char *)serverIPAddr->h_addr, (char*)&serverAddrInfo.sin_addr.s_addr, serverIPAddr->h_length);
	if(connect(sockfd, (struct sockaddr*)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
//printf("-----------------------------------------------------\n");
	int fd=fildes;
	fd=fd*-1-1;
	char * a ="W";
	char cfd[1000];
	sprintf(cfd, "%d", fd);
	char * towrite=(void*)buf;
	bzero(buffer, 1024);
	strcat(buffer, a);
	strcat(buffer, ",");
	strcat(buffer, cfd);
	strcat(buffer, ",");
	strcat(buffer, towrite);
	int msg_size=strlen(buffer);
	char temp_buff[1024];
	char msg_string[1000];
	sprintf(msg_string, "%d", msg_size);
	bzero(temp_buff, 1024);
	strcat(temp_buff, msg_string);
	strcat(temp_buff, ",");
	strcat(temp_buff, buffer);
	//buffer=temp_buff; 
	
	//REMOVE
	size_t bytesread=0;
	//printf("about to write to server = %s\n", temp_buff);
	n=write(sockfd, temp_buff, strlen(temp_buff)); 
	if(n<=0)
		return -1;
	//printf("i wrote %d bytes\n", n);
	bzero(buffer, 256);
	n=read(sockfd, buffer, 1023);
	//printf("buffer == %s\n", buffer);
	if(n<=0)
		return -1;
	//HERE
	//printf("buffer returned from server = %s\n", buffer);
	//decode
	int b=0;
	int c=0;
	while(buffer[b]!=',')
		b++;
	char msg_len[5];
	bzero(msg_len, 5);
	while(c<b){
		msg_len[c]=buffer[c];
		
		
		c++;
	}
	int msg_check = check_completer(buffer);
	if(msg_check==-1){
		return -1;
	}
	c++;
	b++;
	while(buffer[b]!=',')
		b++;;
	char byte_result[b+1];
	bzero(byte_result, b+1);
	int cc=0;
	while(c<b){
		byte_result[cc]=buffer[c];
		c++;
		cc++;
	}
	
	int bytes_read;
	bytes_read=atoi(byte_result);

	int d=strlen(buffer)-1;
	int end=0;
	while(buffer[d]!=',')
		d--;
	char err[strlen(buffer)-d+1];
	bzero(err, strlen(buffer)-d+1);
	end=d;
	d++;
	int e=0;
	while(d<strlen(buffer)){
		err[e]=buffer[d];
		e++;
		d++;
	}
	close(sockfd);
	//printf("bytes read == %d\n", bytes_read);
	//printf("nbyte == %zu\n", nbyte);
	
	char convert[100];
	sprintf(convert, "%zu", nbyte);
	int nbyte2=atoi(convert);
	if(bytes_read<nbyte2){
		errno=atoi(err);
		h_errno=errno;
		//printf("errno here == %d\n", errno);
		if(bytes_read==-1)
			return -1;
		bzero(convert, 100);
		sprintf(convert, "%d", bytes_read);
		ssize_t retbytes;
		sscanf(convert, "%zu", &retbytes);
		return retbytes;
	}
	bzero(convert, 100);
	sprintf(convert, "%zu", nbyte);
	ssize_t retbytes;
	//printf("before sscanf\n");
	sscanf(convert, "%zu", &retbytes);
	//printf("nbyten == %zu, retbytes = %zu\n", nbyte, retbytes);
	return retbytes;
	//return bytes_read;
}

int netclose(int fd){
	int sockfd = 0;
	int port = 10286;
	int n=0;
	char buffer[1024];
	errno=0;
	h_errno=0;
	struct sockaddr_in serverAddrInfo;
	struct hostent * serverIPAddr;
	sockfd=socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	//char * host;
	//host =argv[1];
	serverIPAddr = gethostbyname(host);
	if(serverIPAddr == NULL){ // didnt find the host
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero((char*) &serverAddrInfo, sizeof(serverAddrInfo));
	serverAddrInfo.sin_family = AF_INET;
	serverAddrInfo.sin_port=htons(port);
	bcopy((char *)serverIPAddr->h_addr, (char*)&serverAddrInfo.sin_addr.s_addr, serverIPAddr->h_length);
	if(connect(sockfd, (struct sockaddr*)&serverAddrInfo, sizeof(serverAddrInfo))<0){
		if(errno==0)
			errno=h_errno;
		return -1;
	}
	bzero(buffer, 1023);
	errno=0;
	fd=fd*-1-1;
	char cfd[1000];
	sprintf(cfd, "%d", fd);
	strcat(buffer, "C,");
	strcat(buffer, cfd);
	int msg_l=strlen(buffer);
	char temp[1000];
	char msg_len[1000];
	bzero(temp, 1000);
	sprintf(msg_len, "%d", msg_l);
	strcat(temp, msg_len);
	strcat(temp, ",");
	strcat(temp, buffer);
	//printf("about to write: %s\n");
	n=write(sockfd, temp, strlen(temp));
	if(n<=0)
		return -1;
	bzero(buffer, 256);
	n=read(sockfd, buffer, 1023);
	//HERE
	//printf("recieved %s\n", buffer);
	if(n<=0)
		return -1;
	bzero(msg_len, 1000);
	int x=0;
	int y=0;
	while(buffer[y]!=',')
		y++;
	while(x<y){
		msg_len[x]=buffer[x];
		x++;
	}
	int msg_size=atoi(msg_len);
	//msg check here
	int msg_check = check_completer(buffer);
	if(msg_check==-1){
		return -1;
	}
	x++;
	y++;
	//printf("buffer[%d] == %c\n", x, buffer[x]);
	if(buffer[x]=='0')
		return 0;
	//if could not close file, buffer[0]=='N' instead of 'O'
	//must report errno, which is read from message below
	int d=strlen(buffer)-1;
	int end=0;
	while(buffer[d]!=',')
		d--;
	char err[strlen(buffer)-d+1];
	bzero(err, strlen(buffer)-d+1);
	end=d;
	d++;
	int e=0;
	while(d<strlen(buffer)){
		err[e]=buffer[d];
		e++;
		d++;
	}
	sprintf(err, "%d", errno);
	//h_errno=errno;
	return -1;
}



