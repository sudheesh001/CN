#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	int n;
	server=gethostbyname(argv[1]);
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(atoi(argv[2]));
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	connect(sfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	printf("Please enter the message: ");
	
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n=write(sfd,buffer,strlen(buffer));
	if (n < 0)
   {
      perror("ERROR writing to socket");
      return 0;
   }
	bzero(buffer,256);
	read(sfd,buffer,255);
	printf("%s\n",buffer);
	return 0;
}
