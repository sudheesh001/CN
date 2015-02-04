#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#define GROUPS 3

int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	int portno=1234;
	int sfd[GROUPS];
	for (int i = 0; i < GROUPS; ++i)
	{
		sfd[i]=socket(AF_INET,SOCK_STREAM,0);
		serv_addr.sin_port=htons(portno);
		bind(sfd[i],(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		listen(sfd[i],5);
		portno++;
	}
	fd_set rfds;
	struct timeval tv;
	tv.tv_usec=0;
	tv.tv_sec=5;
	int flag[GROUPS];
	for (int i = 0; i < GROUPS; ++i)
	{
		flag[i]=0;
	}
	while(1){
		FD_ZERO(&rfds);
		for (int i = 0; i < GROUPS; ++i)
			{
				if(flag[i]==0)
					FD_SET(sfd[i],&rfds);
			}
		int maxfd=0;
		for (int i = 0; i < GROUPS; ++i)
			{
				if(maxfd<sfd[i])maxfd=sfd[i];				
			}	
		if(select(maxfd+1,&rfds,NULL,NULL,&tv)>0){
			for (int i = 0; i < GROUPS; ++i)
			{
				if(FD_ISSET(sfd[i],&rfds)){
					int c=fork();
					if(c>0)flag[i]=1;
					else if(c==0){
						for (int j = 0; j< GROUPS; ++j)
						{
							if(j!=i)close(sfd[j]);
						}
						dup2(sfd[i],0);
						execlp("./g1.out","kk",(char *)0);
					}
				}
			}
		}
	}
	return 0;
}