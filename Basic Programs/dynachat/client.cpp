#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>


int sshm,gshm,cshm,p1shm,l1shm,*grp,*len1,l2shm,*len2,p2shm,*cl,*s,ssem,csem,r,n,w,i,pno=0; 
char *p1,*p2,a[10],b[10];
char c,buf[10],pname[10],d[10];
struct sembuf sbuf,cbuf;

void *writes(void* ptr)
{
 while(1){
  w=open(a,O_WRONLY);
  n=read(0,&buf,10);
    write(w,&buf,n);
       close(w);}
}

void *reads(void* ptr)
{
while(1){
      r=open(b,O_RDONLY);
     n=read(r,&buf ,10);
     write(1,&buf,n);   
    close(r);}
}

void fun(int signo)
{
  for(i=0;i<*len1;i++)
  a[i]=*(p1+i);
  for(i=0;i<*len2;i++)
  b[i]=*(p2+i); 

   pthread_t t1,t2;  
  pthread_create(&t1,NULL,writes,NULL);
   pthread_create(&t2,NULL,reads,NULL);
  pthread_join(t1,NULL);
  pthread_join(t2,NULL);
}

int main(int argc,char *argv[])
{
    signal(SIGUSR1,fun);
   sshm=shmget(1200,sizeof(int),IPC_CREAT|0666);
   s=(int *)shmat(sshm,NULL,0);
    gshm=shmget(2000,sizeof(int),IPC_CREAT|0666);
   grp=(int *)shmat(gshm,NULL,0);
   p1shm=shmget(1400,10*sizeof(char),IPC_CREAT|0666);
   p1=(char *)shmat(p1shm,NULL,0);
   p2shm=shmget(1500,10*sizeof(char),IPC_CREAT|0666);
   p2=(char *)shmat(p2shm,NULL,0);
   l1shm=shmget(1600,sizeof(int),IPC_CREAT|0666);
   len1=(int *)shmat(l1shm,NULL,0);
   l2shm=shmget(1700,sizeof(int),IPC_CREAT|0666);
   len2=(int *)shmat(l2shm,NULL,0);
   cshm=shmget(1300,sizeof(int),IPC_CREAT|0666);
   cl=(int *)shmat(cshm,NULL,0);
   csem=semget(200,1,IPC_CREAT|0666);
   semctl(csem,0,SETVAL,1); 
   cbuf.sem_num=0;
   cbuf.sem_op=-1;
  semop(csem,&cbuf,1);
  *cl=getpid();
   *grp=atoi(argv[1]);
  cbuf.sem_op=1;
  semop(csem,&cbuf,1);
 
   kill(*s,SIGUSR2);
  while(1);
  
}

