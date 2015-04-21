#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
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
	int n=0,count=0;
	char buf;
	key_t k=ftok(".",10);
	int semid;
	semid=semget(k,1,IPC_CREAT|0660);
	if(semid==-1){
		perror("Semaphore init error\n");
		exit(1);
	}
	union semun tmp;
	tmp.val=0;
	semctl(semid,0,SETVAL,tmp);

	printf("\tFilereading from process two \n");
	printf("\t***************************************\n");
	sem_change(semid,0,-1);
	while((n=read(0,&buf,1))>0 && count<=5){
		if(buf!='\n')
			printf("%c",buf );
		else{
			count++;printf("\n");}
	}
	printf("\t***********************************\n");
	return 0;
}