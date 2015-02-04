#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <stdlib.h>

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

int main(){
	key_t k=ftok(".",23);
	int semid;
	semid=semget(k,2,IPC_CREAT|0660);
	if(semid==-1){
		perror("Semaphore init error\n");
		exit(1);
	}
	union semun tmp;
	tmp.val=1;
	semctl(semid,0,SETVAL,tmp);
	semctl(semid,1,SETVAL,tmp);
	int *x;int* y;
	k=ftok(".",15);
	int shm_id1=shmget(k,30,IPC_CREAT|0660);
	k=ftok(".",11);
	int shm_id2=shmget(k,30,IPC_CREAT|0660);
	if(shm_id1==-1){
		perror("Shmget1 error!");
		exit(1);
	}
	if(shm_id2==-1){
		perror("Shmget2 error!");
		exit(1);
	}
	x=(int *)shmat(shm_id1,NULL,0);
	if(*x==-1){
		perror("shmat error!");
		exit(1);
	}
	y=(int *)shmat(shm_id2,NULL,0);
	if(*y==-1){
		perror("shmat error!");
		exit(1);
	}
	*x=0;
	*y=0;
	
	while(1){
		sem_change(semid,0,-1);
		*x=*y+1;
		sleep(4);
		printf(" \t x = %d, y = %d\n",*x,*y);
		sem_change(semid,0,1);
	}
}