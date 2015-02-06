#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int sock1;
	struct sockaddr_in addr1;
	unsigned short port = 5353;
	int recv_bytes;
	char buffer[512];

	sock1 = socket(AF_INET,SOCK_STREAM,0);
	if(sock1 < 0){
		printf("sock failed!\n");
	}

	memset(&addr1,0,sizeof(addr1));
	
	addr1.sin_family = AF_INET;
	addr1.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr1.sin_port = htons(port);

	if(connect(sock1,(struct sockaddr *)(&addr1),sizeof(struct sockaddr)) < 0){
		printf("connect failed!\n");
		close(sock1);
	}

	recv_bytes = read(sock1,buffer,512);
	if(recv_bytes < 0){
		printf("read error!\n");
		close(sock1);
	} else if (recv_bytes < 512){
		buffer[recv_bytes] = '\0';
		printf("%s",buffer);
		close(sock1);
	}else{
		printf("buffer full!\n");
	}
	
	return 0;
}
