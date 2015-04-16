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

#define INTERNAL_PATH "/tmp/internal_path"
#define SERVING_PATHONE "/tmp/serving_path_one"
#define SERVING_PATHTWO "/tmp/serving_path_two"
#define SERVING_PATHTHREE "/tmp/serving_path_three"

int newsockfd[3];
int sockfd[3];
int service_provider[3];

pthread_t listen_fds[3];

void perror(const char*s){
	printf("Error: %s\n", s);
	exit(1);
}

ssize_t
sock_fd_write(int sock, void *buf, ssize_t buflen, int fd)
{
    ssize_t     size;
    struct msghdr   msg;
    struct iovec    iov;
    union {
        struct cmsghdr  cmsghdr;
        char        control[CMSG_SPACE(sizeof (int))];
    } cmsgu;
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        printf ("passing fd %d\n", fd);
        *((int *) CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        printf ("not passing fd\n");
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0)
        perror ("sendmsg");
    return size;
}

void listening_fds(void* arg);

int main(){
	int i;

	int sockfd_internal = socket(AF_UNIX, SOCK_STREAM, 0);
	if( sockfd_internal < 0 )
		perror("internal socket error");

	struct sockaddr_un serv_internal;
	bzero( (char*) &serv_internal, sizeof(serv_internal));
	serv_internal.sun_family = AF_UNIX;
	strncpy(serv_internal.sun_path, INTERNAL_PATH, sizeof(serv_internal.sun_path) - 1);
	unlink(serv_internal.sun_path);

	if( bind(sockfd_internal, (struct sockaddr*) &serv_internal, sizeof(serv_internal)) < 0)
			perror("internal bind error");

	listen(sockfd_internal, 3);

	for(i=0; i<3 ;i++){
		struct sockaddr_un cli_internal;
		bzero((char*)&cli_internal, sizeof(cli_internal));
		int cli_len_internal = sizeof(cli_internal);
		service_provider[i] = accept(sockfd_internal, (struct sockaddr*) &cli_internal, &cli_len_internal);
		if( service_provider[i] < 0)
			perror("failed to accept internal connection");
		
		printf("Server %d connected\n", i+1);
	}
	printf("All internal servers connected\n");


	for(i=0; i<3; i++){
		sockfd[i] = socket(AF_UNIX, SOCK_STREAM, 0);
		if(sockfd[i] < 0)
			perror("socket error");
	} 
	printf("socket declared\n");
	struct sockaddr_un serv[3];
	for(i=0; i<3; i++){
		bzero( (char*) &serv[i], sizeof(serv[i]) );
		serv[i].sun_family = AF_UNIX;
		if(i==0)
			strncpy( serv[i].sun_path, SERVING_PATHONE, sizeof(serv[i].sun_path) - 1);
		else if( i==1)
			strncpy( serv[i].sun_path, SERVING_PATHTWO, sizeof(serv[i].sun_path) - 1);
		else
			strncpy( serv[i].sun_path, SERVING_PATHTHREE, sizeof(serv[i].sun_path) - 1);
		unlink(serv[i].sun_path);
	}
	printf("struct\n");
	for(i=0; i < 3 ;i++){
		printf("binding %d\n", i+1);
		if( bind(sockfd[i], (struct sockaddr*) &serv[i], sizeof(serv[i]) ) < 0){
			
			perror("bind error");
		}
	}
	printf("running\n");

	
	printf("hello\n");
	int h[3];
	for(i = 0; i<3; i++){
		printf("run %d\n",i );
		// tot++;
		h[i]=i;
		//lol[tot] = tot;
		printf("blah blah: \n");
		pthread_create(&listen_fds[i], NULL, (void*) &listening_fds ,(void*) &h[i] );
	}

	for(i=0;i<3;i++)
		pthread_join(listen_fds[i], NULL);
	// //client listen
	// for(i = 0 ; i<3; i++)
	// 	listen(sockfd[i], 5);

	return 0;
}


void listening_fds(void *arg){
	int my_id=* ((int*)arg);
	printf("thread %d is listenining\n",my_id+1 );
	listen(sockfd[my_id], 5);

	while(1){
		struct sockaddr_un cli;
		bzero((char*) &cli, sizeof(cli));
		int cli_len = sizeof(cli);

		int newsockfd = accept(sockfd[my_id], (struct sockaddr*) &cli, &cli_len);
		if( newsockfd < 0)
			perror("accept error");

		int n = sock_fd_write(service_provider[my_id], "1", 2, newsockfd);		
		if( n < 0)
		 	perror("fd error");

		 close(newsockfd);
	}


}