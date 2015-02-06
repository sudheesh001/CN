#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int sock1,sock2;
	struct sockaddr_in addr1,addr2;
	unsigned short port = 5353;
	int size;

	printf("server started!\n");
	sock1 = socket(AF_INET,SOCK_STREAM,0);
	if(sock1 < 0){
		printf("sock failed!\n");
		return -1;
	}
	
	memset(&addr1,0,sizeof(addr1));
	addr1.sin_family = AF_INET;
	addr1.sin_addr.s_addr = htonl(INADDR_ANY);
	addr1.sin_port = htons(port);
    
	if(bind(sock1,(struct sockaddr *)(&addr1),sizeof(struct sockaddr)) < 0){
		printf("bind failed!\n");
	}
	
	if(listen(sock1,5) < 0){
		printf("listen failed!\n");
	}
	while(1){
		size = sizeof(struct sockaddr_in);
		sock2 = accept(sock1,(struct sockaddr *)(&addr2),&size);
		if(sock2 < 0){
			printf("accept failed!\n");
			break;
		}
		printf("accpet success from %#x:%#x",
				ntohl(addr2.sin_addr.s_addr),ntohs(addr2.sin_port));

		if(write(sock2,"hello\n",8) < 0){
			printf("write error!\n");
		}
		close(sock2);
	}
	close(sock1);

	return 0;
}
