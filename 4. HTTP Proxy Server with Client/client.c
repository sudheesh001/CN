#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#define VRSN 3
#define STDIN 0
#define STDOUT 1
#define MAX_STR_LEN 1500
#define ERROR 0	

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void extract_URL(char *url, char *hostname, int *port, char *identifier)
{
    char protocol[MAX_STR_LEN], scratch[MAX_STR_LEN], *ptr=0, *nptr=0;
    
    strcpy(scratch, url);
    ptr = (char *)strchr(scratch, ':');
    if (!ptr)
    {
	fprintf(stderr, "Wrong url: no protocol specified\n");
	exit(ERROR);
    }
    strcpy(ptr, "\0");
    strcpy(protocol, scratch);
    if (strcmp(protocol, "http"))
    {
	fprintf(stderr, "Wrong protocol: %s\n", protocol);
	exit(ERROR);
    }

    strcpy(scratch, url);
    ptr = (char *)strstr(scratch, "//");
    if (!ptr)
    {
	fprintf(stderr, "Wrong url: no server specified\n");
	exit(ERROR);
    }
    ptr += 2;

    strcpy(hostname, ptr);
    nptr = (char *)strchr(ptr, ':');
    if (!nptr)
    {
	*port = 80; /* use the default HTTP port number */
	nptr = (char *)strchr(hostname, '/');
    }
    else
    {	
	sscanf(nptr, ":%d", port);
	nptr = (char *)strchr(hostname, ':');
    }

    if (nptr)
      *nptr = '\0';

    nptr = (char *)strchr(ptr, '/');
    
	if (!nptr)
	    {
	fprintf(stderr, "Wrong url: no file specified\n");
	exit(ERROR);
    }
    
    strcpy(identifier, nptr);

}


void createRequest(char *request,char * hostname,char *identifier) 
{	
	strcpy (request ,"GET ");
	request = (char *)strcat( request, identifier);
	request = (char *)strcat( request, " HTTP/1.0\r\nHOST: ");
	request = (char *)strcat( request, hostname);
	request = (char *)strcat( request, "\r\n"); 
	request = (char *)strcat( request, "\r\n"); 
        //return request;
}


int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf[MAX_STR_LEN];
    
    char request[MAX_STR_LEN];
    bzero(request,MAX_STR_LEN);
    char identifier[MAX_STR_LEN];
    //char request =NULL;
    char url[MAX_STR_LEN]; 
    char hostname[MAX_STR_LEN];
    int port;
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
 
    port = atoi(argv[2]);
    //int n =  strlen(argv[3]);
    strcpy(url,argv[3]); 
    printf("url here is %s\n",url);
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
     {
	error("ERROR opening socket\n");
        exit(0);
     }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
     {
        error("ERROR connecting\n");
	exit(0);
     }
     
     //url[n] = '\0';
     
     //extract URL from the request receive from client
     printf("URL is %s\n",url); 
     extract_URL(url, hostname, &port, identifier);
     
     printf("hostname is %s , identifier is %s, port is %d\n",hostname,identifier,port);
     //create request from above information, return completed request
     createRequest(request,hostname,identifier);
     
     printf("request to send to server from client is %s\n",request);
     if (send(sockfd,request,strlen(request), 0) == -1)
	 perror("error in sending MSG to server\n");
     int size = 0;
     int nbytes=0;
     memset(buf,0,MAX_STR_LEN);
     while(nbytes  = (recv(sockfd, buf, MAX_STR_LEN,0)))
     {     
        if(nbytes<0)
         {
           perror("recv error from server \n");
           exit(0);
         }
	printf("%s",buf);
	size +=nbytes;
	memset(buf,0,MAX_STR_LEN);
        if(nbytes < 1500)
          break;
     }
    printf("\n   Total recieved response bytes: %d\n",size);
    close(sockfd);          
    return 0;
}

