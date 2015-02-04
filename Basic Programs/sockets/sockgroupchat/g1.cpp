#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#define CLIENTS 5

int csfd[CLIENTS],noc;
int sfd;
char buff[100]={0};
int main(int argc, char const *argv[])
{
	printf("Child server started\n");
	struct sockaddr_in client_addr;
	fd_set rfds;
	dup2(0,sfd);
	noc=0;
	socklen_t clilen;
	clilen=sizeof(client_addr);
	csfd[noc++]=accept(sfd,(struct sockaddr *)&client_addr,&clilen);
	struct timeval tv;
	tv.tv_usec=0;
	tv.tv_sec=5;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(sfd,&rfds);
		for (int i = 0; i < noc; ++i)
		{
			FD_SET(csfd[i],&rfds);
		}
		int max=0;
		if(sfd>max)max=sfd;
		for (int i = 0; i < noc; ++i)
		{
			if(csfd[i]>max)max=csfd[i];
		}
		if(select(max+1,&rfds,NULL,NULL,&tv)>0){
			if(FD_ISSET(sfd,&rfds)){
				csfd[noc++]=accept(sfd,(struct sockaddr *)&client_addr,&clilen);
			}
			for (int i = 0; i < noc; ++i)
			{
				if(FD_ISSET(csfd[i],&rfds)){
					bzero(buff,256);
					read(csfd[i],buff,100);
					for (int j = 0; j < noc; ++j)
						if(j!=i)
							write(csfd[j],buff,strlen(buff));
				}
			}
		}
	}
	return 0;
}