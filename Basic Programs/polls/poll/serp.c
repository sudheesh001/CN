#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<poll.h>
#define cl1 "./cl1"
#define cl3 "./cl3"
#define cl2 "./cl2"
#define cl11 "./cl11"
#define cl31 "./cl31"
#define cl21 "./cl21"



int main()
{
   int r,w,n;
    struct pollfd fd[3];
   fd[0].fd=open(cl1,O_RDONLY);
    fd[1].fd=open(cl2,O_RDONLY);
    fd[2].fd=open(cl3,O_RDONLY);
  char buf[10];
  
   fd[0].events=POLLIN; fd[1].events=POLLIN; fd[2].events=POLLIN;
    
  

 
while(1){


poll(fd,3,50);

 if(fd[0].revents&POLLIN){

  n=read(fd[0].fd,&buf,10);
  
  w=open(cl21,O_WRONLY);
  write(w,&buf,n);
  close(w);
  w=open(cl31,O_WRONLY);
  write(w,&buf,n);
  close(w);
 }


if(fd[1].revents&POLLIN){
  n=read(fd[1].fd,&buf,10);
  
 w=open(cl11,O_WRONLY);
  write(w,&buf,n);
  close(w);
  w=open(cl31,O_WRONLY);
  write(w,&buf,n);
  close(w);}




if(fd[2].revents&POLLIN){


  n=read(fd[2].fd,&buf,10);  
 w=open(cl21,O_WRONLY);
  write(w,&buf,n);
  close(w);
  w=open(cl11,O_WRONLY);
  write(w,&buf,n);
  close(w);}

  
   
}   
}

