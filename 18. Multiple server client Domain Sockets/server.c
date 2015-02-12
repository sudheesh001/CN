/*
 * socket demonstrations:
 * this is the server side of a "unix domain" socket connection
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main()
{
    int fd, clientfd;
    socklen_t len;
    char buf[80];
    struct sockaddr_un r, q;

    /*
     * create a "socket", like an outlet on the wall -- an endpoint for a
     * connection
     */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");  /* (there's no file name to put in as argument
                            * to perror... so for lack of anything better,
                            * the syscall name is the usual choice here) */
        return(1);
    }

    /* "unix domain" -- rendezvous is via a name in the unix filesystem */
    memset(&r, '\0', sizeof r);
    r.sun_family = AF_UNIX;
    strcpy(r.sun_path, "/tmp/something");
    /*
     * "binding" involves creating the rendezvous resource, in this case the
     * socket inode (a new kind of "special file"); then the client can
     * "connect" to that.
     */
    if (bind(fd, (struct sockaddr *)&r, sizeof r)) {
        perror("bind");
        return(1);
    }
    /*
     * The "listen" syscall is required.  It says the length of the queue for
     * incoming connections which have not yet been "accepted".
     * 5 is a suitable value for your assignment four.
     * It is not a limit on the number of people you are talking to; it's just
     * how many can do a connect() before you accept() them.
     */
    if (listen(fd, 5)) {
	perror("listen");
	return(1);
    }

    /* process client requests (usually in a loop) */
    /*
     * The accept() syscall accepts a connection and gives us a new socket
     * file descriptor for talking to that client.  We can read and write the
     * socket.  Other than that it functions much like a pipe.
     */
     while(1) {
	    len = sizeof q;
	    if ((clientfd = accept(fd, (struct sockaddr *)&q, &len)) < 0) {
	        perror("accept");
	        return(1);
	    }

	    /*
	     * Usually we'd have a more complex protocol than the following, but
	     * in this case we're just reading one line or so and outputting it.
	     */
	    if ((len = read(clientfd, buf, sizeof buf - 1)) < 0) {
	        perror("read");
	        return(1);
	    }
	    /* The read is raw bytes.  This turns it into a C string. */
	    buf[len] = '\0';

	    printf("The other side said: %s\n", buf);
	}

    /*
     * Closing the socket makes the other side see that the connection is
     * dropped.  It's how you "hang up".
     */
    close(clientfd);

    /*
     * A unix domain socket binding is reclaimed upon process exit, but the
     * inode is not.  You have to unlink (delete) it.
     */
    close(fd);
    unlink("/tmp/something");
    return(0);
}