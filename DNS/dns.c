#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>

struct hostent *gethostbyname(const char *name);
const char *inet_ntop(int af, const void *src,			                                char *dst, socklen_t size);

int main( int argc, char *argv[])
{
	char **p;
	struct hostent *host;
	char ip[32];
	char *name;
	name = argv[1];

	if( (host = gethostbyname(name)) == NULL){
		printf("get host error!\n");
	}
	
	printf("name is %s,official name is %s\n",name,host->h_name);

	for(p = host->h_aliases; *p != NULL; p++){
		printf("aliases %s\n",*p);
	}

	switch(host->h_addrtype){
		case AF_INET:
		case AF_INET6:
			for(p = host->h_addr_list; *p != NULL; p++){
				printf("address %s \n",
						inet_ntop(host->h_addrtype, *p, ip, sizeof(ip)));	
			}
			break;
		default :
			break;
	}
	return 0;
}
