#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#define cl2 "./cl2"
#define cl21 "./cl21"


int main()
{
    
    int w,r,k,n;
   mkfifo(cl2,0644);
  mkfifo(cl21,0644);
   char buf[10];
int h=fork();

while(1){

if(h==0){
r=open(cl21,O_RDONLY);
       n=read(r,&buf ,10);
     write(1,&buf,n);   
     close(r);}
  else if (h>0){
        w=open(cl2,O_WRONLY);
        n=read(0,&buf,10);
       write(w,&buf,n);
       close(w);}

    }
}

