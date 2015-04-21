#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr,client_addr;
	int portno=atoi(argv[1]);
	int nsfd;
	char buffer[256];
	socklen_t clilen;
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(portno);
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	bind(sfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfd,5);
	clilen=sizeof(client_addr);
	while(1){	
	nsfd=accept(sfd,(struct sockaddr*)&client_addr,&clilen);
	read(nsfd,buffer,256);
	buffer[5]='k';
	write(nsfd,buffer,256);
	close(nsfd);
	}
	close(sfd);
	return 0;
}
