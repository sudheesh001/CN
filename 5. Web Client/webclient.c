#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}

	/* Need to take an input URL and parse it */
	regex_t urlPreg;
	regex_t errorResponse;
	regex_t okResponse;
	char *string=argv[1];	
	if(regcomp(&urlPreg,"(http://)([^/:. ]+(\\.[^/:. ]+)+)(/?.*)",0 | REG_ICASE | REG_EXTENDED)!=0)
		puts("URL regex compilation failed");					
	if(regcomp(&errorResponse,"^HTTP/1\\.[0-9]+ [34][0-9][0-9]",0 | REG_ICASE | REG_EXTENDED | REG_NOSUB)!=0)
		puts("Error response regex failed to compile");		
	if(regcomp(&okResponse,"^HTTP/1\\.[0-9]+ 2[0-9][0-9]",0 | REG_ICASE | REG_EXTENDED | REG_NOSUB)!=0)
		puts("Ok response regex failed to compile");	
	regmatch_t matchedArray[6];
	if(regexec(&urlPreg,string,(size_t)6,matchedArray,0)==REG_NOMATCH)
	{
		puts("Please enter a proper URL");			
		return 1;
	}

	char hostName[1024];
	char requestFileName[1024];
	char saveFileName[1024];
	strncpy(hostName,&string[matchedArray[2].rm_so],(matchedArray[2].rm_eo-matchedArray[2].rm_so));
	hostName[(matchedArray[2].rm_eo-matchedArray[2].rm_so)]='\0';
	strncpy(requestFileName,&string[matchedArray[4].rm_so],(matchedArray[4].rm_eo-matchedArray[4].rm_so));
	requestFileName[(matchedArray[4].rm_eo-matchedArray[4].rm_so)]='\0';
	printf("The Host is :%s\n",hostName);
	printf("The requested filename is:%s\n",requestFileName);
	
	struct addrinfo hints;	
	struct addrinfo *result=0;	
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=0;	
	if(getaddrinfo((const char *)hostName,0,&hints,&result)!=0)		// get the address for name without the protocol part			
	{	perror("No name record exists for the host: "); return 1;	}
	
	((struct sockaddr_in *)(result->ai_addr))->sin_port=htons(80);	//set the port no to 80		
	
	int res = connect(sock, (struct sockaddr*)(result->ai_addr),result->ai_addrlen);	//try to connect
	if(res < 0) {
		perror("Error connecting: ");
		exit(1);
	}
	printf("Connected to %s\n", hostName);
	
	return 0;
}