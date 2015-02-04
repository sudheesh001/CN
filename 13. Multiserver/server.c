#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr,client_addr;
	int nsfd;
	char buffer[256];
	socklen_t clilen;
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(1234);
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	int sfd1=socket(AF_INET,SOCK_STREAM,0);
	bind(sfd1,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfd1,5);

	int sfd2=socket(AF_INET,SOCK_STREAM,0);
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(1235);
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	bind(sfd2,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfd2,5);

	int sfd3=socket(AF_INET,SOCK_STREAM,0);
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(1236);
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	bind(sfd3,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfd3,5);

	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sfd1,&rfds);
	FD_SET(sfd2,&rfds);
	FD_SET(sfd3,&rfds);
	struct timeval tv;
	tv.tv_usec=0;
	tv.tv_sec=5;
	int maxfd;
	maxfd=(sfd1>sfd2)?sfd1:sfd2;
	maxfd=(maxfd>sfd3)?maxfd:sfd3;
	while(1){	
	int retval=select(maxfd+1,&rfds,'\0','\0',&tv);
	if(retval>0){
		if(FD_ISSET(sfd1,&rfds)){
			clilen=sizeof(client_addr);
			nsfd=accept(sfd1,(struct sockaddr*)&client_addr,&clilen);
			dup2(nsfd,0);
			int pid=fork();
			if(pid>0){
				close(nsfd);			
			}
			else{
			close(sfd1);

			char* arr[]={"p1.out"};
			execve("p1.out",NULL,NULL);
			}
		}
		if(FD_ISSET(sfd2,&rfds)){
	
			clilen=sizeof(client_addr);
			nsfd=accept(sfd2,(struct sockaddr*)&client_addr,&clilen);
			dup2(nsfd,0);
			int pid=fork();
			if(pid>0){
				close(nsfd);			
			}
			else{
			close(sfd2);
			printf("hello\n");
			char* arr[]={"p2.out"};
			execve("p2.out",NULL,NULL);
			}		
		}
		if(FD_ISSET(sfd3,&rfds)){
			
			clilen=sizeof(client_addr);
			nsfd=accept(sfd3,(struct sockaddr*)&client_addr,&clilen);
			dup2(nsfd,0);
			int pid=fork();
			if(pid>0){
				close(nsfd);			
			}
			else{
			close(sfd3);

			char* arr[]={"p3.out"};
			execve("p3.out",NULL,NULL);
			}
		}
	}
	}	
	
	return 0;
}
