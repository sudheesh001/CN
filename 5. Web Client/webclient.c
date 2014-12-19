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
	
	return 0;
}