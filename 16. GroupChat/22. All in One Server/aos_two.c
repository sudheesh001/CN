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
#include <sys/poll.h>

#define INTERNAL_PATH "/tmp/internal_path"
#define SERVING_PATH "/tmp/serving_path_two"
#define MAX_CLIENT 10

int newsockfd;

void serv_client();

ssize_t
sock_fd_read(int sock, void *buf, ssize_t bufsize, int *fd)
{
    ssize_t     size;

    if (fd) {
        struct msghdr   msg;
        struct iovec    iov;
        union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        } cmsgu;
        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg (sock, &msg, 0);
        if (size < 0) {
            perror ("recvmsg");
            exit(1);
        }
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET) {
                fprintf (stderr, "invalid cmsg_level %d\n",
                     cmsg->cmsg_level);
                exit(1);
            }
            if (cmsg->cmsg_type != SCM_RIGHTS) {
                fprintf (stderr, "invalid cmsg_type %d\n",
                     cmsg->cmsg_type);
                exit(1);
            }

            *fd = *((int *) CMSG_DATA(cmsg));
            printf ("received fd %d\n", *fd);
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0) {
            perror("read");
            exit(1);
        }
    }
    return size;
}

void perror(const char*s){
	printf("Error: %s\n", s);
	exit(1);
}

int main(int argc, char *argv[]){

	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
		perror("sock error");

	struct sockaddr_un serv;
	bzero((char *) &serv, sizeof(serv));
	serv.sun_family = AF_UNIX;
	strncpy(serv.sun_path, INTERNAL_PATH, sizeof(serv.sun_path) - 1 );
	//unlink(serv.sun_path);


	int g = connect(sockfd, (struct sockaddr*) &serv, sizeof(serv));
	if( g < 0)
		perror("failed to connect");

	// int t=-1;
	// int *h;
	int p;
	while(1){
		printf("parent running\n");
		char ptr[16];
		//t++;
		//printf("waiting for read fdsss\n");
		int n = sock_fd_read(sockfd, ptr, sizeof(ptr), &newsockfd);
		if( n < 0 )
			perror("failed to get sockfd");
		//printf("socket fd recvd by service server %d\n", n);
		if( (p = fork()) < 0)
			perror("fork error");
		else if(p == 0){
			//child
			serv_client();
			break;
		}
		else{//parent
			close(newsockfd);
		}

		// h[t] = t;
		// pthread_create(&p[t], NULL, (void*) &serv_client, (void*)&h[t]);
	}

	
	return 0;
}

void serv_client(){

	///serve the client
	//int my_id=* ((int*)arg);
	printf("thread started\n");
	int n =  write(newsockfd,"you are connected to service server",36);
	if( n < 0)
		perror("failed to write back to client"); 
		char buf[100];
		
		while(1){
			bzero(buf, 100);
			n = recv(newsockfd, buf, sizeof(buf), 0);
			
			if( n < 0)
				perror("failed to read from client");
			if( n == 0) break;
			buf[n]='\0';

			printf("Message from client::%s ",buf );

			n =  send(newsockfd,"got msg",8,0);
			if( n < 0)
				perror("failed to write back to client");
		}
		printf("exiting child....\n");

}