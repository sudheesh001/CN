#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>

int main(){
	key_t k;
	char *x;
	k=ftok(".",15);
	int shm_id1=shmget(k,30,IPC_CREAT|0660);
	if(shm_id1==-1){
		perror("Shmget1 error!");
		exit(1);
	}
	
	x=(char *)shmat(shm_id1,NULL,0);
	
	if(*x==-1){
		perror("shmat error!");
		exit(1);
	}
	char* t="readfifo";
	strcpy(x,t);
	mkfifo(t,0666);
	int fd,n;
	char c;
	// while(1){
	fd=open(t,O_RDWR|O_NONBLOCK);
	while((n=read(fd,&c,1))>0){
		printf("%c",c);
	}
	close(fd);
	// }
	
	return 0;

}