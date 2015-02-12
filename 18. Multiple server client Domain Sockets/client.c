/*
 * socket demonstrations:
 * this is the client side of a "unix domain" socket connection
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main()
{
    int fd, len;
    struct sockaddr_un r;

    /*
     * create a "socket", like an outlet on the wall -- an endpoint for a
     * connection
     */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");  /* (there's no file name to put in as argument to
                            *  perror... so for lack of anything better, the
                            *  syscall name is the usual choice here) */
        return(1);
    }

    /* "unix domain" -- rendezvous is via a name in the unix filesystem */
    memset(&r, '\0', sizeof r);
    r.sun_family = AF_UNIX;
    strcpy(r.sun_path, "/tmp/something");  /* and this is the name */
    /* The server does an "accept"; the client does a "connect": */
    if (connect(fd, (struct sockaddr *)&r, sizeof r)) {
        perror("connect");
        return(1);
    }
    while(1){
	    /* at this point we have connected to the socket successfully */

	    /* send the transmission */
	    if ((len = write(fd, "Hello", 5)) != 5) {
	        perror("write");
	        return(1);
	    }
	}

    /*
     * exiting reclaims all resources, including open files, open pipes,
     * open sockets
     */
    return(0);
}