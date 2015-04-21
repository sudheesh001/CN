#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>

int *x,*y;
void handler(int signalno){
*x=*y+1;
printf("\t x = %d , y = %d \n",*x,*y );
}

int main(){
key_t k=ftok(".",23);
int shmid1=shmget(k,30,IPC_CREAT|0660);
int shmid2=shmget(k,30,IPC_CREAT|0660);
x=(int *)shmat(shmid1,NULL,0);
y=(int *)shmat(shmid2,NULL,0);
int pid=getpid();
kill(pid,SIGUSR2);
if(signal())
}