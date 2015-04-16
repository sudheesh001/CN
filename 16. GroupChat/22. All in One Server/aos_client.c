#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/un.h>

#define SERVING_PATH1 "/tmp/serving_path_one"
#define SERVING_PATH2 "/tmp/serving_path_two"
#define SERVING_PATH3 "/tmp/serving_path_three"
//#define MAX_SERVICE_PROVIDER 3

/** we are gonna use service no. 1, 2 and 3 **/

void perror(const char *s){
	printf("Error: %s", s);
	exit(1);
}

int main(int argc, char *argv[] ){
	//socket
	int sockfd;
	char SERVING_PATH[100];
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("socket error");
	}

	int argIndex = 0;
	for(argIndex; argIndex<argc; argIndex++)
		printf("%s\n", argv[argIndex]);

	//connect
	struct sockaddr_un serv;
	bzero((char *) &serv, sizeof(serv));
	serv.sun_family = AF_UNIX;
	if(argv[argIndex-1] == 1)
		strncpy(serv.sun_path, SERVING_PATH1, sizeof(serv.sun_path) - 1 );
	else if(argv[argIndex-1] == 2)
		strncpy(serv.sun_path, SERVING_PATH2, sizeof(serv.sun_path) - 1 );
	else
		strncpy(serv.sun_path, SERVING_PATH3, sizeof(serv.sun_path) - 1 );
	//unlink(serv.sun_path);


	int p = connect(sockfd, (struct sockaddr*) &serv, sizeof(serv));
	if( p < 0)
		perror("failed to connect");

	char buf[100];
	//
		bzero(buf, 100);
		int n = read(sockfd, buf, sizeof(buf));
		if( n < 0 )
			perror(" failed to read message from server");
		//
		buf[n]= '\0';
		printf("%s\n",buf );
		
	while(1){

		bzero(buf, 100);
		printf("enter your message : \n");
		// fgets(buf,100,stdin);
		int nn=read(0,buf,100);
		buf[nn]='\0';

		printf("sending %s\n", buf);

		int n = write ( sockfd, buf, sizeof(buf));
		if( n < 0 )
			perror(" failed to write message to server");

		bzero(buf, 100);
		n = read(sockfd, buf, sizeof(buf));
		if( n < 0 )
			perror(" failed to read message from server");

		printf("message from server : %s\n",buf );

	}

	return 0;
}