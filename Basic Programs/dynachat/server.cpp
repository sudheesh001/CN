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
#include <sys/select.h>
#include <signal.h>
#include <poll.h>


int sshm,cshm,*grp,gshm,p1shm,p2shm,l1shm,*len1,l2shm,*len2,*cl,*s,ssem,csem,r,n,w,pno=0,i,max=0,j; 
char *p1,*p2;
char c,buf[10],pname1[10],pname2[10],d[10];
struct sembuf sbuf,cbuf;
struct pollfd fdin[100];int fdout[100],group[100];
 

void addclient(int signo)
{

 strcpy(pname1,"./in");
  sprintf(d,"%d",pno);
  strcat(pname1,d);
  mkfifo(pname1,0666);
  *len1=strlen(pname1);

  for(i=0;i<strlen(pname1);i++)
  *(p1+i)=pname1[i];
 
  strcpy(pname2,"./out");
  sprintf(d,"%d",pno);
  strcat(pname2,d);
  mkfifo(pname2,0666);
  *len2=strlen(pname2);
  
  for(i=0;i<strlen(pname2);i++)
  *(p2+i)=pname2[i];

   kill(*cl,SIGUSR1);
   fdin[pno].fd=open(pname1,O_RDONLY);
  fdout[pno]=open(pname2,O_WRONLY);
 group[pno]=*grp;
   pno++;

}


int main()
{
  
   pthread_t t1,t2;
   signal(SIGUSR2,addclient);
  ssem=semget(100,1,IPC_CREAT|0666);
  semctl(ssem,0,SETVAL,1); 
    sshm=shmget(1200,sizeof(int),IPC_CREAT|0666);
    s=(int *)shmat(sshm,NULL,0);
    *(s)=getpid();
    gshm=shmget(2000,sizeof(int),IPC_CREAT|0666);
   grp=(int *)shmat(gshm,NULL,0);
    cshm=shmget(1300,sizeof(int),IPC_CREAT|0666);
    cl=(int *)shmat(cshm,NULL,0);
     p1shm=shmget(1400,10*sizeof(char),IPC_CREAT|0666);
   p1=(char *)shmat(p1shm,NULL,0);
   p2shm=shmget(1500,10*sizeof(char),IPC_CREAT|0666);
   p2=(char *)shmat(p2shm,NULL,0);
   l1shm=shmget(1600,sizeof(int),IPC_CREAT|0666);
   len1=(int *)shmat(l1shm,NULL,0);
   l2shm=shmget(1700,sizeof(int),IPC_CREAT|0666);
   len2=(int *)shmat(l2shm,NULL,0);
   
  
   while(1)
    {
     if(pno>=0){
     sbuf.sem_num=0;
  sbuf.sem_op=-1;
  semop(ssem,&sbuf,1);
     
    for(i=0;i<pno;i++)
        fdin[i].events=POLLIN;

 
   poll(fdin,pno,50);

     for( i=0;i<pno;i++){
      if(fdin[i].revents&POLLIN)
       {
     
          n=read(fdin[i].fd,&buf,10);
          for(j=0;j<pno;j++)
          {
              if(j!=i && group[j]==group[i]){
       
               write(fdout[j],&buf,n);}
      
        }
   
      }}
     sbuf.sem_num=0;
  sbuf.sem_op= 1;
  semop(ssem,&sbuf,1);

}

}

}
    

