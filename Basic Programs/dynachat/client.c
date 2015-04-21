#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>

int main(int argc,const char* argv){
	key_t k;
	char *y;
	k=ftok(".",15);
	int shm_id1=shmget(k,30,IPC_CREAT|0660);
	if(shm_id1==-1){
		perror("Shmget1 error!");
		exit(1);
	}
	
	y=(char *)shmat(shm_id1,NULL,0);
	
	if(*y==-1){
		perror("shmat error!");
		exit(1);
	}
	printf("%s\n",y );
	mkfifo(y,0666);
	int fd,n;
	char buf[10];
	int pid;
	pid=fork();
	if(pid>0){

	}
	else{
	// while(1){
	fd=open(y,O_WRONLY);
	read(0,buf,10);
	write(fd,buf,strlen(buf));
	close(fd);
	}
	return 0;
}