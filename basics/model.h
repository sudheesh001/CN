#include <stddef.h>
struct adrinfo 
{
    int     ai_flags; 
    int     ai_family;
    int     ai_socketype;
    int     ai_protocol;
    size_t  ai_addrlen;
    struct sockaddr *ai_add;
    char    *ai_canonname;
    struct addrinfo *ai_next;
};   
