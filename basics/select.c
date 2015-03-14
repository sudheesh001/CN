#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define STDIN 0
int main(void)
{
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = 2;
    tv.tv_usec = 5000;

    FD_ZERO(&readfds);
    FD_SET(STDIN,&readfds);

    select(STDIN+1,&readfds,NULL,NULL,&tv);
    if (FD_ISSET(STDIN,&readfds))
        printf("A key was pressed");
    else
        printf("Timed out");
    return 0;
}
