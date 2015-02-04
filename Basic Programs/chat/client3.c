#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int t;
	mkfifo("piper3",0666);
	mkfifo("pipew3",0666);
	int pid=0;
	pid=fork();
	int fd;
	char buf[100];
	if(pid>0){
	while(1){

		fd=open("piper3",O_RDONLY);
		int n=0;
		while((n=read(fd,buf,100))>0){
			printf("%s\n",buf );
		}
		close(fd);
	}
	}
	else if(pid==0){
	while(1){
	int i=0;		
		for (i = 0; i < 100; ++i)buf[i]='\0';
		read(0,buf,100);
		fd=open("pipew3",O_WRONLY);
		write(fd,buf,100);
		close(fd);
	}
	}
}