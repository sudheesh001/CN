#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
int main(int argc, char const *argv[])
{
	struct hostent *server;
	struct sockaddr_in serv_addr;
	int portno=atoi(argv[2]);
	int nsfd;
	char buffer[256];
	server=gethostbyname(argv[1]);
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(atoi(argv[2]));
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	int sfd=socket(AF_INET,SOCK_DGRAM,0);
	strcpy(buffer,"kranthi");
	sendto(sfd,buffer,strlen(buffer),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	recvfrom(sfd,buffer,255,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	return 0;
}
