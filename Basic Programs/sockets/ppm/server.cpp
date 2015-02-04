#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#define MAXFORKS 2
#define MAXTHREADS 10
#define MAXSOCKS 50

pthread_t thread[MAXTHREADS];
int* sfd;
char buff[256];
struct sockaddr_in cli_addr;
int max(int* a){
	int mx=0;
	for (int i = 0; i < MAXSOCKS; ++i)
	{
		if(a[i]>mx)mx=a[i];
	}
	return mx;
}
socklen_t clilen;
void* myfunction(void* arg){
	for (int i = 0; i < MAXSOCKS; ++i)
	{
		clilen=sizeof(cli_addr);
		fd_set rfds;
		struct timeval tv;tv.tv_sec=5;tv.tv_usec=0;
		while(1){
		FD_ZERO(&rfds);
		for (int i = 0; i < MAXSOCKS; ++i)
			FD_SET(sfd[i],&rfds);
		
		int maxfd=max(sfd);
		int retval=select(maxfd+1,&rfds,NULL,NULL,&tv);
		if(retval>0){
			for (int i = 0; i < MAXSOCKS; ++i)
			{
				if(FD_ISSET(sfd[i],&rfds)){
					int nsfd=accept(sfd[i],(struct sockaddr*)&cli_addr,&clilen);
					close(sfd[i]);
					bzero(buff,256);
					read(nsfd,buff,255);
					for(int i=0;i<strlen(buff);i++){
					buff[i]=toupper(buff[i]);
					}
					write(nsfd,buff,255);
					close(nsfd);
				}
			}
			}
		}
	}
}
int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	int portno=1234;
	sfd=new int[MAXSOCKS];
	socklen_t clilen;
	for (int i = 0; i < MAXSOCKS; ++i)
		sfd[i]=socket(AF_INET,SOCK_STREAM,0);
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	for(int i=0;i<MAXSOCKS;i++){
		serv_addr.sin_port=htons(portno++);
		bind(sfd[i],(struct sockaddr*)&serv_addr,sizeof(serv_addr));
		listen(sfd[i],5);
	}
	int pid,j;
	for (int i = 0; i < MAXFORKS; ++i)
	{
		if((pid=fork())==0){
			for(j=0;j<MAXTHREADS;j++){
				pthread_create(&thread[j],NULL,myfunction,NULL);
			}
			pthread_join(thread[0],NULL);
		}
	}
	return 0;
}