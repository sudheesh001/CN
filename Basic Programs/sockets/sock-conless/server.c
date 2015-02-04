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
	int sfd=socket(AF_INET,SOCK_DGRAM,0);
	bind(sfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	clilen=sizeof(client_addr);
	recvfrom(sfd,buffer,255,0,&client_addr,&clilen);
	buffer[3]='k';
	sendto(sfd,buffer,255,0,&client_addr,sizeof(client_addr));
	return 0;
}
