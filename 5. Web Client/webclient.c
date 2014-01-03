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
	int sock = socket(AF_INET, SOCK_STREAM, 0);		// create a socket
	if(sock < 0) {
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
	
	char getRequest[2048];						//initialize the getRequest buffer
	memset(getRequest,0,sizeof(getRequest));	//zero out the buffer
	if(strcmp(requestFileName,"")==0 || strcmp(requestFileName,"/")==0)
	{
		sprintf(getRequest,"GET / HTTP/1.0\r\nHost: %s\r\n\r\n",hostName);	//construct the HTTP GET request
		sprintf(saveFileName,"index.html",0);
	}
	else
	{
		regmatch_t matchedFileName[3];
		regex_t fileNameRegex;
		sprintf(getRequest,"GET %s HTTP/1.0\r\nHost: %s\r\n\r\n",requestFileName,hostName);	//construct the HTTP GET request
		if(regcomp(&fileNameRegex,"(/[^/]+)*(/?)",0 | REG_ICASE | REG_EXTENDED)!=0)
		{	perror("fileNameRegex failed to compile:"); return 1;	}
		if(regexec(&fileNameRegex,requestFileName,(size_t)3,matchedFileName,0)==REG_NOMATCH)
		{	perror("Filename regex did not match:"); return 1;	}		
		/*int i=0;		
		puts("Here");
		for(i=matchedFileName[1].rm_so+1;i<matchedFileName[1].rm_eo;i++)
			putc(requestFileName[i],stdout);
			printf("\n");
		for(i=matchedFileName[2].rm_so;i<matchedFileName[2].rm_eo;i++)
			putc(requestFileName[i],stdout);		*/
		if(requestFileName[matchedFileName[2].rm_so]=='/')
			sprintf(saveFileName,"index.html",0);
		else if(strcmp(&requestFileName[matchedFileName[2].rm_so],"")==0)		
			strncpy(saveFileName,&requestFileName[matchedFileName[1].rm_so+1],matchedFileName[1].rm_eo-matchedFileName[1].rm_so);
	}
	printf("The save filename is %s\n",saveFileName);
	printf("The GET request is \n%s",getRequest);
	
	
	/*NOW SEND THE REQUEST*/
	if(send(sock,(const char *)getRequest,strlen(getRequest),0)!=strlen(getRequest))
	{	perror("ERROR: Send Failed");	return 1;	}
	
	/*PREPARE TO RECEIVE */		
	char buf[1024*1024];					//declare a buffer of 1MB
	memset(&buf,0,sizeof(buf));			//zero out the buffer		
	FILE *filePtr=0;
	int recvdBytes=recv(sock, buf, 1024*1024, 0);
	int writeFlag=0;		//flag to check whether writing to file has started
	while(recvdBytes>0)		//receive stuff until the server disconnects, wherein we receive a 0
	{ 
		int i=0;
		printf("Totaly bytes received is %d\n",recvdBytes);		
		
		//if we've already parsed and started to write to files, then we contnue to write
		//whatever we get into the file
		if(writeFlag==1)				
		{
			i=0;
			while(i<recvdBytes)
			{
				putc(*(buf+i),filePtr);
				i++;
			}				
		}
		//parse the response		
		if(regexec(&errorResponse,buf,0,0,0)!=REG_NOMATCH && writeFlag==0)		
		{
			puts("Received some error!");
			return 1;				
		}							
		if(regexec(&okResponse,buf,0,0,0)!=REG_NOMATCH && writeFlag==0)
		{						
			char *subStr=0;			
			subStr=strstr(buf,"\r\n\r\n");
			puts("The string starting after the substring is");			
			subStr=subStr+4;			
			filePtr=fopen(saveFileName,"w+");
			i=(subStr-buf);
			while(i<recvdBytes)
			{
				putc(*(buf+i),filePtr);
				i++;
			}
			writeFlag=1;
		}		
		memset(&buf,0,sizeof(buf)); 	
		recvdBytes=recv(sock, buf, 1024*1024, 0);	
	}

	shutdown(sock,SHUT_RDWR);
	fclose(filePtr);
	
	return 0;
}
