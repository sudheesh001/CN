#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h>

union semun {
 int val;
 struct semid_ds *buf;
 unsigned short int *array;
 };
void sem_change(int sem_id, int sem_no, int amount)
{
 struct sembuf tmp;
 tmp.sem_num=sem_no;
 tmp.sem_flg=0;
 tmp.sem_op=amount;
 if(semop(sem_id, &tmp, 1)==-1)
  {
   printf("Sem_op error\n");
   exit(1);
  }
}

int main(int argc, char const *argv[])
{
	int fd;
	fd=open("input.txt",O_RDONLY);
	if(fd==-1){
		perror("cannot open file");
	}	
	key_t k=ftok(".",10);
	int semid;
	semid=semget(k,2,IPC_CREAT|0660);
	if(semid==-1){
		perror("Semaphore init error\n");
		exit(1);
	}
	union semun tmp;
	tmp.val=1;
	semctl(semid,0,SETVAL,tmp);
	tmp.val=0;
	semctl(semid,1,SETVAL,tmp);
	int n;
	char buf;
	
	close(0);
	fcntl(fd,F_DUPFD,0);
	int pid=0;
	pid=fork();
	if(pid>0){
		
		while(1){
		printf("\tFilereading happening from process one\n");
		
		sem_change(semid,0,-1);
		if((n=read(fd,&buf,1))>0)
		{	
			while(buf!='\n'){
			printf("%c",buf );
			read(fd,&buf,1);
			}
		}
		else{
			sem_change(semid,0,1);
			break;
		}
		sem_change(semid,0,1);
		sleep(3);
		}
	}
	
	else if(pid==0){
			execve("fileread2",NULL,NULL);
	}
	
	return 0;
}