#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int t;
	mkfifo("./piper1",0666);
	mkfifo("./pipew1",0666);
	int pid=0;
	pid=fork();
	int fd;
	char buf[12];
	while(1){
	if(pid>0){

		fd=open("./piper1",O_RDWR|O_NONBLOCK);
		int n=0;
		while((n=read(fd,&buf,12))>0){
			buf[strlen(buf)-1]='\0';
			printf("%s\n",buf );
		}
		close(fd);
	}
	else if(pid==0){
		int i=0;
		for (i = 0; i < 12; ++i)buf[i]='\0';
		read(0,&buf,12);
		fd=open("./pipew1",O_RDWR|O_NONBLOCK);
		write(fd,&buf,12);
		close(fd);
	}
	}
}