#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"

int sendall(int s,char *buf,int *len)
{
    int total = 0;
    int bytesleft = *len;
    int n;
    while ( total < *len)
    {
        n = send(s,buf+total,bytesleft,0);
        if ( n == -1 )break;
        total +=n;
        bytesleft -= n;
    }
    *len = total;
    return n == -1 ?-1 :0;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
    fd_set master;
    fd_set read_fds;
    int fdmax;
    int listener;
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char buf[256];
    int nbytes;
    
    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;
    int i,j,rv;

    struct addrinfo hints,*ai,*p;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (( rv = getaddrinfo(NULL,PORT,&hints,&ai)) != 0){
        fprintf(stderr, "select server : %s\n",gai_strerror(rv));
        exit(1);
    }
    for (p = ai; p != NULL;p = p->ai_next){
        listener = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if ( listener < 0){
            continue;
        }

        setsockopt(listener , SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int));

        if ( bind(listener, p->ai_addr,p->ai_addrlen) < 0){
            close(listener);
            continue;
        }
        break;
    }

    if ( p == NULL )
    {
        fprintf(stderr,"selectserver : failed to bind \n");
        exit(2);
    }
    freeaddrinfo(ai);

    if ( listen(listener,10) == -1){
        perror("listen");
        exit(3);
    }
    FD_SET(listener, &master);

    fdmax = listener;

    for(;;)
    {
        read_fds = master;
        if ( select (fdmax+1,&read_fds,NULL,NULL,NULL) == -1)
        {
            perror("select");
            exit(4);
        }
    }

    for( i =0;i <= fdmax;i++){
        if (FD_ISSET(i,&read_fds))
        {
            if ( i == listener)
            {
                addrlen = sizeof(remoteaddr);
                newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
                if ( newfd == -1)
                {
                    perror("accept");
                }
                else {
                    FD_SET(newfd,&master);
                    if(newfd > fdmax)
                    {
                        fdmax = newfd;
                    }
                    printf("select server : new connection from %s on socket %d\n",inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP,INET6_ADDRSTRLEN),newfd);
                }
            }
            else{
                if ((nbytes = recv(i,buf,sizeof(buf),0)) <= 0){
                    if ( nbytes == 0) {
                        printf("select server : socket %d hung up \n",i);
                    } else {
                        perror("recv");
                    }
                    close(i);
                    FD_CLR(i,&master);
                }else {
                    for(j = 0; j <= fdmax;j++)
                    {

                        if (FD_ISSET(j,&master)){
                            if ( j != listener && j != i){
                                if (send(j,buf,nbytes,0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                }
            }
        }

               
    }
return 0;
}
