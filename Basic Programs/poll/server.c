#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>

int main(){
struct pollfd fd[3];
char buf[10];
fd[0].fd=open("./pipew1",O_RDONLY);
fd[1].fd=open("./pipew2",O_RDONLY);
fd[2].fd=open("./pipew3",O_RDONLY);
if(fd[0].fd==-1){
	perror("Error opening file");
}
if(fd[1].fd==-1){
	perror("Error opening file");
}
if(fd[2].fd==-1){
	perror("Error opening file");
}
int i;
for (i = 0; i < 3; ++i)
	fd[i].events=POLLIN;

int b,c;
while(1){
	poll(fd,3,75);
	if(fd[0].revents & POLLIN){
		printf("First client messaged\n");
		b=read(fd[0].fd,buf,10);
		printf("%s\n",buf);
		c=open("piper2",O_WRONLY);
		write(c,buf,b);
		close(c);
		c=open("piper3",O_WRONLY);
		write(c,buf,b);
		close(c);
	}

	if(fd[1].revents & POLLIN){
		b=read(fd[1].fd,&buf,10);
		printf("%s\n",buf);
		c=open("piper1",O_WRONLY);
		write(c,&buf,b);
		close(c);
		c=open("piper3",O_WRONLY);
		write(c,&buf,b);
		close(c);
	}

	if(fd[2].revents & POLLIN){
		b=read(fd[2].fd,&buf,10);
		printf("%s\n",buf);
		c=open("piper1",O_WRONLY);
		write(c,&buf,b);
		close(c);
		c=open("piper2",O_WRONLY);
		write(c,&buf,b);
		close(c);
	}
}
	exit(0);
	return 0;
}