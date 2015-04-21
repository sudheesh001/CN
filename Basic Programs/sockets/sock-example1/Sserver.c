#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#define derror(msg) {printf("%s\n", msg); exit(0);}

int sfd,cfd;

int main()
{
	struct sockaddr_in ser_addr,cli_addr;
	int portno,cli_len,child_pid;
	char buf[10];
	//pthread_t sender,receiver;
	
	sfd = socket(AF_INET,SOCK_STREAM,0); // socket create... AF_INET for IPv4 domain and SOCK_STREAM for connection oriented systems
	
	if(sfd < 0)
		derror("Socket create");

	memset(&ser_addr,0,sizeof(struct sockaddr_in)); // Initialize to 0
	memset(&cli_addr,0,sizeof(struct sockaddr_in));

	ser_addr.sin_family = AF_INET;
	portno = 28765;
	ser_addr.sin_port = htons(portno); // converts int to 16 bit integer in network byte order
	ser_addr.sin_addr.s_addr = INADDR_ANY; // to get IP address of machine on which server is running

	if(bind(sfd,(struct sockaddr*)&ser_addr,sizeof(struct sockaddr_in))<0)
		derror("Bind error");
	
	if(listen(sfd,3)<0) // No. of clients that server can service
		derror("Listen error");

	cli_len = sizeof(struct sockaddr_in);
	
	par: cfd = accept(sfd,(struct sockaddr*)&cli_addr,&cli_len); // Type cast sockaddr_in to sockaddr and pass by reference and pointer of length to be passed

	if(cfd < 0)
		derror("Accept error");

	// strcpy(buf,"Hi client...Sup?");

	// send(cfd,buf,strlen(buf),0);

	child_pid = fork();

	if(child_pid<0)
	{
		derror("Fork error");
	}
	
	else if(child_pid==0)
	{
		close(cfd);
		goto par;
	}

	else
	{
		close(sfd);
		memset(buf,0,10);
		while(recv(cfd,buf,10,0)<0);
		dup2(cfd,0);
		if(buf[0]=='1')
			execlp("./s1","s1",(char*) 0);
		else
			execlp("./s2","s2",(char*) 0);
		derror("Exec error!")
	}
	return 0;
}
