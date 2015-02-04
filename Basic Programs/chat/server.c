#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int t;
	mkfifo("pipew1",0666);
	mkfifo("piper1",0666);
	mkfifo("piper2",0666);
	mkfifo("pipew2",0666);
	mkfifo("piper3",0666);
	mkfifo("pipew3",0666);
	if(t==-1){
		perror("Error creating pipe");
	}
	int pid=0;
	pid=fork();
	
	if(pid>0){
		int npid=0;
		npid=fork();
		if(npid>0){
			while(1){
			int fd1=open("pipew1",O_RDONLY);
			char buf[100];
			read(fd1,buf,100);
			int fd2=open("piper2",O_WRONLY);
			write(fd2,buf,100);
			int fd3=open("piper3",O_WRONLY);
			write(fd3,buf,100);
			close(fd1);
			close(fd2);
			close(fd3);
			}
		}
		else if(npid==0){
			while(1){
			int fd1=open("pipew2",O_RDONLY);
			char buf[100];
			read(fd1,buf,100);
			int fd2=open("piper1",O_WRONLY);
			write(fd2,buf,100);
			int fd3=open("piper3",O_WRONLY);
			write(fd3,buf,100);
			close(fd1);
			close(fd2);
			close(fd3);
			}
		}
		
	}
	else if(pid==0){
		while(1){
			int fd1=open("pipew3",O_RDONLY);
			char buf[100];
			read(fd1,buf,100);
			int fd2=open("piper1",O_WRONLY);
			write(fd2,buf,100);
			int fd3=open("piper2",O_WRONLY);
			write(fd3,buf,100);
			close(fd1);
			close(fd2);
			close(fd3);
			}
	}

	return 0;
}